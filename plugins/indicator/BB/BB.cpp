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
  
  bandList.append("Upper");
  bandList.append("Lower");
  
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
  maType = 1;
  customBand = "Upper";
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
  QString s = "BBM";
  sma->setLabel(s);
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
  s = "BBU";
  bbu->setLabel(s);
  
  PlotLine *bbl = new PlotLine;
  bbl->setColor(color);
  bbl->setType(lineType);
  s = "BBL";
  bbl->setLabel(s);

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
  QString pl = QObject::tr("Parms");
  QString cl = QObject::tr("Color");
  QString ll = QObject::tr("Label");
  QString ltl = QObject::tr("Line Type");
  QString sl = QObject::tr("Period");
  QString stl = QObject::tr("MA Type");
  QString dl = QObject::tr("Deviation");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(QObject::tr("BB Indicator"));
  dialog->setHelpFile(helpFile);
  dialog->createPage (pl);
  dialog->addColorItem(cl, pl, color);
  dialog->addComboItem(ltl, pl, lineTypes, lineType);
  dialog->addIntItem(sl, pl, period, 1, 99999999);
  dialog->addFloatItem(dl, pl, deviation, 0, 99999999);
  QStringList l = getMATypes();
  dialog->addComboItem(stl, pl, l, maType);
  dialog->addTextItem(ll, pl, label);
  if (customFlag)
  {
    QString s = QObject::tr("Plot");
    dialog->addComboItem(s, pl, bandList, customBand);
  }
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(cl);
    lineType = (PlotLine::LineType) dialog->getComboIndex(ltl);
    period = dialog->getInt(sl);
    maType = dialog->getComboIndex(stl);
    deviation = dialog->getFloat(dl);
    label = dialog->getText(ll);
    if (customFlag)
    {
      QString s = QObject::tr("Plot");
      customBand = dialog->getCombo(s);
    }
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void BB::setIndicatorSettings (Setting &dict)
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
    maType = s.toInt();

  s = dict.getData("customBand");
  if (s.length())
    customBand = s;

  s = dict.getData("label");
  if (s.length())
    label = s;
}

void BB::getIndicatorSettings (Setting &dict)
{
  dict.setData("color", color.name());
  dict.setData("lineType", QString::number(lineType));
  dict.setData("period", QString::number(period));
  dict.setData("deviation", QString::number(deviation));
  dict.setData("maType", QString::number(maType));
  dict.setData("customBand", customBand);
  dict.setData("label", label);
  dict.setData("plugin", pluginName);
}

PlotLine * BB::calculateCustom (QDict<PlotLine> *)
{
  clearOutput();
  calculate();
  if (! customBand.compare("Upper"))
    return output->getLine(0);
  else
    return output->getLine(1);
}

int BB::getMinBars ()
{
  int t = minBars + period;
  return t;
}

//*******************************************************
//*******************************************************
//*******************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  BB *o = new BB;
  return ((IndicatorPlugin *) o);
}


