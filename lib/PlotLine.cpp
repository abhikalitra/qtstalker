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
  _plotFlag = FALSE;
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
}

PlotLineBar * PlotLine::data (int i)
{
  return _data.value(i);
}

int PlotLine::count ()
{
  return (int) _data.count();
}

int PlotLine::highLowRange (int start, int end, double &h, double &l)
{
  int rc = 1;
  int loop;
  h = -99999999;
  l = 99999999;

  for (loop = start; loop <= end; loop++)
  {
    PlotLineBar *r = _data.value(loop);
    if (! r)
      continue;

    double th, tl;
    if (r->highLow(th, tl))
      continue;

    rc = 0;
    
    if (th > h)
      h = th;

    if (tl < l)
      l = tl;
  }

  return rc;
}

void PlotLine::setPlotFlag (bool d)
{
  _plotFlag = d;
}

bool PlotLine::plotFlag ()
{
  return _plotFlag;
}

void PlotLine::keys (QList<int> &l)
{
  l = _data.keys();
}

void PlotLine::keyRange (int &start, int &end)
{
  QMapIterator<int, PlotLineBar *> it(_data);
  it.toFront();
  it.next();
  PlotLineBar *bar = it.value();
  if (! bar)
  {
    start = 0;
    end = 0;
    return;
  }

  start = it.key();

  it.toBack();
  it.previous();
  end = it.key();
}


