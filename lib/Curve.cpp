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
#include "DataInteger.h"
#include "DataString.h"
#include "DataColor.h"

#include <QDebug>

Curve::Curve ()
{
  _settings = new CurveData;
  init();
}

Curve::~Curve ()
{
  delete _settings;
}

void Curve::init ()
{
  _high = 0;
  _low = 0;
  setZ(-1);

  delete _settings;
  _settings = new CurveData;

  _settings->set(CurveData::_PEN, new DataInteger(1));
  _settings->set(CurveData::_Z, new DataInteger(-1));
  _settings->set(CurveData::_LABEL, new DataString(QString()));

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
    CurveBar *r = (CurveBar *) _settings->toData(loop);
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
  Data *b = _settings->toData(index);
  if (! b)
    return 1;

  Strip strip;
  QString s;
  strip.strip(b->toData(CurveBar::_VALUE)->toDouble(), 4, s);

  data->insert(_settings->toData(CurveData::_LABEL)->toString(), s);

  return 0;
}

QList<int> Curve::keys ()
{
  return _settings->keys();
}

void Curve::keyRange (int &startIndex, int &endIndex)
{
  startIndex = 0;
  endIndex = 0;
  _settings->keyRange(startIndex, endIndex);
}

int Curve::setAllColor (QColor color)
{
  QList<int> keys = _settings->keys();
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *set = _settings->toData(keys.at(loop));
    set->set(CurveBar::_COLOR, new DataColor(color));
  }

  return 0;
}

void Curve::setSettings (Data *d)
{
  delete _settings;
  _settings = d;

  setZ(_settings->toData(CurveData::_Z)->toInteger());
}

Data * Curve::settings ()
{
  return _settings;
}

int Curve::scalePoint (int i, QColor &color, double &v)
{
  Data *bar = _settings->toData(i);
  if (! bar)
    return 1;

  color = bar->toData(CurveBar::_COLOR)->toColor();
  v = bar->toData(CurveBar::_VALUE)->toDouble();

  return 0;
}
