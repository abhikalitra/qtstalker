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
#include "CurveBar.h"
#include "KeyCurveBar.h"
#include "KeyCurveData.h"

#include <QDebug>

Curve::Curve ()
{
  init();
}

Curve::~Curve ()
{
}

void Curve::init ()
{
  _high = 0;
  _low = 0;
  setZ(-1);

  setItemAttribute(QwtPlotItem::AutoScale, TRUE);
  setItemAttribute(QwtPlotItem::Legend, TRUE);
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
  h = _high;
  l = _low;
  return 0;
}

int Curve::highLowRange (int start, int end, double &h, double &l)
{
  int rc = 1;
  int loop = start;
  int flag = 0;
  for (; loop < end; loop++)
  {
    CurveBar r;
    if (_settings.toIndex(loop, r))
      continue;

    double th, tl;
    if (r.highLow(th, tl))
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

int Curve::info (int index, Entity &data)
{
  Entity b;
  if (_settings.toIndex(index, b))
    return 1;
  
  KeyCurveBar cbkeys;
  Data value;
  if (b.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), value))
    return 1;

  KeyCurveData cdkeys;
  Data label;
  if (_settings.toData(cdkeys.indexToString(KeyCurveData::_LABEL), label))
    return 1;

  Strip strip;
  QString s;
  strip.strip(value.toDouble(), 4, s);
  data.set(label.toString(), s);

  return 0;
}

void Curve::keys (QList<int> &d)
{
  _settings.ekeys(d);
}

void Curve::keyRange (int &startIndex, int &endIndex)
{
  startIndex = 0;
  endIndex = 0;
  _settings.ekeyRange(startIndex, endIndex);
}

int Curve::setAllColor (QColor color)
{
  KeyCurveBar cbkeys;
  QList<int> keys;
  _settings.ekeys(keys);
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Entity bar;
    if (_settings.toIndex(keys.at(loop), bar))
      continue;
    
    bar.set(cbkeys.indexToString(KeyCurveBar::_COLOR), Data(color));
    _settings.setEntity(keys.at(loop), bar);
  }

  return 0;
}

void Curve::setSettings (Entity &d)
{
  _settings = d;

  KeyCurveData keys;
  Data tdi;
  _settings.toData(keys.indexToString(KeyCurveData::_Z), tdi);
  setZ(tdi.toInteger());
}

Entity & Curve::settings ()
{
  return _settings;
}

int Curve::scalePoint (int i, QColor &color, double &v)
{
  Entity bar;
  if (_settings.toIndex(i, bar))
    return 1;

  KeyCurveBar keys;
  Data td;
  if (bar.toData(keys.indexToString(KeyCurveBar::_COLOR), td))
    return 1;
  color = td.toColor();
  
  if (bar.toData(keys.indexToString(KeyCurveBar::_VALUE), td))
    return 1;
  v = td.toDouble();

  return 0;
}
