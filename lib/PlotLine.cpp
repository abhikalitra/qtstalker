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
#include "PlotStyleFactory.h"

#include <QDebug>

PlotLine::PlotLine ()
{
  _plotFlag = FALSE;
  _plotStyle = 0;
}

PlotLine::~PlotLine ()
{
  if (_plotStyle)
    delete _plotStyle;

  if (_data.count())
    qDeleteAll(_data);
}

void PlotLine::draw (PlotData &pd, Scaler &scaler)
{
  if (! _plotStyle)
    return;

  _plotStyle->draw(pd, scaler, _data);
}

void PlotLine::info (int index, Setting *info)
{
  if (! _plotStyle)
    return;

  _plotStyle->info(index, info, _data, _label);
}

void PlotLine::setType (QString &type)
{
  _type = type;

  if (_plotStyle)
    delete _plotStyle;

  PlotStyleFactory fac;
  _plotStyle = fac.plotStyle(_type);
  if (! _plotStyle)
    qDebug() << "PlotLine::setType: no style found" << _type;
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

void PlotLine::setData (int i, double d)
{
  PlotLineBar *bar = _data.value(i);
  if (bar)
  {
    bar->setData(d);
    return;
  }

  bar = new PlotLineBar(d);
  _data.insert(i, bar);
}

void PlotLine::setData (int i, double d, QColor &color)
{
  PlotLineBar *bar = _data.value(i);
  if (bar)
  {
    bar->setData(d);
    bar->setColor(color);
    return;
  }

  bar = new PlotLineBar(color, d);
  _data.insert(i, bar);
}

void PlotLine::setData (int i, PlotLineBar *b)
{
  PlotLineBar *bar = _data.value(i);
  if (bar)
    delete bar;

  _data.insert(i, b);
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
  if (! it.hasNext())
  {
    start = 0;
    end = 0;
    return;
  }
  
  it.next();

  start = it.key();

  it.toBack();
  it.previous();
  end = it.key();
}

void PlotLine::setColor (QString &d)
{
  QColor color(d);
  
  QMapIterator<int, PlotLineBar *> it(_data);
  while (it.hasNext())
  {
    it.next();
    it.value()->setColor(color);
  }
}


