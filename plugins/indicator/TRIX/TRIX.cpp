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

#include "TRIX.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

TRIX::TRIX ()
{
  pluginName = "TRIX";
  helpFile = "trix.html";
  
  setDefaults();
}

TRIX::~TRIX ()
{
}

void TRIX::setDefaults ()
{
  color.setNamedColor("red");
  trigColor.setNamedColor("yellow");
  lineType = PlotLine::Line;
  trigLineType = PlotLine::Dash;
  label = pluginName;
  trigLabel = QObject::tr("TRIX Trig");
  period = 12;
  tperiod = 9;
  input = BarData::Close;
  maType = IndicatorPlugin::SMA;  
}

void TRIX::calculate ()
{
  PlotLine *in = 0;
  if (customFlag)
    in = getInputLine(customInput);
  else
    in = data->getInput(input);
  if (! in)
  {
    qDebug("MOM::calculate: no input");
    return;
  }

  PlotLine *ema = getMA(in, IndicatorPlugin::EMA, period);
  
  PlotLine *ema2 = getMA(ema, IndicatorPlugin::EMA, period);
  
  PlotLine *ema3 = getMA(ema2, IndicatorPlugin::EMA, period);
  int emaLoop = ema3->getSize() - 1;

  PlotLine *trix = new PlotLine();

  while (emaLoop > 0)
  {
    trix->prepend(((ema3->getData(emaLoop) - ema3->getData(emaLoop - 1)) / ema3->getData(emaLoop - 1)) * 100);
    emaLoop--;
  }

  PlotLine *trigger = getMA(trix, maType, tperiod);
  trigger->setColor(trigColor);
  trigger->setType(trigLineType);
  trigger->setLabel(trigLabel);

  trix->setColor(color);
  trix->setType(lineType);
  trix->setLabel(label);
  output->addLine(trix);

  output->addLine(trigger);

  if (! customFlag)
    delete in;
  delete ema;
  delete ema2;
  delete ema3;
}

int TRIX::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("TRIX Indicator"));
  dialog->createPage (QObject::tr("Parms"));
  dialog->setHelpFile(helpFile);
  dialog->addColorItem(QObject::tr("Color"), QObject::tr("Parms"), color);
  dialog->addComboItem(QObject::tr("Line Type"), QObject::tr("Parms"), lineTypes, lineType);
  dialog->addTextItem(QObject::tr("Label"), QObject::tr("Parms"), label);
  dialog->addIntItem(QObject::tr("Period"), QObject::tr("Parms"), period, 1, 99999999);
  if (customFlag)
    dialog->addFormulaInputItem(QObject::tr("Input"), QObject::tr("Parms"), FALSE, customInput);
  else
    dialog->addComboItem(QObject::tr("Input"), QObject::tr("Parms"), inputTypeList, input);
  
  dialog->createPage (QObject::tr("Trigger Parms"));
  dialog->addColorItem(QObject::tr("Trigger Color"), QObject::tr("Trigger Parms"), trigColor);
  dialog->addComboItem(QObject::tr("Trigger Line Type"), QObject::tr("Trigger Parms"), lineTypes, trigLineType);
  dialog->addTextItem(QObject::tr("Trigger Label"), QObject::tr("Trigger Parms"), trigLabel);
  dialog->addIntItem(QObject::tr("Trigger Period"), QObject::tr("Trigger Parms"), tperiod, 1, 99999999);
  dialog->addComboItem(QObject::tr("Trigger Type"), QObject::tr("Trigger Parms"), maTypeList, maType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(QObject::tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("Line Type"));
    period = dialog->getInt(QObject::tr("Period"));
    label = dialog->getText(QObject::tr("Label"));
    if (customFlag)
      customInput = dialog->getFormulaInput(QObject::tr("Input"));
    else
      input = (BarData::InputType) dialog->getComboIndex(QObject::tr("Input"));
    
    trigColor = dialog->getColor(QObject::tr("Trigger Color"));
    trigLineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("Trigger Line Type"));
    tperiod = dialog->getInt(QObject::tr("Trigger Period"));
    trigLabel = dialog->getText(QObject::tr("Trigger Label"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(QObject::tr("Trigger Type"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void TRIX::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void TRIX::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void TRIX::setIndicatorSettings (Setting dict)
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
      
  s = dict.getData("input");
  if (s.length())
    input = (BarData::InputType) s.toInt();
  
  s = dict.getData("trigColor");
  if (s.length())
    trigColor.setNamedColor(s);
    
  s = dict.getData("trigLineType");
  if (s.length())
    trigLineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("tperiod");
  if (s.length())
    tperiod = s.toInt();

  s = dict.getData("trigLabel");
  if (s.length())
    trigLabel = s;
  
  s = dict.getData("maType");
  if (s.length())
    maType = (IndicatorPlugin::MAType) s.toInt();

  s = dict.getData("customInput");
  if (s.length())
    customInput = s;
}

Setting TRIX::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("period", QString::number(period));
  dict.setData("label", label);
  dict.setData("input", QString::number(input));

  dict.setData("trigColor", trigColor.name());
  dict.setData("trigLineType", QString::number(trigLineType));
  dict.setData("tperiod", QString::number(tperiod));
  dict.setData("trigLabel", trigLabel);
  dict.setData("maType", QString::number(maType));
  dict.setData("customInput", customInput);
  dict.setData("plugin", pluginName);
  return dict;
}

PlotLine * TRIX::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  return output->getLine(0);
}

int TRIX::getMinBars ()
{
  int t = minBars + period + period + period + tperiod;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  TRIX *o = new TRIX;
  return ((IndicatorPlugin *) o);
}


