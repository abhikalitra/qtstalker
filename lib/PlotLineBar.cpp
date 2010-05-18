/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#include "PlotLineBar.h"

PlotLineBar::PlotLineBar ()
{
  _color.setNamedColor("red");
}

PlotLineBar::PlotLineBar (double d)
{
  _color.setNamedColor("red");
  setData(0, d);
}

PlotLineBar::PlotLineBar (QColor &c, double d)
{
  _color = c;
  setData(0, d);
}

double PlotLineBar::data (int i)
{
  return _data.value(i);
}

double PlotLineBar::data ()
{
  if (count() == 4)
    return _data.value(3);
  else
    return _data.value(0);
}

void PlotLineBar::setData (int i, double d)
{
  _data.insert(i, d);
}

void PlotLineBar::setData (double d)
{
  if (count() == 4)
    setData(3, d);
  else
    setData(0, d);
}

void PlotLineBar::setColor (QColor &d)
{
  _color = d;
}

QColor & PlotLineBar::color ()
{
  return _color;
}

int PlotLineBar::count ()
{
  return (int) _data.count();
}

int PlotLineBar::highLow (double &h, double &l)
{
  int rc = 1;
  h = -99999999;
  l = 99999999;
  QHashIterator<int, double> it(_data);

  while (it.hasNext())
  {
    it.next();
    rc = 0;

    if (it.value() > h)
      h = it.value();

    if (it.value() < l)
      l = it.value();
  }

  return rc;
}



