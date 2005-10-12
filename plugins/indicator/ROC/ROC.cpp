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

#include "ROC.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

ROC::ROC ()
{
  pluginName = "ROC";
  helpFile = "roc.html";
  
  setDefaults();
}

ROC::~ROC ()
{
}

void ROC::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Histogram;
  label = pluginName;
  period = 14;
  smoothing = 10;
  maType = 1;  
  input = BarData::Close;
}

void ROC::calculate ()
{
  PlotLine *in = data->getInput(input);
  if (! in)
  {
    qDebug("ROC::calculate: no input");
    return;
  }

  calculate2(in);
  delete in;
}

void ROC::calculate2 (PlotLine *in)
{
  PlotLine *roc = new PlotLine();

  int loop;
  for (loop = period; loop < (int) in->getSize(); loop++)
    roc->append(((in->getData(loop) - in->getData(loop - period)) / in->getData(loop - period)) * 100);

  if (smoothing > 1)
  {
    PlotLine *ma = getMA(roc, maType, smoothing, 0, 0);
    ma->setColor(color);
    ma->setType(lineType);
    ma->setLabel(label);
    output->addLine(ma);
    delete roc;
  }
  else
  {
    roc->setColor(color);
    roc->setType(lineType);
    roc->setLabel(label);
    output->addLine(roc);
  }
}

int ROC::indicatorPrefDialog (QWidget *w)
{
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString perl = QObject::tr("Period");
  QString sl = QObject::tr("Smoothing");
  QString stl = QObject::tr("Smoothing Type");
  QString il = QObject::tr("Input");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("ROC Indicator"));
  dialog->createPage (pl);
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(cl, pl, color);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addTextItem(ll, pl, label);
  dialog->addIntItem(perl, pl, period, 1, 99999999);
  QStringList l = getMATypes();
  dialog->addComboItem(il, pl, inputTypeList, input);
  dialog->addIntItem(sl, pl, smoothing, 0, 99999999);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    period = dialog->getInt(perl);
    label = dialog->getText(ll);
    maType = dialog->getComboIndex(stl);
    smoothing = dialog->getInt(sl);
    input = (BarData::InputType) dialog->getComboIndex(il);
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void ROC::setIndicatorSettings (Setting &dict)
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

  s = dict.getData("period");
  if (s.length())
    period = s.toInt();

  s = dict.getData("label");
  if (s.length())
    label = s;
      
  s = dict.getData("maType");
  if (s.length())
    maType = s.toInt();
    
  s = dict.getData("input");
  if (s.length())
    input = (BarData::InputType) s.toInt();
    
  s = dict.getData("smoothing");
  if (s.length())
    smoothing = s.toInt();
}

void ROC::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("period", QString::number(period));
  dict.setData("label", label);
  dict.setData("maType", QString::number(maType));
  dict.setData("input", QString::number(input));
  dict.setData("smoothing", QString::number(smoothing));
  dict.setData("plugin", pluginName);
}

PlotLine * ROC::calculateCustom (QString &p, QPtrList<PlotLine> &d)
{
  // format1: ARRAY_INPUT, MA_TYPE, PERIOD, SMOOTHING

  QStringList l = QStringList::split(",", p, FALSE);

  if (l.count() == 4)
    ;
  else
  {
    qDebug("ROC::calculateCustom: invalid parm count");
    return 0;
  }

  if (! d.count())
  {
    qDebug("ROC::calculateCustom: no input");
    return 0;
  }

  QStringList mal = getMATypes();
  if (mal.findIndex(l[1]) == -1)
  {
    qDebug("ROC::calculateCustom: invalid MA_TYPE parm");
    return 0;
  }
  else
    maType = mal.findIndex(l[1]);

  bool ok;
  int t = l[2].toInt(&ok);
  if (ok)
    period = t;
  else
  {
    qDebug("ROC::calculateCustom: invalid PERIOD parm");
    return 0;
  }

  t = l[3].toInt(&ok);
  if (ok)
    smoothing = t;
  else
  {
    qDebug("ROC::calculateCustom: invalid SMOOTHING parm");
    return 0;
  }

  clearOutput();
  calculate2(d.at(0));
  return output->getLine(0);
}

int ROC::getMinBars ()
{
  int t = minBars + period + smoothing;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  ROC *o = new ROC;
  return ((IndicatorPlugin *) o);
}




