/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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
  PlotLine *in = 0;
  if (customFlag)
    in = getInputLine(customInput);
  else
    in = data->getInput(input);
  if (! in)
  {
    qDebug("MAOSC::calculate: no input");
    return;
  }
  
  PlotLine *fma = getMA(in, fastMaType, fastPeriod);
  int fmaLoop = fma->getSize() - 1;

  PlotLine *sma = getMA(in, slowMaType, slowPeriod);
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
  
  if (! customFlag)
    delete in;
  
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
  if (customFlag)
    dialog->addFormulaInputItem(il, pl, FALSE, customInput);
  else
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
    if (customFlag)
      customInput = dialog->getFormulaInput(il);
    else
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

  s = dict.getData("customInput");
  if (s.length())
    customInput = s;
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
  dict.setData("customInput", customInput);
  dict.setData("plugin", pluginName);
}

PlotLine * MAOSC::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
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

