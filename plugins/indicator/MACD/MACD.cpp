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
#include <qobject.h>

MACD::MACD ()
{
  pluginName = "MACD";
  helpFile = "macd.html";
  
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
  label = macdLabel;
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

  output->addLine(osc);
  output->addLine(macd);
  output->addLine(signal);
}

int MACD::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("MACD Indicator"));
  dialog->setHelpFile(helpFile);
  
  dialog->createPage (QObject::tr("MACD"));
  dialog->addColorItem(QObject::tr("MACD Color"), QObject::tr("MACD"), macdColor);
  dialog->addIntItem(QObject::tr("Fast Period"), QObject::tr("MACD"), fastPeriod, 1, 99999999);
  dialog->addIntItem(QObject::tr("Slow Period"), QObject::tr("MACD"), slowPeriod, 1, 99999999);
  dialog->addTextItem(QObject::tr("MACD Label"), QObject::tr("MACD"), macdLabel);
  dialog->addComboItem(QObject::tr("MACD Line Type"), QObject::tr("MACD"), lineTypes, macdLineType);
  dialog->addComboItem(QObject::tr("MACD MA Type"), QObject::tr("MACD"), maTypeList, macdMAType);
  if (customFlag)
    dialog->addFormulaInputItem(QObject::tr("MACD Input"), QObject::tr("MACD"), FALSE, customInput);
  else
    dialog->addComboItem(QObject::tr("MACD Input"), QObject::tr("MACD"), inputTypeList, macdInput);
  
  dialog->createPage (QObject::tr("Trigger"));
  dialog->addColorItem(QObject::tr("Trigger Color"), QObject::tr("Trigger"), trigColor);
  dialog->addIntItem(QObject::tr("Trigger Period"), QObject::tr("Trigger"), trigPeriod, 1, 99999999);
  dialog->addTextItem(QObject::tr("Trigger Label"), QObject::tr("Trigger"), trigLabel);
  dialog->addComboItem(QObject::tr("Trigger Line Type"), QObject::tr("Trigger"), lineTypes, trigLineType);
  
  dialog->createPage (QObject::tr("Osc"));
  dialog->addColorItem(QObject::tr("Osc Color"), QObject::tr("Osc"), oscColor);
  dialog->addTextItem(QObject::tr("Osc Label"), QObject::tr("Osc"), oscLabel);
  dialog->addComboItem(QObject::tr("Osc Line Type"), QObject::tr("Osc"), lineTypes, oscLineType);
  dialog->addCheckItem(QObject::tr("Osc Scaling Max"), QObject::tr("Osc"), oscScaleFlag);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    macdColor = dialog->getColor(QObject::tr("MACD Color"));
    fastPeriod = dialog->getInt(QObject::tr("Fast Period"));
    slowPeriod = dialog->getInt(QObject::tr("Slow Period"));
    macdLabel = dialog->getText(QObject::tr("MACD Label"));
    label = macdLabel;
    macdLineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("MACD Line Type"));
    macdMAType = (IndicatorPlugin::MAType) dialog->getComboIndex(QObject::tr("MACD MA Type"));
    if (customFlag)
      customInput = dialog->getFormulaInput(QObject::tr("MACD Input"));
    else
      macdInput = (BarData::InputType) dialog->getComboIndex(QObject::tr("MACD Input"));
    
    trigColor = dialog->getColor(QObject::tr("Trigger Color"));
    trigPeriod = dialog->getInt(QObject::tr("Trigger Period"));
    trigLabel = dialog->getText(QObject::tr("Trigger Label"));
    trigLineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("Trigger Line Type"));
    
    oscColor = dialog->getColor(QObject::tr("Osc Color"));
    oscLabel = dialog->getText(QObject::tr("Osc Label"));
    oscLineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("Osc Line Type"));
    oscScaleFlag = dialog->getCheck(QObject::tr("Osc Scaling Max"));
    
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

  s = dict.getData("label");
  if (s.length())
    label = s;
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
  dict.setData("label", label);
  dict.setData("plugin", pluginName);
  return dict;
}

PlotLine * MACD::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  return output->getLine(1);
}

IndicatorPlugin * createIndicatorPlugin ()
{
  MACD *o = new MACD;
  return ((IndicatorPlugin *) o);
}

