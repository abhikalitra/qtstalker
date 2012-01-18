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
#include "CurveData.h"
#include "CurveBar.h"
#include "CurveBarKey.h"
#include "CurveDataKey.h"

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
    if (_settings.toEntity(QString::number(loop), r))
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
  if (_settings.toEntity(QString::number(index), b))
    return 1;
  
  CurveBarKey cbkeys;
  Data value;
  if (b.toData(cbkeys.indexToString(CurveBarKey::_VALUE), value))
    return 1;

  CurveDataKey cdkeys;
  Data label;
  if (_settings.toData(cdkeys.indexToString(CurveDataKey::_LABEL), label))
    return 1;

  Strip strip;
  QString s;
  strip.strip(value.toDouble(), 4, s);
  data.set(label.toString(), s);

  return 0;
}

QList<QString> Curve::keys ()
{
  return _settings.ekeys();
}

void Curve::keyRange (int &startIndex, int &endIndex)
{
  startIndex = 0;
  endIndex = 0;
  _settings.ekeyRange(startIndex, endIndex);
}

int Curve::setAllColor (QColor color)
{
  CurveBarKey cbkeys;
  QList<QString> keys = _settings.ekeys();
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Entity bar;
    if (_settings.toEntity(keys.at(loop), bar))
      continue;
    
    bar.set(cbkeys.indexToString(CurveBarKey::_COLOR), Data(color));
    _settings.setEntity(keys.at(loop), bar);
  }

  return 0;
}

void Curve::setSettings (Entity &d)
{
  _settings = d;

  CurveDataKey keys;
  Data tdi;
  _settings.toData(keys.indexToString(CurveDataKey::_Z), tdi);
  setZ(tdi.toInteger());
}

Entity & Curve::settings ()
{
  return _settings;
}

int Curve::scalePoint (int i, QColor &color, double &v)
{
  Entity bar;
  if (_settings.toEntity(QString::number(i), bar))
    return 1;

  CurveBarKey keys;
  Data td;
  if (bar.toData(keys.indexToString(CurveBarKey::_COLOR), td))
    return 1;
  color = td.toColor();
  
  if (bar.toData(keys.indexToString(CurveBarKey::_VALUE), td))
    return 1;
  v = td.toDouble();

  return 0;
}
