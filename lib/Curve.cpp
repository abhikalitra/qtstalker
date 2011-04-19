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
  qDeleteAll(_data);
}

void Curve::init ()
{
  _z = -1;
  _pen = 1;
  qDeleteAll(_data);
  _data.clear();
  _label.clear();
  _type.clear();
}

void Curve::setBar (int index, CurveBar *bar)
{
  if (_data.contains(index))
  {
    CurveBar *b = _data.value(index);
    delete b;
  }

  _data.insert(index, bar);
}

CurveBar * Curve::bar (int index)
{
  if (_data.contains(index))
    return _data.value(index);
  else
    return 0;
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
  qSort(l);
}

void Curve::keyRange (int &startIndex, int &endIndex)
{
  startIndex = 0;
  endIndex = 0;

  if (! _data.count())
    return;
  
  QList<int> keys;
  keys = _data.keys();
  qSort(keys);
  startIndex = keys.at(0);
  endIndex = keys.at(keys.count() - 1);
}

int Curve::setAllColor (QColor color)
{
  if (! color.isValid())
    return 1;

  QHashIterator<int, CurveBar *> it(_data);
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

QStringList Curve::list ()
{
  QSettings set(g_globalSettings);
  return set.value("curve_plugins").toStringList();
}

void Curve::deleteBar (int d)
{
  if (_data.contains(d))
  {
    CurveBar *bar = _data.value(d);
    _data.remove(d);
    delete bar;
  }
}

void Curve::copy (Curve *d)
{
  d->init();

  QHashIterator<int, CurveBar *> it(_data);
  while (it.hasNext())
  {
    it.next();
    CurveBar *b = new CurveBar;
    it.value()->copy(b);
    d->setBar(it.key(), b);
  }

  d->setLabel(_label);
  d->setType(_type);
  d->setZ(_z);
}

int Curve::highLow (double &h, double &l)
{
  int rc = 1;
  int flag = 0;
  h = 0;
  l = 0;
  QHashIterator<int, CurveBar *> it(_data);
  while (it.hasNext())
  {
    it.next();
    rc = 0;

    if (! flag)
    {
      it.value()->highLow(h, l);
      flag++;
    }
    else
    {
      double th = 0;
      double tl = 0;
      it.value()->highLow(th, tl);
      if (th > h)
	h = th;
      if (tl < l)
	l = tl;
    }
  }

  return rc;
}

void Curve::setPen (int d)
{
  _pen = d;
}

int Curve::pen ()
{
  return _pen;
}
