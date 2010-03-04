/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "Scaler.h"
#include "PlotLine.h"
#include "ChartObject.h"
#include "PluginFactory.h"
#include "COPlugin.h"

#include <cmath>
#include <QList>
#include <QDateTime>
#include <QString>
#include <QHash>
#include <QHashIterator>

Scaler::Scaler ()
{
  height = 0;
  logScale = 0;
  scaleHigh = 0;
  scaleLow = 0;
  logScaleHigh = 0;
  logRange = 0;
  range = 0;
  scaler = 0;

  scaleList.append(".00001");
  scaleList.append(".00002");
  scaleList.append(".00005");
  scaleList.append(".0001");
  scaleList.append(".0002");
  scaleList.append(".0005");
  scaleList.append(".001");
  scaleList.append(".002");
  scaleList.append(".005");
  scaleList.append(".01");
  scaleList.append(".02");
  scaleList.append(".05");
  scaleList.append(".1");
  scaleList.append(".2");
  scaleList.append(".5");
  scaleList.append("1");
  scaleList.append("2");
  scaleList.append("5");
  scaleList.append("10");
  scaleList.append("25");
  scaleList.append("50");
  scaleList.append("100");
  scaleList.append("250");
  scaleList.append("500");
  scaleList.append("1000");
  scaleList.append("2500");
  scaleList.append("5000");
  scaleList.append("10000");
  scaleList.append("25000");
  scaleList.append("50000");
  scaleList.append("100000");
  scaleList.append("250000");
  scaleList.append("500000");
  scaleList.append("1000000");
  scaleList.append("2500000");
  scaleList.append("5000000");
  scaleList.append("10000000");
  scaleList.append("25000000");
  scaleList.append("50000000");
  scaleList.append("100000000");
  scaleList.append("250000000");
  scaleList.append("500000000");
  scaleList.append("1000000000");
  scaleList.append("2500000000");
  scaleList.append("5000000000");
  scaleList.append("10000000000");
  scaleList.append("25000000000");
  scaleList.append("50000000000");
  scaleList.append("100000000000");
  scaleList.append("250000000000");
  scaleList.append("500000000000");
}

void Scaler::set (int ht, double h, double l, double lh, double lr, bool lf)
{
  height = 0;
  logScale = 0;
  scaleHigh = 0;
  scaleLow = 0;
  logScaleHigh = 0;
  logRange = 0;
  range = 0;
  scaler = 0;

  if (h - l == 0)
    return;
    
  height = ht;
  logScale = lf;
  scaleHigh = h;
  scaleLow = l;
  logScaleHigh = lh;
  logRange = lr;

  range = scaleHigh - scaleLow;
  scaler = height / range;
}

int Scaler::convertToY (double val)
{
  if (logScale)
  {
    if (val <= 0.0)
      return height;
    else
      return (int) (height * (logScaleHigh - log(val)) / logRange);
  }

  double t = val - scaleLow;
  int y = (int) (t * scaler);
  y = height - y;
  if (y > height)
    y = height;
  return y;
}

double Scaler::convertToVal (int y)
{
  if (logScale)
  {
    if (y >= height)
      return scaleLow;
    else
      return exp(logScaleHigh - ((y * logRange) / height));
  }

  if (height == 0)
    return 0;
    
  int p = height - y;
  double val = scaleLow + (p / scaler) ;
  return val;
}

void Scaler::getScaleArray (QVector<double> &scaleArray)
{
  int ticks;
  for (ticks = 2; (ticks * 15) < height; ticks++)
    ;
  ticks--;
  if (ticks > 10)
    ticks = 10;
    
  double interval = 0;
  int loop;
  for (loop = 0; loop < (int) scaleList.count(); loop++)
  {
    interval = scaleList[loop].toDouble();
    if ((range / interval) < ticks)
      break;
  }

  scaleArray.resize(20);

  loop = 0;
  double t = 0 - (ticks * interval);
  
/*  
  double t = scaleLow;
  if (t < 0)
    t = 0 - (ticks * interval);
  else
    t = t - interval;
*/

  if (interval > 0)
  {
    while (t <= scaleHigh)
    {
      t = t + interval;

      if (t >= scaleLow)
      {
        scaleArray[loop] = t;
        loop++;
      }
    }
  }

  scaleArray.resize(loop);
}

double Scaler::getLogScaleHigh ()
{
  return logScaleHigh;
}

double Scaler::getLogRange ()
{
  return logRange;
}

int Scaler::getHeight ()
{
  return height;
}

bool Scaler::getLogFlag ()
{
  return logScale;
}

double Scaler::getLow ()
{
  return scaleLow;
}

void Scaler::setScale (Indicator &indicator, DateBar &dateBars, int scaleToScreen, int startIndex,
		       int width, int theight, QString &coPluginPath)
{
  double tscaleHigh = -99999999;
  double tscaleLow = 99999999;

  QList<PlotLine *> plotList;
  indicator.getLines(plotList);
  
  int loop;
  for (loop = 0; loop < plotList.count(); loop++)
  {
    PlotLine *line = plotList.at(loop);

    if (line->getScaleFlag())
      continue;

    if (! scaleToScreen)
    {
      if (line->getHigh() > tscaleHigh)
        tscaleHigh = line->getHigh();

      if (line->getLow() < tscaleLow)
        tscaleLow = line->getLow();
    }
    else
    {
      int start = line->count() - dateBars.count() + startIndex;
      int end = width + start;
      if (start < 0)
        start = 0;
      if (end > line->count())
        end = line->count();

      double h, l;
      line->getHighLowRange(start, end - 1, h, l);
      if (h > tscaleHigh)
        tscaleHigh = h;
      if (l < tscaleLow)
        tscaleLow = l;
    }
  }

  // do this anyway for scaleToScreen even if we dont use it
  int start = startIndex;
  int end = width + startIndex;
  if (start < 0)
    start = 0;
  if (end > dateBars.count())
    end = dateBars.count();
  QDateTime sd;
  dateBars.getDate(start, sd);
  QDateTime ed;
  dateBars.getDate(end - 1, ed);

  QHash<QString, ChartObject *> coList;
  indicator.getChartObjects(coList);
  PluginFactory fac;
  QHashIterator<QString, ChartObject *> it(coList);
  while (it.hasNext())
  {
    it.next();
    ChartObject *co = it.value();
      
    QString s;
    co->getData(ChartObject::ParmPlugin, s);
    COPlugin *plug = fac.getCO(coPluginPath, s);
    
    if (scaleToScreen)
    {
      if (! plug->inDateRange(co, sd, ed))
        continue;
    }
      
    if (plug->getHighLow(co))
      continue;
    
    double h = co->getDouble(ChartObject::ParmHigh);
    double l = co->getDouble(ChartObject::ParmLow);
      
    if (h > tscaleHigh)
      tscaleHigh = h;

    if (l < tscaleLow)
      tscaleLow = l;
  }

  // create a little more room between chart edges and plots
  double t = (tscaleHigh - tscaleLow) * 0.02; // get 2% of the range
  tscaleHigh = tscaleHigh + t;
  tscaleLow = tscaleLow - t;

  // handle log scaling if toggled
  double tlogScaleHigh = 1;
  double tlogRange = 0;
  if (indicator.getLog())
  {
    tlogScaleHigh = tscaleHigh > 0.0 ? log(tscaleHigh) : 1;
    double tlogScaleLow = tscaleLow > 0.0 ? log(tscaleLow) : 0;
    tlogRange = tlogScaleHigh - tlogScaleLow;
  }

  set(theight, tscaleHigh, tscaleLow, tlogScaleHigh, tlogRange, indicator.getLog());
}



