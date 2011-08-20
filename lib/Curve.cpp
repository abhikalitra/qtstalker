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

QwtDoubleRect Curve::boundingRect () const
{
  QwtDoubleRect rect(0, 0, 0, 0);
  rect.setBottom(_low);
  rect.setTop(_high);
  return rect;
}

int Curve::rtti () const
{
  return QwtPlotCurve::UserCurve;
}

int Curve::highLow (double &h, double &l)
{
  if (! _data.count())
    return 1;

  h = _high;
  l = _low;

  return 0;
}

int Curve::highLowRange (int start, int end, double &h, double &l)
{
  if (! _data.count())
    return 1;

  int rc = 1;
  int loop = start;
  int flag = 0;
  for (; loop < end; loop++)
  {
    CurveBar *r = _data.value(loop);
    if (! r)
      continue;

    double th, tl;
    if (r->highLow(th, tl))
      continue;

    rc = 0;

    if (! flag)
    {
      h = th;
      l = tl;
      flag++;
    }
    else
    {
      if (th > h)
        h = th;
      if (tl < l)
        l = tl;
    }
  }

  return rc;
}

int Curve::info (int index, Message *data)
{
  if (! _data.count())
    return 1;

  CurveBar *b = _data.value(index);
  if (! b)
    return 1;

  Strip strip;
  QString s;
  strip.strip(b->data(), 4, s);
  data->insert(_label, s);

  return 0;
}

void Curve::init ()
{
  _high = 0;
  _low = 0;
  setZ(-1);
  _penWidth = 1;
  _label.clear();
//  _type.clear();

  qDeleteAll(_data);
  _data.clear();

  setItemAttribute(QwtPlotItem::AutoScale, TRUE);
  setItemAttribute(QwtPlotItem::Legend, TRUE);
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

void Curve::deleteBar (int d)
{
  if (_data.contains(d))
  {
    CurveBar *bar = _data.value(d);
    _data.remove(d);
    delete bar;
  }
}

void Curve::setPenWidth (int d)
{
  _penWidth = d;
}

int Curve::penWidth ()
{
  return _penWidth;
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
  d->setZ(z());
  d->setPenWidth(_penWidth);
  d->setPlotName(_plotName);
}

void Curve::setPlotName (QString d)
{
  _plotName = d;
}

QString Curve::plotName ()
{
  return _plotName;
}

QString Curve::toString ()
{
  QStringList l;
  l << _type << _label << _plotName << QString::number(z()) << QString::number(_penWidth);

  QHashIterator<int, CurveBar *> it(_data);
  while (it.hasNext())
  {
    it.next();
    l << QString::number(it.key()) << it.value()->toString();
  }

  return l.join(";");
}

int Curve::fromString (QString d)
{
  QStringList l = d.split(";");
  if (l.count() < 5)
    return 1;

  int loop = 0;
  _type = l.at(loop++);
  _label = l.at(loop++);
  _plotName = l.at(loop++);
  setZ(l.at(loop++).toInt());
  _penWidth = l.at(loop++).toInt();

  for (; loop < l.count(); loop+= 2)
  {
    if (loop + 1 >= l.count())
      return 1;

    CurveBar *b = new CurveBar;
    b->fromString(l.at(loop + 1));
    setBar(l.at(loop).toInt(), b);
  }

  return 0;
}
