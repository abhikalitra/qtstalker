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
  fastMaType = IndicatorPlugin::SMA;  
  slowMaType = IndicatorPlugin::SMA;  
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
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("MAOSC Indicator"));
  dialog->createPage (QObject::tr("Parms"));
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(QObject::tr("Color"), QObject::tr("Parms"), color);
  dialog->addComboItem(QObject::tr("Line Type"), QObject::tr("Parms"), lineTypes, lineType);
  dialog->addTextItem(QObject::tr("Label"), QObject::tr("Parms"), label);
  dialog->addIntItem(QObject::tr("Fast Period"), QObject::tr("Parms"), fastPeriod, 1, 99999999);
  dialog->addIntItem(QObject::tr("Slow Period"), QObject::tr("Parms"), slowPeriod, 1, 99999999);
  dialog->addComboItem(QObject::tr("Fast MA Type"), QObject::tr("Parms"), maTypeList, fastMaType);
  dialog->addComboItem(QObject::tr("Slow MA Type"), QObject::tr("Parms"), maTypeList, slowMaType);
  if (customFlag)
    dialog->addFormulaInputItem(QObject::tr("Input"), QObject::tr("Parms"), FALSE, customInput);
  else
    dialog->addComboItem(QObject::tr("Input"), QObject::tr("Parms"), inputTypeList, input);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(QObject::tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("Line Type"));
    fastPeriod = dialog->getInt(QObject::tr("Fast Period"));
    slowPeriod = dialog->getInt(QObject::tr("Slow Period"));
    label = dialog->getText(QObject::tr("Label"));
    fastMaType = (IndicatorPlugin::MAType) dialog->getComboIndex(QObject::tr("Fast MA Type"));
    slowMaType = (IndicatorPlugin::MAType) dialog->getComboIndex(QObject::tr("Slow MA Type"));
    if (customFlag)
      customInput = dialog->getFormulaInput(QObject::tr("Input"));
    else
      input = (BarData::InputType) dialog->getComboIndex(QObject::tr("Input"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void MAOSC::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void MAOSC::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void MAOSC::setIndicatorSettings (Setting dict)
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
    fastMaType = (IndicatorPlugin::MAType) s.toInt();
    
  s = dict.getData("slowMaType");
  if (s.length())
    slowMaType = (IndicatorPlugin::MAType) s.toInt();
  
  s = dict.getData("input");
  if (s.length())
    input = (BarData::InputType) s.toInt();

  s = dict.getData("customInput");
  if (s.length())
    customInput = s;
}

Setting MAOSC::getIndicatorSettings ()
{
  Setting dict;
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
  return dict;
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

