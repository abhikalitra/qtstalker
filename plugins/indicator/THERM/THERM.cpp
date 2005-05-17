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

/* The "Market Thermometer" indicator is described in
   Dr. Alexander Elder's book _Come Into My Trading Room_, p.162 */

#include "THERM.h"
#include <math.h>
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>

THERM::THERM ()
{
  pluginName = "THERM";
  helpFile = "therm.html";
  
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
  maLabel = QObject::tr("THERM MA");
  threshold = 3;
  smoothing = 2;
  maPeriod = 22;
  maType = 0;
  smoothType = 0;
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
    output->addLine(ma);
    delete therm;
    therm = ma;
  }
  else
    output->addLine(therm);

  PlotLine *therm_ma = getMA(therm, maType, maPeriod);
  therm_ma->setColor(maColor);
  therm_ma->setType(maLineType);
  therm_ma->setLabel(maLabel);
  output->addLine(therm_ma);

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
  dialog->setCaption(QObject::tr("THERM Indicator"));
  dialog->setHelpFile(helpFile);

  QString pl = QObject::tr("THERM Parms");
  dialog->createPage (pl);
  QString t = QObject::tr("Color Above MA");
  dialog->addColorItem(t, pl, upColor);
  t = QObject::tr("Color Below MA");
  dialog->addColorItem(t, pl, downColor);
  t = QObject::tr("Color Threshold");
  dialog->addColorItem(t, pl, threshColor);
  t = QObject::tr("Label");
  dialog->addTextItem(t, pl, label);
  t = QObject::tr("Threshold");
  dialog->addFloatItem(t, pl, threshold, 1, 99999999);
  t = QObject::tr("Smoothing");
  dialog->addIntItem(t, pl, smoothing, 0, 99999999);
  QStringList l = getMATypes();
  t = QObject::tr("Smoothing Type");
  dialog->addComboItem(t, pl, l, smoothType);
  
  pl = QObject::tr("MA Parms");
  dialog->createPage (pl);
  t = QObject::tr("MA Color");
  dialog->addColorItem(t, pl, maColor);
  t = QObject::tr("MA Line Type");
  dialog->addComboItem(t, pl, lineTypes, maLineType);
  t = QObject::tr("MA Label");
  dialog->addTextItem(t, pl, maLabel);
  t = QObject::tr("MA Period");
  dialog->addIntItem(t, pl, maPeriod, 0, 99999999);
  t = QObject::tr("MA Type");
  dialog->addComboItem(t, pl, l, maType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    t = QObject::tr("Color Above MA");
    upColor = dialog->getColor(t);
    t = QObject::tr("Color Below MA");
    downColor = dialog->getColor(t);
    t = QObject::tr("Color Threshold");
    threshColor = dialog->getColor(t);
    t = QObject::tr("Label");
    label = dialog->getText(t);
    t = QObject::tr("Threshold");
    threshold = dialog->getFloat(t);
    t = QObject::tr("Smoothing");
    smoothing = dialog->getInt(t);
    t = QObject::tr("Smoothing Type");
    smoothType = dialog->getComboIndex(t);
    
    t = QObject::tr("MA Color");
    maColor = dialog->getColor(t);
    t = QObject::tr("MA Line Type");
    maLineType = (PlotLine::LineType) dialog->getComboIndex(t);
    t = QObject::tr("MA Label");
    maLabel = dialog->getText(t);
    t = QObject::tr("MA Period");
    maPeriod = dialog->getInt(t);
    t = QObject::tr("MA Type");
    maType = dialog->getComboIndex(t);
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void THERM::setIndicatorSettings (Setting &dict)
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
    smoothType = s.toInt();
  
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
    maType = s.toInt();
}

void THERM::getIndicatorSettings (Setting &dict)
{
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
}

PlotLine * THERM::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  return output->getLine(0);
}

int THERM::getMinBars ()
{
  int t = minBars + maPeriod + smoothing;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  THERM *o = new THERM;
  return ((IndicatorPlugin *) o);
}

/* Alert Notes

1) enter when therm falls below MA
2) exit when threshold is triggered
3) explosive move expected when therm stays below MA for 5-7 days

*/


