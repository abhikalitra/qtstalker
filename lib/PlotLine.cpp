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

#include "PlotLine.h"

PlotLine::PlotLine ()
{
  data.setAutoDelete(TRUE);
  colorBars.setAutoDelete(TRUE);
  color = "red";
  lineType = "Line";
  high = -99999999;
  low = 99999999;
  colorBarFlag = FALSE;
  scaleFlag = FALSE;
}

PlotLine::~PlotLine ()
{
}

void PlotLine::copy (PlotLine *d)
{
  setColor(d->getColor());
  setType(d->getType());
  setLabel(d->getLabel());
  setColorFlag(d->getColorFlag());
  setScaleFlag(d->getScaleFlag());

  int loop;
  for (loop = 0; loop < (int) d->getSize(); loop++)
  {
    append(d->getData(loop));

    if (d->getColorFlag() == TRUE)
      appendColorBar(d->getColorBar(loop));
  }
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

void PlotLine::setColorFlag (bool d)
{
  colorBarFlag = d;
}

bool PlotLine::getColorFlag ()
{
  return colorBarFlag;
}

void PlotLine::appendColorBar (QString d)
{
  QString *r = new QString(d);
  colorBars.append(r);
}

void PlotLine::prependColorBar (QString d)
{
  QString *r = new QString(d);
  colorBars.prepend(r);
}

QString PlotLine::getColorBar (int d)
{
  if (d >= (int) colorBars.count())
    return QString::null;

  QString *r = colorBars.at(d);
  return r->left(r->length());
}


