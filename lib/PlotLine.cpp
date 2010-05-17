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

#include "PlotLine.h"

PlotLine::PlotLine ()
{
  _high = -99999999;
  _low = 99999999;
  _plotFlag = FALSE;
  _scaleFlag = FALSE;
}

PlotLine::~PlotLine ()
{
  if (count())
    qDeleteAll(_data);
}

// virtual
void PlotLine::draw (PlotData &, Scaler &)
{
}

// virtual
void PlotLine::info (int, Setting *)
{
}

QString & PlotLine::type ()
{
  return _type;
}

void PlotLine::setLabel (QString &d)
{
  _label = d;
}

QString & PlotLine::label ()
{
  return _label;
}

void PlotLine::setData (int i, PlotLineBar *d)
{
  PlotLineBar *bar = _data.value(i);
  if (bar)
    delete bar;

  _data.insert(i, d);

  double h, l;
  d->highLow(h, l);

  checkHighLow(h);
  checkHighLow(l);
}

PlotLineBar * PlotLine::data (int i)
{
  return _data.value(i);
}

int PlotLine::count ()
{
  return (int) _data.count();
}

double PlotLine::high ()
{
  return _high;
}

double PlotLine::low ()
{
  return _low;
}

void PlotLine::checkHighLow (double d)
{
  if (d > _high)
    _high = d;

  if (d < _low)
    _low = d;
}

void PlotLine::highLowRange (int start, int end, double &h, double &l)
{
  int loop;
  h = -99999999;
  l = 99999999;

  for (loop = start; loop <= end; loop++)
  {
    PlotLineBar *r = _data.value(loop);
    if (! r)
      continue;

    double th, tl;
    r->highLow(th, tl);

    if (th > h)
      h = th;

    if (tl < l)
      l = tl;
  }
}

void PlotLine::setPlotFlag (bool d)
{
  _plotFlag = d;
}

bool PlotLine::plotFlag ()
{
  return _plotFlag;
}

void PlotLine::setScaleFlag (bool d)
{
  _scaleFlag = d;
}

bool PlotLine::scaleFlag ()
{
  return _scaleFlag;
}

void PlotLine::keys (QList<int> &l)
{
  l = _data.keys();
}

