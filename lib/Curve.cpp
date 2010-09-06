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

#include "Curve.h"
#include "Strip.h"

#include <QDebug>

Curve::Curve ()
{
  init();
}

Curve::Curve (Curve::Type type)
{
  init();
  _type = type;
}

Curve::~Curve ()
{
  if (_data.count())
    qDeleteAll(_data);
}

void Curve::init ()
{
  _list << "Candle" << "Dash" << "Dot" << "Histogram" << "Histogram Bar" << "Horizontal" << "Line" << "OHLC";
  _type = Line;
  _z = -1;
  _color = QColor(Qt::red);
}

void Curve::setBar (int index, CurveBar *bar)
{
  CurveBar *b = _data.value(index);
  if (b)
    delete b;

  _data.insert(index, bar);
}

CurveBar * Curve::bar (int index)
{
  return _data.value(index);
}

void Curve::setType (Curve::Type type)
{
  _type = type;
}

Curve::Type & Curve::type ()
{
  return _type;
}

void Curve::setLabel (QString &text)
{
  _label = text;
}

QString & Curve::label ()
{
  return _label;
}

int Curve::count ()
{
  return (int) _data.count();
}

void Curve::keys (QList<int> &l)
{
  l = _data.keys();
}

void Curve::keyRange (int &startIndex, int &endIndex)
{
  QMapIterator<int, CurveBar *> it(_data);
  it.toFront();
  if (! it.hasNext())
  {
    startIndex = 0;
    endIndex = 0;
    return;
  }
  
  it.next();

  startIndex = it.key();

  it.toBack();
  it.previous();
  endIndex = it.key();
}

int Curve::setAllColor (QColor &color)
{
  if (! color.isValid())
    return 1;
  
  QMapIterator<int, CurveBar *> it(_data);
  while (it.hasNext())
  {
    it.next();
    it.value()->setColor(color);
  }

  return 0;
}

void Curve::setZ (int z)
{
  _z = z;
}

int Curve::z ()
{
  return _z;
}

void Curve::setColor (QColor &c)
{
  _color = c;
}

QColor & Curve::color ()
{
  return _color;
}

void Curve::list (QStringList &l, int flag)
{
  l = _list;

  if (flag)
  {
    l.removeAll("Candle");
    l.removeAll("OHLC");
  }
}

int Curve::typeFromString (QString &d)
{
  return _list.indexOf(d);
}

void Curve::info (int index, Setting &set)
{
  Strip strip;
  
  switch (_type)
  {
    case Candle:
    case OHLC:
    {
      CurveBar *b = bar(index);
      if (! b)
        return;

      QString d;
      strip.strip(b->data(0), 4, d);
      set.setData("O", d);
      strip.strip(b->data(1), 4, d);
      set.setData("H", d);
      strip.strip(b->data(2), 4, d);
      set.setData("L", d);
      strip.strip(b->data(3), 4, d);
      set.setData("C", d);
      break;
    }
    case Dot:
    case Histogram:
    case HistogramBar:
    case Line:
    {
      CurveBar *b = bar(index);
      if (! b)
        return;

      QString d;
      strip.strip(b->data(), 4, d);
      set.setData(label(), d);
      break;
    }
    default:
      break;
  }
}

int Curve::highLowRange (int start, int end, double &h, double &l)
{
  int rc = 1;
  int loop;
  h = -99999999;
  l = 99999999;

  for (loop = start; loop <= end; loop++)
  {
    CurveBar *r = _data.value(loop);
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


