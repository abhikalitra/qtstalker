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

/* The "Market Thermometer" indicator is described in
   Dr. Alexander Elder's book _Come Into My Trading Room_, p.162 */

#include "THERM.h"
#include <math.h>
#include "PrefDialog.h"
#include <qdict.h>

THERM::THERM ()
{
  pluginName = "THERM";
  plotFlag = FALSE;
  setDefaults();
}

THERM::~THERM ()
{
}

void THERM::setDefaults ()
{
  downColor.setNamedColor("green");
  upColor.setNamedColor("magenta");
  threshColor.setNamedColor("red");
  maColor.setNamedColor("yellow");
  lineType = PlotLine::HistogramBar;
  maLineType = PlotLine::Line;
  label = pluginName;
  maLabel = tr("THERM MA");
  threshold = 3;
  smoothing = 2;
  maPeriod = 22;
  maType = IndicatorPlugin::EMA;
  smoothType = IndicatorPlugin::EMA;
}

void THERM::calculate ()
{
  PlotLine *therm = new PlotLine();
  int loop;
  double thermometer = 0;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    double high = fabs(data->getHigh(loop) - data->getHigh(loop - 1));
    double lo = fabs(data->getLow(loop - 1) - data->getLow(loop));
    
    if (high > lo)
      thermometer = high;
    else
      thermometer = lo;

    therm->append(thermometer);
  }

  if (smoothing > 1)
  {
    PlotLine *ma = getMA(therm, smoothType, smoothing);
    output.append(ma);
    delete therm;
    therm = ma;
  }
  else
    output.append(therm);

  PlotLine *therm_ma = getMA(therm, maType, maPeriod);
  therm_ma->setColor(maColor);
  therm_ma->setType(maLineType);
  therm_ma->setLabel(maLabel);
  output.append(therm_ma);

  // assign the therm colors

  therm->setColorFlag(TRUE);
  therm->setType(lineType);
  therm->setLabel(label);

  int thermLoop = therm->getSize() - 1;
  int maLoop = therm_ma->getSize() - 1;
  while (thermLoop > -1)
  {
    if (maLoop > -1)
    {
      double thrm = therm->getData(thermLoop);
      double thrmma = therm_ma->getData(maLoop);

      if (thrm > (thrmma * threshold))
        therm->prependColorBar(threshColor);
      else
      {
        if (thrm > thrmma)
          therm->prependColorBar(upColor);
        else
          therm->prependColorBar(downColor);
      }
    }
    else
      therm->prependColorBar(downColor);

    thermLoop--;
    maLoop--;
  }
}

int THERM::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("THERM Indicator"));

  dialog->createPage (tr("THERM Parms"));
  dialog->addColorItem(tr("Color Above MA"), tr("THERM Parms"), upColor);
  dialog->addColorItem(tr("Color Below MA"), tr("THERM Parms"), downColor);
  dialog->addColorItem(tr("Color Threshold"), tr("THERM Parms"), threshColor);
  dialog->addTextItem(tr("Label"), tr("THERM Parms"), label);
  dialog->addFloatItem(tr("Threshold"), tr("THERM Parms"), threshold, 1, 99999999);
  dialog->addIntItem(tr("Smoothing"), tr("THERM Parms"), smoothing, 0, 99999999);
  dialog->addComboItem(tr("Smoothing Type"), tr("THERM Parms"), maTypeList, smoothType);
  
  dialog->createPage (tr("MA Parms"));
  dialog->addColorItem(tr("MA Color"), tr("MA Parms"), maColor);
  dialog->addComboItem(tr("MA Line Type"), tr("MA Parms"), lineTypes, maLineType);
  dialog->addTextItem(tr("MA Label"), tr("MA Parms"), maLabel);
  dialog->addIntItem(tr("MA Period"), tr("MA Parms"), maPeriod, 0, 99999999);
  dialog->addComboItem(tr("MA Type"), tr("MA Parms"), maTypeList, maType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    upColor = dialog->getColor(tr("Color Above MA"));
    downColor = dialog->getColor(tr("Color Below MA"));
    threshColor = dialog->getColor(tr("Color Threshold"));
    label = dialog->getText(tr("Label"));
    threshold = dialog->getFloat(tr("Threshold"));
    smoothing = dialog->getInt(tr("Smoothing"));
    smoothType = (IndicatorPlugin::MAType) dialog->getComboIndex(tr("Smoothing Type"));
    
    maColor = dialog->getColor(tr("MA Color"));
    maLineType = (PlotLine::LineType) dialog->getComboIndex(tr("MA Line Type"));
    maLabel = dialog->getText(tr("MA Label"));
    maPeriod = dialog->getInt(tr("MA Period"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(tr("MA Type"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void THERM::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void THERM::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void THERM::setIndicatorSettings (Setting dict)
{
  setDefaults();
  
  if (! dict.count())
    return;
  
  QString s = dict.getData("upColor");
  if (s.length())
    upColor.setNamedColor(s);
    
  s = dict.getData("downColor");
  if (s.length())
    downColor.setNamedColor(s);
  
  s = dict.getData("threshColor");
  if (s.length())
    threshColor.setNamedColor(s);
  
  s = dict.getData("maColor");
  if (s.length())
    maColor.setNamedColor(s);
    
  s = dict.getData("label");
  if (s.length())
    label = s;
    
  s = dict.getData("threshold");
  if (s.length())
    threshold = s.toFloat();
  
  s = dict.getData("smoothing");
  if (s.length())
    smoothing = s.toInt();
  
  s = dict.getData("smoothType");
  if (s.length())
    smoothType = (IndicatorPlugin::MAType) s.toInt();
  
  s = dict.getData("maLineType");
  if (s.length())
    maLineType = (PlotLine::LineType) s.toInt();

  s = dict.getData("maLabel");
  if (s.length())
    maLabel = s;
    
  s = dict.getData("maPeriod");
  if (s.length())
    maPeriod = s.toInt();

  s = dict.getData("maType");
  if (s.length())
    maType = (IndicatorPlugin::MAType) s.toInt();
}

Setting THERM::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("upColor", upColor.name());
  dict.setData("downColor", downColor.name());
  dict.setData("threshColor", threshColor.name());
  dict.setData("label", label);
  dict.setData("threshold", QString::number(threshold));
  dict.setData("smoothing", QString::number(smoothing));
  dict.setData("smoothType", QString::number(smoothType));
  
  dict.setData("maColor", maColor.name());
  dict.setData("maLineType", QString::number(maLineType));
  dict.setData("maPeriod", QString::number(maPeriod));
  dict.setData("maLabel", maLabel);
  dict.setData("maType", QString::number(maType));
  dict.setData("plugin", pluginName);
  return dict;
}

PlotLine * THERM::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  return output.at(0);
}

Plugin * create ()
{
  THERM *o = new THERM;
  return ((Plugin *) o);
}

/* Alert Notes

1) enter when therm falls below MA
2) exit when threshold is triggered
3) explosive move expected when therm stays below MA for 5-7 days

*/


