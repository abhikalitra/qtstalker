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

#include "ENV.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

ENV::ENV ()
{
  pluginName = "ENV";
  
  bandList.append(QObject::tr("Upper"));
  bandList.append(QObject::tr("Lower"));
  
  setDefaults();
}

ENV::~ENV ()
{
}

void ENV::setDefaults ()
{
  upperColor.setNamedColor("red");
  lowerColor.setNamedColor("red");
  upperLineType = PlotLine::Line;
  lowerLineType = PlotLine::Line;
  upperLabel = QObject::tr("ENVU");
  lowerLabel = QObject::tr("ENVL");
  period = 10;
  upperPercent = 1.02;
  lowerPercent = 0.98;
  input = BarData::Close;
  maType = IndicatorPlugin::SMA;
  customBand = QObject::tr("Upper");
  customInput = "1";
  label = pluginName;
}

void ENV::calculate ()
{
  PlotLine *in = 0;
  if (customFlag)
    in = getInputLine(customInput);
  else
    in = data->getInput(input);
  if (! in)
  {
    qDebug("ENV::calculate: no input");
    return;
  }

  PlotLine *uma = getMA(in, maType, period);

  PlotLine *lma = getMA(in, maType, period);

  int maLoop = uma->getSize() - 1;

  while (maLoop > -1)
  {
    double t = uma->getData(maLoop);
    uma->setData(maLoop, t * upperPercent);

    t = lma->getData(maLoop);
    lma->setData(maLoop, t * lowerPercent);

    maLoop--;
  }

  if (! customFlag)
    delete in;

  uma->setColor(upperColor);
  uma->setType(upperLineType);
  uma->setLabel(upperLabel);
  output->addLine(uma);

  lma->setColor(lowerColor);
  lma->setType(lowerLineType);
  lma->setLabel(lowerLabel);
  output->addLine(lma);
}

int ENV::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("ENV Indicator"));
  dialog->createPage (QObject::tr("Parms"));
  dialog->addIntItem(QObject::tr("Period"), QObject::tr("Parms"), period, 1, 99999999);
  dialog->addComboItem(QObject::tr("MA Type"), QObject::tr("Parms"), maTypeList, maType);
  if (customFlag)
  {
    dialog->addTextItem(QObject::tr("Label"), QObject::tr("Parms"), label);
    dialog->addComboItem(QObject::tr("Plot"), QObject::tr("Parms"), bandList, customBand);
    dialog->addFormulaInputItem(QObject::tr("Input"), QObject::tr("Parms"), FALSE, customInput);
  }
  else
    dialog->addComboItem(QObject::tr("Input"), QObject::tr("Parms"), inputTypeList, input);
  
  dialog->createPage (QObject::tr("Upper"));
  dialog->addColorItem(QObject::tr("Upper Color"), QObject::tr("Upper"), upperColor);
  dialog->addComboItem(QObject::tr("Upper Line Type"), QObject::tr("Upper"), lineTypes, upperLineType);
  dialog->addTextItem(QObject::tr("Upper Label"), QObject::tr("Upper"), upperLabel);
  dialog->addFloatItem(QObject::tr("Upper %"), QObject::tr("Upper"), upperPercent, 0, 99999999.0);
  
  dialog->createPage (QObject::tr("Lower"));
  dialog->addColorItem(QObject::tr("Lower Color"), QObject::tr("Lower"), lowerColor);
  dialog->addComboItem(QObject::tr("Lower Line Type"), QObject::tr("Lower"), lineTypes, lowerLineType);
  dialog->addTextItem(QObject::tr("Lower Label"), QObject::tr("Lower"), lowerLabel);
  dialog->addFloatItem(QObject::tr("Lower %"), QObject::tr("Lower"), lowerPercent, 0, 99999999.0);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    period = dialog->getInt(QObject::tr("Period"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(QObject::tr("MA Type"));
    if (customFlag)
    {
      label = dialog->getText(QObject::tr("Label"));
      customBand = dialog->getCombo(QObject::tr("Plot"));
      customInput = dialog->getFormulaInput(QObject::tr("Input"));
    }
    else
      input = (BarData::InputType) dialog->getComboIndex(QObject::tr("Input"));
    
    upperColor = dialog->getColor(QObject::tr("Upper Color"));
    upperLineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("Upper Line Type"));
    upperLabel = dialog->getText(QObject::tr("Upper Label"));
    upperPercent = dialog->getFloat(QObject::tr("Upper %"));
    
    lowerColor = dialog->getColor(QObject::tr("Lower Color"));
    lowerLineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("Lower Line Type"));
    lowerLabel = dialog->getText(QObject::tr("Lower Label"));
    lowerPercent = dialog->getFloat(QObject::tr("Lower %"));
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void ENV::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void ENV::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void ENV::setIndicatorSettings (Setting dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("period");
  if (s.length())
    period = s.toInt();
  
  s = dict.getData("maType");
  if (s.length())
    maType = (IndicatorPlugin::MAType) s.toInt();

  s = dict.getData("input");
  if (s.length())
    input = (BarData::InputType) s.toInt();
      
  s = dict.getData("upperColor");
  if (s.length())
    upperColor.setNamedColor(s);
    
  s = dict.getData("upperLineType");
  if (s.length())
    upperLineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("upperLabel");
  if (s.length())
    upperLabel = s;
      
  s = dict.getData("upperPercent");
  if (s.length())
    upperPercent = s.toFloat();
    
  s = dict.getData("lowerColor");
  if (s.length())
    lowerColor.setNamedColor(s);
    
  s = dict.getData("lowerLineType");
  if (s.length())
    lowerLineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("lowerLabel");
  if (s.length())
    lowerLabel = s;
      
  s = dict.getData("lowerPercent");
  if (s.length())
    lowerPercent = s.toFloat();

  s = dict.getData("customBand");
  if (s.length())
    customBand = s;

  s = dict.getData("customInput");
  if (s.length())
    customInput = s;

  s = dict.getData("label");
  if (s.length())
    label = s;
}

Setting ENV::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("period", QString::number(period));
  dict.setData("maType", QString::number(maType));
  dict.setData("input", QString::number(input));
  dict.setData("label", label);
  
  dict.setData("upperColor", upperColor.name());
  dict.setData("upperLineType", QString::number(upperLineType));
  dict.setData("upperLabel", upperLabel);
  dict.setData("upperPercent", QString::number(upperPercent));
  
  dict.setData("lowerColor", lowerColor.name());
  dict.setData("lowerLineType", QString::number(lowerLineType));
  dict.setData("lowerLabel", lowerLabel);
  dict.setData("lowerPercent", QString::number(lowerPercent));

  dict.setData("customBand", customBand);
  dict.setData("customInput", customInput);
  dict.setData("plugin", pluginName);
  return dict;
}

PlotLine * ENV::calculateCustom (QDict<PlotLine> *d)
{
  customLines = d;
  clearOutput();
  calculate();
  if (! customBand.compare(QObject::tr("Upper")))
    return output->getLine(0);
  else
    return output->getLine(1);
}

IndicatorPlugin * createIndicatorPlugin ()
{
  ENV *o = new ENV;
  return ((IndicatorPlugin *) o);
}


