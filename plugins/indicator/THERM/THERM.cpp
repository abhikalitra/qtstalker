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
  alertFlag = FALSE;
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

int THERM::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("THERM Indicator"));

  dialog->createPage (tr("THERM Parms"));
  dialog->addColorItem(tr("Color Above MA"), 1, upColor);
  dialog->addColorItem(tr("Color Below MA"), 1, downColor);
  dialog->addColorItem(tr("Color Threshold"), 1, threshColor);
  dialog->addTextItem(tr("Label"), 1, label);
  dialog->addFloatItem(tr("Threshold"), 1, threshold, 1, 99999999);
  dialog->addIntItem(tr("Smoothing"), 1, smoothing, 0, 99999999);
  dialog->addComboItem(tr("Smoothing Type"), 1, getMATypes(), smoothType);
  
  dialog->createPage (tr("MA Parms"));
  dialog->addColorItem(tr("MA Color"), 2, maColor);
  dialog->addComboItem(tr("MA Line Type"), 2, lineTypes, maLineType);
  dialog->addTextItem(tr("MA Label"), 2, maLabel);
  dialog->addIntItem(tr("MA Period"), 2, maPeriod, 0, 99999999);
  dialog->addComboItem(tr("MA Type"), 2, getMATypes(), maType);
  
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
  setDefaults();
  
  QDict<QString> dict = loadFile(file);
  if (! dict.count())
    return;
  
  QString *s = dict["upColor"];
  if (s)
    upColor.setNamedColor(s->left(s->length()));
    
  s = dict["downColor"];
  if (s)
    downColor.setNamedColor(s->left(s->length()));
  
  s = dict["threshColor"];
  if (s)
    threshColor.setNamedColor(s->left(s->length()));
  
  s = dict["maColor"];
  if (s)
    maColor.setNamedColor(s->left(s->length()));
    
  s = dict["label"];
  if (s)
    label = s->left(s->length());
    
  s = dict["threshold"];
  if (s)
    threshold = s->left(s->length()).toFloat();
  
  s = dict["smoothing"];
  if (s)
    smoothing = s->left(s->length()).toInt();
  
  s = dict["smoothType"];
  if (s)
    smoothType = (IndicatorPlugin::MAType) s->left(s->length()).toInt();
  
  s = dict["maLineType"];
  if (s)
    maLineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["maLabel"];
  if (s)
    maLabel = s->left(s->length());
    
  s = dict["maPeriod"];
  if (s)
    maPeriod = s->left(s->length()).toInt();

  s = dict["maType"];
  if (s)
    maType = (IndicatorPlugin::MAType) s->left(s->length()).toInt();
}

void THERM::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);

  dict.replace("upColor", new QString(upColor.name()));
  dict.replace("downColor", new QString(downColor.name()));
  dict.replace("threshColor", new QString(threshColor.name()));
  dict.replace("label", new QString(label));
  dict.replace("threshold", new QString(QString::number(threshold)));
  dict.replace("smoothing", new QString(QString::number(smoothing)));
  dict.replace("smoothType", new QString(QString::number(smoothType)));
  
  dict.replace("maColor", new QString(maColor.name()));
  dict.replace("maLineType", new QString(QString::number(maLineType)));
  dict.replace("maPeriod", new QString(QString::number(maPeriod)));
  dict.replace("maLabel", new QString(maLabel));
  dict.replace("maType", new QString(QString::number(maType)));
  dict.replace("plugin", new QString(pluginName));
  
  saveFile(file, dict);
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


