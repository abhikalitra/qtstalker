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

#include "BB.h"
#include "PrefDialog.h"
#include <qdict.h>
#include <qobject.h>
#include <math.h>

BB::BB ()
{
  pluginName = "BB";
  
  bandList.append(QObject::tr("Upper"));
  bandList.append(QObject::tr("Lower"));
  
  helpFile = "bb.html";
  
  setDefaults();
}

BB::~BB ()
{
}

void BB::setDefaults ()
{
  color.setNamedColor("red");
  lineType = PlotLine::Line;
  deviation = 2;
  period = 20;
  maType = IndicatorPlugin::SMA;
  customBand = QObject::tr("Upper");
  label = pluginName;
}

void BB::calculate ()
{
  PlotLine *in = new PlotLine();
  int loop;
  for (loop = 0; loop < (int) data->count(); loop++)
    in->append((data->getHigh(loop) + data->getLow(loop) + data->getClose(loop)) / 3);
    
  PlotLine *sma = getMA(in, maType, period);
  sma->setColor(color);
  sma->setType(lineType);
  sma->setLabel(QObject::tr("BBM"));
  int smaLoop = sma->getSize() - 1;

  if ((int) sma->getSize() < period * 2)
  {
    delete in;
    delete sma;
    return;
  }
  
  PlotLine *bbu = new PlotLine;
  bbu->setColor(color);
  bbu->setType(lineType);
  bbu->setLabel(QObject::tr("BBU"));
  
  PlotLine *bbl = new PlotLine;
  bbl->setColor(color);
  bbl->setType(lineType);
  bbl->setLabel(QObject::tr("BBL"));

  int inputLoop = in->getSize() - 1;
  while (inputLoop >= period && smaLoop >= period)
  {
    int count;
    double t2 = 0;
    for (count = 0, t2 = 0; count < period; count++)
    {
      double t = in->getData(inputLoop - count) - sma->getData(smaLoop);
      t2 = t2 + (t * t);
    }

    double t = sqrt(t2 / period);

    bbu->prepend(sma->getData(smaLoop) + (deviation * t)); // upper band
    bbl->prepend(sma->getData(smaLoop) - (deviation * t)); // lower band

    inputLoop--;
    smaLoop--;
  }

  delete in;
  delete sma;

  output->addLine(bbu);
  output->addLine(bbl);
}

int BB::indicatorPrefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("BB Indicator"));
  dialog->setHelpFile(helpFile);
  dialog->createPage (QObject::tr("Parms"));
  dialog->addColorItem(QObject::tr("Color"), QObject::tr("Parms"), color);
  dialog->addComboItem(QObject::tr("Line Type"), QObject::tr("Parms"), lineTypes, lineType);
  dialog->addIntItem(QObject::tr("Period"), QObject::tr("Parms"), period, 1, 99999999);
  dialog->addFloatItem(QObject::tr("Deviation"), QObject::tr("Parms"), deviation, 0, 99999999);
  dialog->addComboItem(QObject::tr("MA Type"), QObject::tr("Parms"), maTypeList, maType);
  dialog->addTextItem(QObject::tr("Label"), QObject::tr("Parms"), label);
  if (customFlag)
    dialog->addComboItem(QObject::tr("Plot"), QObject::tr("Parms"), bandList, customBand);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(QObject::tr("Color"));
    lineType = (PlotLine::LineType) dialog->getComboIndex(QObject::tr("Line Type"));
    period = dialog->getInt(QObject::tr("Period"));
    maType = (IndicatorPlugin::MAType) dialog->getComboIndex(QObject::tr("MA Type"));
    deviation = dialog->getFloat(QObject::tr("Deviation"));
    label = dialog->getText(QObject::tr("Label"));
    if (customFlag)
      customBand = dialog->getCombo(QObject::tr("Plot"));
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void BB::loadIndicatorSettings (QString file)
{
  setIndicatorSettings(loadFile(file));
}

void BB::saveIndicatorSettings (QString file)
{
  saveFile(file, getIndicatorSettings());
}

void BB::setIndicatorSettings (Setting dict)
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

  s = dict.getData("deviation");
  if (s.length())
    deviation = s.toFloat();
  
  s = dict.getData("maType");
  if (s.length())
    maType = (IndicatorPlugin::MAType) s.toInt();

  s = dict.getData("customBand");
  if (s.length())
    customBand = s;

  s = dict.getData("label");
  if (s.length())
    label = s;
}

Setting BB::getIndicatorSettings ()
{
  Setting dict;
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("period", QString::number(period));
  dict.setData("deviation", QString::number(deviation));
  dict.setData("maType", QString::number(maType));
  dict.setData("customBand", customBand);
  dict.setData("label", label);
  dict.setData("plugin", pluginName);
  return dict;
}

PlotLine * BB::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  if (! customBand.compare(QObject::tr("Upper")))
    return output->getLine(0);
  else
    return output->getLine(1);
}

IndicatorPlugin * createIndicatorPlugin ()
{
  BB *o = new BB;
  return ((IndicatorPlugin *) o);
}


