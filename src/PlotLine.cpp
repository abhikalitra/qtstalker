/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

PlotLine::PlotLine ()
{
  data.setAutoDelete(TRUE);
  color = "red";
  lineType = "Line";
  high = -99999999;
  low = 99999999;
  colorBars = FALSE;
}

PlotLine::~PlotLine ()
{
}

void PlotLine::setColor (QString d)
{
  color = d;
}

QString PlotLine::getColor ()
{
  return color;
}

void PlotLine::setType (QString d)
{
  lineType = d;
}

QString PlotLine::getType ()
{
  return lineType;
}

void PlotLine::setLabel (QString d)
{
  label = d;
}

QString PlotLine::getLabel ()
{
  return label;
}

void PlotLine::append (double d)
{
  Val *r = new Val;
  r->v  = d;
  data.append(r);
  checkHighLow(d);
}

void PlotLine::prepend (double d)
{
  Val *r = new Val;
  r->v = d;
  data.prepend(r);
  checkHighLow(d);
}

double PlotLine::getData (int d)
{
  Val *r = data.at(d);
  return r->v;
}

void PlotLine::setData (int i, double d)
{
  Val *r = data.at(i);
  r->v = d;
  checkHighLow(d);
}

int PlotLine::getSize ()
{
  return (int) data.count();
}

double PlotLine::getHigh ()
{
  return high;
}

double PlotLine::getLow ()
{
  return low;
}

void PlotLine::checkHighLow (double d)
{
  if (d > high)
    high = d;
  if (d < low)
    low = d;
}

void PlotLine::setColorBars (bool d)
{
  colorBars = d;
}

bool PlotLine::getColorBars ()
{
  return colorBars;
}




