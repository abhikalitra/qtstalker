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
#include "Globals.h"

#include <QDebug>
#include <QSettings>

Curve::Curve ()
{
  init();
}

Curve::~Curve ()
{
  if (_data.count())
    qDeleteAll(_data);
}

void Curve::init ()
{
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

void Curve::setType (QString d)
{
  _type = d;
}

QString Curve::type ()
{
  return _type;
}

void Curve::setLabel (QString text)
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

int Curve::setAllColor (QColor color)
{
  if (! color.isValid())
    return 1;

  setColor(color);
  
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

void Curve::setColor (QColor c)
{
  _color = c;
}

QColor & Curve::color ()
{
  return _color;
}

QStringList Curve::list ()
{
  QSettings set(g_globalSettings);
  return set.value("curve_plugins").toStringList();
}

void Curve::info (int index, Setting &set)
{
  CurveBar *b = bar(index);
  if (! b)
    return;

  Strip strip;

  if (b->count() == 4)
  {
    QString d;
    strip.strip(b->data(0), 4, d);
    set.setData("O", d);
      
    strip.strip(b->data(1), 4, d);
    set.setData("H", d);
      
    strip.strip(b->data(2), 4, d);
    set.setData("L", d);
      
    strip.strip(b->data(3), 4, d);
    set.setData("C", d);
  }
  else
  {
    QString d;
    strip.strip(b->data(), 4, d);
    set.setData(label(), d);
  }
}

int Curve::highLowRange (int start, int end, double &h, double &l)
{
  int rc = 1;
  int loop;
  h = -9999999999.0;
  l = 9999999999.0;

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

void Curve::deleteBar (int d)
{
  CurveBar *bar = _data.value(d);
  if (bar)
  {
    _data.remove(d);
    delete bar;
  }
}
