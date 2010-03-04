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

#include "PlotLine.h"
#include "Utils.h"

PlotLine::PlotLine ()
{
  high = -99999999;
  low = 99999999;
  scaleFlag = FALSE;
  plotFlag = FALSE;
}

void PlotLine::setPlugin (QString &d)
{
  plugin = d;
}

void PlotLine::getPlugin (QString &d)
{
  d = plugin;
}

void PlotLine::setLabel (QString &d)
{
  label = d;
}

void PlotLine::getLabel (QString &d)
{
  d = label;
}

void PlotLine::append (double d)
{
  QColor color("red");
  append(color, d);
}

void PlotLine::append (QColor &c, double d)
{
  PlotLineBar bar;
  bar.append(d);
  bar.setColor(c);
  data.append(bar);
  checkHighLow(d);
}

void PlotLine::append (PlotLineBar &d)
{
  data.append(d);
}

void PlotLine::prepend (double d)
{
  QColor color("red");
  prepend(color, d);
}

void PlotLine::prepend (QColor &c, double d)
{
  PlotLineBar bar;
  bar.append(d);
  bar.setColor(c);
  data.prepend(bar);
  checkHighLow(d);
}

void PlotLine::setData (int i, double d)
{
  PlotLineBar bar = data.at(i);
  bar.append(d);
  data.replace(i, bar);
}

double PlotLine::getData (int i, QColor &c)
{
  PlotLineBar bar = data.at(i);
  bar.getColor(c);
  return bar.getData(0);
}

double PlotLine::getData (int i)
{
  PlotLineBar bar = data.at(i);
  return bar.getData(0);
}

void PlotLine::getData (int i, PlotLineBar &d)
{
  d = data.at(i);
}

int PlotLine::count ()
{
  return (int) data.count();
}

double PlotLine::getHigh ()
{
  return high;
}

void PlotLine::setHigh (double d)
{
  high = d;
}

double PlotLine::getLow ()
{
  return low;
}

void PlotLine::setLow (double d)
{
  low = d;
}

void PlotLine::checkHighLow (double d)
{
  if (d > high)
    high = d;
  if (d < low)
    low = d;
}

void PlotLine::setScaleFlag (bool d)
{
  scaleFlag = d;
}

bool PlotLine::getScaleFlag ()
{
  return scaleFlag;
}

void PlotLine::getHighLowRange (int start, int end, double &h, double &l)
{
  int loop;
  h = -99999999;
  l = 99999999;
  for (loop = start; loop <= end; loop++)
  {
    PlotLineBar r = data.at(loop);
    if (r.getData(0) > h)
      h = r.getData(0);
    if (r.getData(0) < l)
      l = r.getData(0);
  }
}

void PlotLine::setPlotFlag (bool d)
{
  plotFlag = d;
}

bool PlotLine::getPlotFlag ()
{
  return plotFlag;
}

void PlotLine::setOffset (int d)
{
  offset = d;
}

int PlotLine::getOffset ()
{
  return offset;
}

void PlotLine::getInfo (int i, Setting *set)
{
  Utils util;
  
  if (plugin == "OHLC" || plugin == "Candle")
  {
    PlotLineBar bar;
    getData(i, bar);
    
    QString k = "O";
    QString d;
    util.strip(bar.getData(0), 4, d);
    set->setData(k, d);
	
    k = "H";
    util.strip(bar.getData(1), 4, d);
    set->setData(k, d);
	
    k = "L";
    util.strip(bar.getData(2), 4, d);
    set->setData(k, d);
	
    k = "C";
    util.strip(bar.getData(3), 4, d);
    set->setData(k, d);
  }
  else
  {
    QString d;
    util.strip(getData(i), 4, d);
    set->setData(label, d);
  }
}

void PlotLine::setColor (QColor &color)
{
  int loop;
  for (loop = 0; loop < count(); loop++)
    setColorBar(loop, color);
}

void PlotLine::setColor (QString &d)
{
  QColor color(d);
  setColor(color);
}

void PlotLine::setColorBar (int i, QColor &color)
{
  PlotLineBar bar = data.at(i);
  bar.setColor(color);
  data.replace(i, bar);
}

