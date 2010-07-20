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

#include "Bar.h"

#include <QtDebug>

Bar::Bar ()
{
}

int Bar::setDates (QString &start, QString &end)
{
  _startDate = QDateTime::fromString(start, "yyyyMMddHHmmss");
  if (! _startDate.isValid())
    return TRUE;

  _endDate = QDateTime::fromString(end, "yyyyMMddHHmmss");
  if (! _endDate.isValid())
    return TRUE;

  return FALSE;
}

int Bar::setData (BarField k, QString &d)
{
  bool ok;
  double v = d.toDouble(&ok);
  if (! ok)
    return 1;

  _data.insert(k, v);

  return 0;
}

double Bar::getData (BarField k)
{
  return _data.value(k);
}

void Bar::getDateString (QString &d)
{
  d = _endDate.toString("yyyy-MM-dd");
}

void Bar::getDateTimeString (QString &d)
{
  d = _endDate.toString(Qt::ISODate);
}

void Bar::getTimeString (QString &d)
{
  d = _endDate.toString("HH:mm:ss");
}

int Bar::count ()
{
  return _data.count();
}

void Bar::getRangeKey (QString &d)
{
  d = _startDate.toString(Qt::ISODate) + _endDate.toString(Qt::ISODate);
}

QDateTime & Bar::date ()
{
  return _endDate;
}

double Bar::getOpen ()
{
  return _data.value(BarFieldOpen);
}

double Bar::getHigh ()
{
  return _data.value(BarFieldHigh);
}

double Bar::getLow ()
{
  return _data.value(BarFieldLow);
}

double Bar::getClose ()
{
  return _data.value(BarFieldClose);
}

double Bar::getVolume ()
{
  return _data.value(BarFieldVolume);
}

