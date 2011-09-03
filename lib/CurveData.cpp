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

#include <QtDebug>
#include <QStringList>

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
  Data::clear();

  _type = "CURVE";
  _startIndex = 99999999;
  _endIndex = -99999999;

  qDeleteAll(_bars);
  _bars.clear();
}

int CurveData::set (int k, Data *d)
{
  Data *b = _bars.value(k);
  if (b)
    delete b;

  _bars.insert(k, d);

  if (k < _startIndex)
    _startIndex = k;
  if (k > _endIndex)
    _endIndex = k;

  return 0;
}

Data * CurveData::getData (int d)
{
  return _bars.value(d);
}

QList<int> CurveData::barKeys ()
{
  return _bars.keys();
}

QString CurveData::toString ()
{
  QStringList l;

  QHashIterator<QString, QVariant> it(_data);
  while (it.hasNext())
  {
    it.next();
    l << it.key() + "=" + it.value().toString();
  }

  QHashIterator<int, Data *> it2(_bars);
  while (it2.hasNext())
  {
    it2.next();
    l << QString::number(it2.key()) + "=" + it2.value()->toString();
  }

  return l.join("\n");
}

int CurveData::fromString (QString d)
{
  clear();

  QStringList l = d.split("\n");

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QStringList tl = l.at(loop).split("=");
    if (tl.count() != 2)
      continue;

    int k = tl.at(0).toInt();

    switch ((Parm) k)
    {
      case _LABEL:
      case _TYPE:
      case _CHART:
      case _Z:
      case _PEN:
      case _STYLE:
        _data.insert(tl.at(0), QVariant(tl.at(1)));
        break;
      default:
      {
        CurveBar *b = new CurveBar;
        b->fromString(tl.at(1));
        set(k, b);
        break;
      }
    }
  }

  return 0;
}

int CurveData::barKeyCount ()
{
  return _bars.count();
}

void CurveData::barKeyRange (int &start, int &end)
{
  start = _startIndex;
  end = _endIndex;
}
