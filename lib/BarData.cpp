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

#include "BarData.h"

#include <QObject>
#include <QtDebug>

BarData::BarData ()
{
  clear();
}

BarData::~BarData ()
{
  if (_barList.count())
    qDeleteAll(_barList);
}

void BarData::clear ()
{
  if (_barList.count())
    qDeleteAll(_barList);

  _high = -99999999;
  _low = 99999999;
  _length = -1;
  _maxBars = 0;
  _range = -1;
  _type = "Stock";
}

int BarData::count ()
{
  return _barList.count();
}

void BarData::prepend (Bar *bar)
{
  _barList.prepend(bar);
}

void BarData::append (Bar *bar)
{
  _barList.append(bar);
}

double BarData::max ()
{
  return _high;
}

double BarData::min ()
{
  return _low;
}

Bar * BarData::bar (int d)
{
  if (! _barList.count())
    return 0;
  else
  {
    if (d < 0 || d >= _barList.count())
      return 0;
  }

  return _barList.at(d);
}

void BarData::setMinMax ()
{
  int loop;
  for (loop = 0; loop < (int) _barList.count(); loop++)
  {
    Bar *bar = _barList.at(loop);

    if (bar->high() > _high)
      _high = bar->high();

    if (bar->low() < _low)
      _low = bar->low();
  }
}

void BarData::setBarLength (int d)
{
  _length = d;
}

int BarData::barLength ()
{
  return _length;
}

QString & BarData::symbol ()
{
  return _symbol;
}

void BarData::setSymbol (QString d)
{
  _symbol = d;
}

QString & BarData::name ()
{
  return _name;
}

void BarData::setName (QString d)
{
  _name = d;
}

QString & BarData::exchange ()
{
  return _exchange;
}

void BarData::setExchange (QString d)
{
  _exchange = d;
}

int BarData::setKey (QString d)
{
  QStringList l = d.split(":");
  if (l.count() != 2)
    return 1;

  _exchange  = l.at(0);
  _symbol = l.at(1);

  return 0;
}

QString BarData::key ()
{
  QString s = _exchange + ":" + _symbol;
  return s;
}

QString BarData::toString ()
{
  QStringList l;
  l << _exchange;
  l << _symbol;
  l << QString::number(_length);
  l << QString::number (_range);

  int loop = 0;
  for (; loop < _barList.count(); loop++)
  {
    Bar *b = _barList.at(loop);
    if (! b)
      continue;

    l << b->toString();
  }

  return l.join(";");
}

int BarData::fromString (QString d)
{
  QStringList l = d.split(";", QString::SkipEmptyParts);

  if (l.count() < 4)
    return 1;

  int loop = 0;
  _exchange = l.at(loop++);
  _symbol = l.at(loop++);
  _length = l.at(loop++).toInt();
  _range = l.at(loop++).toInt();

  for (; loop < l.count(); loop++)
  {
    Bar *b = new Bar;
    if (b->fromString(l.at(loop)))
    {
      delete b;
      continue;
    }

    append(b);
  }

  return 0;
}

int BarData::maxBars ()
{
  return _maxBars;
}

void BarData::setMaxBars (int d)
{
  _maxBars = d;
}

QDateTime BarData::startDate ()
{
  return _startDate;
}

void BarData::setStartDate (QDateTime d)
{
  _startDate = d;
}

QString BarData::startDateString ()
{
  return _startDate.toString("yyyyMMddHHmmss");
}

QDateTime BarData::endDate ()
{
  return _endDate;
}

void BarData::setEndDate (QDateTime d)
{
  _endDate = d;
}

QString BarData::endDateString ()
{
  return _endDate.toString("yyyyMMddHHmmss");
}

QString BarData::table ()
{
  return _table;
}

void BarData::setTable (QString d)
{
  _table = d;
}

QString BarData::type ()
{
  return _type;
}

void BarData::setType (QString d)
{
  _type = d;
}

int BarData::range ()
{
  return _range;
}

void BarData::setRange (int d)
{
  _range = d;
}
