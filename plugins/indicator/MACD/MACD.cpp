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

#include "MACD.h"
#include "PrefDialog.h"
#include <qdict.h>

MACD::MACD ()
{
  pluginName = "MACD";
  plotFlag = FALSE;
  setDefaults();
}

MACD::~MACD ()
{
}

void MACD::setDefaults ()
{
  macdColor.setNamedColor("red");
  trigColor.setNamedColor("yellow");
  oscColor.setNamedColor("blue");
  macdLineType = PlotLine::Line;
  trigLineType = PlotLine::Dash;
  oscLineType = PlotLine::Histogram;
  macdLabel = "MACD";
  trigLabel = "Trig";
  oscLabel = "Osc";
  fastPeriod = 12;
  slowPeriod = 26;
  trigPeriod = 9;
  macdMAType = IndicatorPlugin::EMA;  
  macdInput = BarData::Close;
  oscScaleFlag = FALSE;
}

void MACD::calculate ()
{
  PlotLine *d = 0;
  if (customFlag)
    d = getInputLine(customInput);
  else
    d = data->getInput(macdInput);
  if (! d)
  {
    qDebug("MACD::calculate: no input");
    return;
  }
  
  PlotLine *fma = getMA(d, macdMAType, fastPeriod);
  int fmaLoop = fma->getSize() - 1;

  PlotLine *sma = getMA(d, macdMAType, slowPeriod);
  int smaLoop = sma->getSize() - 1;
  
  if (! customFlag)
    delete d;

  PlotLine *macd = new PlotLine();
  macd->setColor(macdColor);
  macd->setType(macdLineType);
  macd->setLabel(macdLabel);
  
  while (fmaLoop > -1 && smaLoop > -1)
  {
    macd->prepend(fma->getData(fmaLoop) - sma->getData(smaLoop));
    fmaLoop--;
    smaLoop--;
  }
  delete fma;
  delete sma;

  PlotLine *signal = getMA(macd, macdMAType, trigPeriod);
  signal->setColor(trigColor);
  signal->setType(trigLineType);
  signal->setLabel(trigLabel);

  PlotLine *osc = new PlotLine();
  osc->setColor(oscColor);
  osc->setType(oscLineType);
  osc->setLabel(oscLabel);
  osc->setScaleFlag(oscScaleFlag);

  int floop = macd->getSize() - 1;
  int sloop = signal->getSize() - 1;

  while (floop > -1 && sloop > -1)
  {
    osc->prepend((macd->getData(floop) - signal->getData(sloop)));
    floop--;
    sloop--;
  }

  output.append(osc);
  output.append(macd);
  output.append(signal);
}

int MACD::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("MACD Indicator"));
  
  dialog->createPage (tr("MACD"));
  dialog->addColorItem(tr("MACD Color"), tr("MACD"), macdColor);
  dialog->addIntItem(tr("Fast Period"), tr("MACD"), fastPeriod, 1, 99999999);
  dialog->addIntItem(tr("Slow Period"), tr("MACD"), slowPeriod, 1, 99999999);
  dialog->addTextItem(tr("MACD Label"), tr("MACD"), macdLabel);
  dialog->addComboItem(tr("MACD Line Type"), tr("MACD"), lineTypes, macdLineType);
  dialog->addComboItem(tr("MACD MA Type"), tr("MACD"), maTypeList, macdMAType);
  if (customFlag)
    dialog->addFormulaInputItem(tr("MACD Input"), tr("MACD"), FALSE, customInput);
  else
    dialog->addComboItem(tr("MACD Input"), tr("MACD"), inputTypeList, macdInput);
  
  dialog->createPage (tr("Trigger"));
  dialog->addColorItem(tr("Trigger Color"), tr("Trigger"), trigColor);
  dialog->addIntItem(tr("Trigger Period"), tr("Trigger"), trigPeriod, 1, 99999999);
  dialog->addTextItem(tr("Trigger Label"), tr("Trigger"), trigLabel);
  dialog->addComboItem(tr("Trigger Line Type"), tr("Trigger"), lineTypes, trigLineType);
  
  dialog->createPage (tr("Osc"));
  dialog->addColorItem(tr("Osc Color"), tr("Osc"), oscColor);
  dialog->addTextItem(tr("Osc Label"), tr("Osc"), oscLabel);
  dialog->addComboItem(tr("Osc Line Type"), tr("Osc"), lineTypes, oscLineType);
  dialog->addCheckItem(tr("Osc Scaling Max"), tr("Osc"), oscScaleFlag);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    macdColor = dialog->getColor(tr("MACD Color"));
    fastPeriod = dialog->getInt(tr("Fast Period"));
    slowPeriod = dialog->getInt(tr("Slow Period"));
    macdLabel = dialog->getText(tr("MACD Label"));
    macdLineType = (PlotLine::LineType) dialog->getComboIndex(tr("MACD Line Type"));
    macdMAType = (IndicatorPlugin::MAType) dialog->getComboIndex(tr("MACD MA Type"));
    if (customFlag)
      customInput = dialog->getFormulaInput(tr("MACD Input"));
    else
      macdInput = (BarData::InputType) dialog->getComboIndex(tr("MACD Input"));
    
    trigColor = dialog->getColor(tr("Trigger Color"));
    trigPeriod = dialog->getInt(tr("Trigger Period"));
    trigLabel = dialog->getText(tr("Trigger Label"));
    trigLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Trigger Line Type"));
    
    oscColor = dialog->getColor(tr("Osc Color"));
    oscLabel = dialog->getText(tr("Osc Label"));
    oscLineType = (PlotLine::LineType) dialog->getComboIndex(tr("Osc Line Type"));
    oscScaleFlag = dialog->getCheck(tr("Osc Scaling Max"));
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void MACD::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void MACD::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void MACD::setIndicatorSettings (Setting dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("macdColor");
  if (s.length())
    macdColor.setNamedColor(s);
    
  s = dict.getData("fastPeriod");
  if (s.length())
    fastPeriod = s.toInt();
	
  s = dict.getData("slowPeriod");
  if (s.length())
    slowPeriod = s.toInt();
  
  s = dict.getData("macdLabel");
  if (s.length())
    macdLabel = s;
        
  s = dict.getData("macdLineType");
  if (s.length())
    macdLineType = (PlotLine::LineType) s.toInt();
        
  s = dict.getData("macdMAType");
  if (s.length())
    macdMAType = (IndicatorPlugin::MAType) s.toInt();
        
  s = dict.getData("macdInput");
  if (s.length())
    macdInput = (BarData::InputType) s.toInt();

  s = dict.getData("trigColor");
  if (s.length())
    trigColor.setNamedColor(s);
  
  s = dict.getData("trigPeriod");
  if (s.length())
    trigPeriod = s.toInt();
  
  s = dict.getData("trigLabel");
  if (s.length())
    trigLabel = s;
        
  s = dict.getData("trigLineType");
  if (s.length())
    trigLineType = (PlotLine::LineType) s.toInt();
        
  s = dict.getData("oscColor");
  if (s.length())
    oscColor.setNamedColor(s);
  
  s = dict.getData("oscLabel");
  if (s.length())
    oscLabel = s;
        
  s = dict.getData("oscLineType");
  if (s.length())
    oscLineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("oscScaleFlag");
  if (s.length())
    oscScaleFlag = s.toInt();

  s = dict.getData("customInput");
  if (s.length())
    customInput = s;
}

Setting MACD::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("macdColor", macdColor.name());
  dict.setData("fastPeriod", QString::number(fastPeriod));
  dict.setData("slowPeriod", QString::number(slowPeriod));
  dict.setData("macdLabel", macdLabel);
  dict.setData("macdLineType", QString::number(macdLineType));
  dict.setData("macdMAType", QString::number(macdMAType));
  dict.setData("macdInput", QString::number(macdInput));
  
  dict.setData("trigColor", trigColor.name());
  dict.setData("trigPeriod", QString::number(trigPeriod));
  dict.setData("trigLabel", trigLabel);
  dict.setData("trigLineType", QString::number(trigLineType));
  
  dict.setData("oscColor", oscColor.name());
  dict.setData("oscLabel", oscLabel);
  dict.setData("oscLineType", QString::number(oscLineType));
  dict.setData("oscScaleFlag", QString::number(oscScaleFlag));
  
  dict.setData("customInput", customInput);
  dict.setData("plugin", pluginName);
  return dict;
}

PlotLine * MACD::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  return output.at(1);
}

Plugin * create ()
{
  MACD *o = new MACD;
  return ((Plugin *) o);
}

