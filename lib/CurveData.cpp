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

#include "CurveData.h"
#include "CurveBar.h"
#include "DataFactory.h"
#include "DataInteger.h"

#include <QtDebug>

CurveData::CurveData ()
{
  clear();
}

CurveData::~CurveData ()
{
  clear();
}

void CurveData::clear ()
{
  _type = DataFactory::_CURVE;
  _startIndex = 99999999;
  _endIndex = -1;

  qDeleteAll(_data);
  _data.clear();

  qDeleteAll(_bars);
  _bars.clear();

  set(_Z, new DataInteger(-1));
}

int CurveData::set (int k, Data *d)
{
  if (k < 0)
  {
    Data *td = _data.value(k);
    if (td)
      delete td;

    _data.insert(k, d);
  }
  else
  {
    Data *td = _bars.value(k);
    if (td)
      delete td;

    _bars.insert(k, d);

    if (k < _startIndex)
      _startIndex = k;

    if (k > _endIndex)
      _endIndex = k;
  }

  return 0;
}

Data * CurveData::toData (int d)
{
  Data *td = 0;
  if (d < 0)
    td = _data.value(d);
  else
    td = _bars.value(d);

  return td;
}

QList<int> CurveData::keys ()
{
  return _bars.keys();
}

int CurveData::keyCount ()
{
  return _bars.count();
}

void CurveData::keyRange (int &start, int &end)
{
  start = _startIndex;
  end = _endIndex;
}
