/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#include "MAOSC.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

MAOSC::MAOSC ()
{
  pluginName = "MAOSC";
  helpFile = "maosc.html";
  
  setDefaults();
}

MAOSC::~MAOSC ()
{
}

void MAOSC::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Histogram;
  label = pluginName;
  fastPeriod = 9;
  slowPeriod = 18;
  fastMaType = 1;  
  slowMaType = 1;  
  input = BarData::Close;
}

void MAOSC::calculate ()
{
  PlotLine *in = data->getInput(input);
  if (! in)
  {
    qDebug("MAOSC::calculate: no input");
    return;
  }

  calculate2(in);
  delete in;
}

void MAOSC::calculate2 (PlotLine *in)
{
  PlotLine *fma = getMA(in, fastMaType, fastPeriod, 0, 0);
  int fmaLoop = fma->getSize() - 1;

  PlotLine *sma = getMA(in, slowMaType, slowPeriod, 0, 0);
  int smaLoop = sma->getSize() - 1;

  PlotLine *osc = new PlotLine();
  osc->setColor(color);
  osc->setType(lineType);
  osc->setLabel(label);
  
  while (fmaLoop > -1 && smaLoop > -1)
  {
    osc->prepend(fma->getData(fmaLoop) - sma->getData(smaLoop));
    fmaLoop--;
    smaLoop--;
  }

  output->addLine(osc);
  
  delete fma;
  delete sma;
}

int MAOSC::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString fperl = QObject::tr("Fast Period");
  QString sperl = QObject::tr("Slow Period");
  QString fml = QObject::tr("Fast MA Type");
  QString sml = QObject::tr("Slow MA Type");
  QString il = QObject::tr("Input");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("MAOSC Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addTextItem(ll, pl, label);
  dialog->addIntItem(fperl, pl, fastPeriod, 1, 99999999);
  dialog->addIntItem(sperl, pl, slowPeriod, 1, 99999999);
  QStringList l = getMATypes();
  dialog->addComboItem(fml, pl, l, fastMaType);
  dialog->addComboItem(sml, pl, l, slowMaType);
  dialog->addComboItem(il, pl, inputTypeList, input);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    fastPeriod = dialog->getInt(fperl);
    slowPeriod = dialog->getInt(sperl);
    label = dialog->getText(ll);
    fastMaType = dialog->getComboIndex(fml);
    slowMaType = dialog->getComboIndex(sml);
    input = (BarData::InputType) dialog->getComboIndex(il);
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void MAOSC::setIndicatorSettings (Setting &dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("color");
  if (s.length())
    color.setNamedColor(s);
    
  s = dict.getData("lineType");
  if (s.length())
    lineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("fastPeriod");
  if (s.length())
    fastPeriod = s.toInt();

  s = dict.getData("slowPeriod");
  if (s.length())
    slowPeriod = s.toInt();
  
  s = dict.getData("label");
  if (s.length())
    label = s;
      
  s = dict.getData("fastMaType");
  if (s.length())
    fastMaType = s.toInt();
    
  s = dict.getData("slowMaType");
  if (s.length())
    slowMaType = s.toInt();
  
  s = dict.getData("input");
  if (s.length())
    input = (BarData::InputType) s.toInt();
}

void MAOSC::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("fastPeriod", QString::number(fastPeriod));
  dict.setData("slowPeriod", QString::number(slowPeriod));
  dict.setData("label", label);
  dict.setData("fastMaType", QString::number(fastMaType));
  dict.setData("slowMaType", QString::number(slowMaType));
  dict.setData("input", QString::number(input));
  dict.setData("plugin", pluginName);
}

PlotLine * MAOSC::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format1: ARRAY_INPUT, FAST_MA_TYPE, SLOW_MA_TYPE, FAST_PERIOD, SLOW_PERIOD

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 5)
    ;
  else
  {
    qDebug("MAOSC::calculateCustom: invalid parm count");
    return 0;
  }

  if (! d.count())
  {
    qDebug("MAOSC::calculateCustom: no input");
    return 0;
  }

  QStringList mal = getMATypes();
  if (mal.findIndex(l[1]) == -1)
  {
    qDebug("MAOSC::calculateCustom: invalid FAST_MA_TYPE parm");
    return 0;
  }
  else
    fastMaType = mal.findIndex(l[1]);

  if (mal.findIndex(l[2]) == -1)
  {
    qDebug("MAOSC::calculateCustom: invalid SLOW_MA_TYPE parm");
    return 0;
  }
  else
    slowMaType = mal.findIndex(l[2]);

  bool ok;
  int t = l[3].toInt(&ok);
  if (ok)
    fastPeriod = t;
  else
  {
    qDebug("MAOSC::calculateCustom: invalid FAST_PERIOD parm");
    return 0;
  }

  t = l[4].toInt(&ok);
  if (ok)
    slowPeriod = t;
  else
  {
    qDebug("MAOSC::calculateCustom: invalid SLOW_PERIOD parm");
    return 0;
  }

  clearOutput();
  calculate2(d.at(0));
  return output->getLine(0);
}

int MAOSC::getMinBars ()
{
  int t = 0;
  if (fastPeriod > t)
    t = fastPeriod;
  if (slowPeriod > t)
    t = slowPeriod;
  t++;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  MAOSC *o = new MAOSC;
  return ((IndicatorPlugin *) o);
}

