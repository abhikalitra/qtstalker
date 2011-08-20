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
  _length = -1;
  _open = 0;
  _high = 0;
  _low = 0;
  _close = 0;
  _volume = 0;
  _oi = 0;
}

void Bar::setDateRange (QDateTime dt, BarLength::Length l)
{
  _length = (int) l;
  _startDate = dt;

  switch ((BarLength::Length) _length)
  {
    case BarLength::_NONE:
      _endDate = _startDate;
      break;
    case BarLength::_MINUTE1:
      _startDate.setTime(QTime(_startDate.time().hour(), _startDate.time().minute(), 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(60);
      _endDate = _endDate.addSecs(-1);
      break;
    case BarLength::_MINUTE5:
    {
      int tint = _startDate.time().minute() / 5;
      _startDate.setTime(QTime(_startDate.time().hour(), tint * 5, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(300);
      _endDate = _endDate.addSecs(-1);
      break;
    }
    case BarLength::_MINUTE10:
    {
      int tint = _startDate.time().minute() / 10;
      _startDate.setTime(QTime(_startDate.time().hour(), tint * 10, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(600);
      _endDate = _endDate.addSecs(-1);
      break;
    }
    case BarLength::_MINUTE15:
    {
      int tint = _startDate.time().minute() / 15;
      _startDate.setTime(QTime(_startDate.time().hour(), tint * 15, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(900);
      _endDate = _endDate.addSecs(-1);
      break;
    }
    case BarLength::_MINUTE30:
    {
      int tint = _startDate.time().minute() / 30;
      _startDate.setTime(QTime(_startDate.time().hour(), tint * 30, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(1800);
      _endDate = _endDate.addSecs(-1);
      break;
    }
    case BarLength::_MINUTE60:
      _startDate.setTime(QTime(_startDate.time().hour(), 0, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(3600);
      _endDate = _endDate.addSecs(-1);
      break;
    case BarLength::_DAILY:
      _startDate.setTime(QTime(0, 0, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addDays(1);
      _endDate = _endDate.addSecs(-1);
      break;
    case BarLength::_WEEKLY:
      _startDate.setTime(QTime(0, 0, 0, 0));
      _startDate = _startDate.addDays(- _startDate.date().dayOfWeek());
      _endDate = _startDate;
      _endDate = _endDate.addDays(7);
      _endDate = _endDate.addSecs(-1);
      break;
    case BarLength::_MONTHLY:
      _startDate.setTime(QTime(0, 0, 0, 0));
      _startDate = _startDate.addDays(- (_startDate.date().day() - 1));
      _endDate = _startDate;
      _endDate = _endDate.addDays(_endDate.date().daysInMonth());
      _endDate = _endDate.addSecs(-1);
      break;
    default:
      break;
  }
}

int Bar::setDates (QString start, QString end)
{
  _startDate = QDateTime::fromString(start, Qt::ISODate);
  if (! _startDate.isValid())
    return TRUE;

  _endDate = QDateTime::fromString(end, Qt::ISODate);
  if (! _endDate.isValid())
    return TRUE;

  return FALSE;
}

int Bar::setDates (QDateTime start, QDateTime end)
{
  _startDate = start;
  if (! _startDate.isValid())
    return TRUE;

  _endDate = end;
  if (! _endDate.isValid())
    return TRUE;

  return FALSE;
}

void Bar::dateString (QString &d)
{
  d = _lastDate.toString("yyyy-MM-dd");
}

void Bar::dateTimeString (QString &d)
{
  d = _lastDate.toString(Qt::ISODate);
}

void Bar::timeString (QString &d)
{
  d = _lastDate.toString("HH:mm:ss");
}

void Bar::rangeKey (QString &d)
{
  d = _startDate.toString(Qt::ISODate) + _endDate.toString(Qt::ISODate);
}

QDateTime & Bar::date ()
{
  return _lastDate;
}

int Bar::setOpen (QString d)
{
  bool ok;
  double v = d.toDouble(&ok);
  if (! ok)
    return 1;

  _open = v;

  return 0;
}

void Bar::setOpen (double d)
{
  _open = d;
}

double Bar::open ()
{
  return _open;
}

int Bar::setHigh (QString d)
{
  bool ok;
  double v = d.toDouble(&ok);
  if (! ok)
    return 1;

  _high = v;

  return 0;
}

void Bar::setHigh (double d)
{
  _high = d;
}

double Bar::high ()
{
  return _high;
}

int Bar::setLow (QString d)
{
  bool ok;
  double v = d.toDouble(&ok);
  if (! ok)
    return 1;

  _low = v;

  return 0;
}

void Bar::setLow (double d)
{
  _low = d;
}

double Bar::low ()
{
  return _low;
}

int Bar::setClose (QString d)
{
  bool ok;
  double v = d.toDouble(&ok);
  if (! ok)
    return 1;

  _close = v;

  return 0;
}

void Bar::setClose (double d)
{
  _close = d;
}

double Bar::close ()
{
  return _close;
}

int Bar::setVolume (QString d)
{
  bool ok;
  double v = d.toDouble(&ok);
  if (! ok)
    return 1;

  _volume = v;

  return 0;
}

void Bar::setVolume (double d)
{
  _volume = d;
}

double Bar::volume ()
{
  return _volume;
}

int Bar::setOI (QString d)
{
  bool ok;
  double v = d.toDouble(&ok);
  if (! ok)
    return 1;

  _oi = v;

  return 0;
}

void Bar::setOI (double d)
{
  _oi = d;
}

double Bar::oi ()
{
  return _oi;
}

QString Bar::toString ()
{
  QStringList l;
  l << _startDate.toString(Qt::ISODate);
  l << _endDate.toString(Qt::ISODate);
  l << QString::number(_open);
  l << QString::number(_high);
  l << QString::number(_low);
  l << QString::number(_close);
  l << QString::number(_volume);
  l << QString::number(_oi);
  l << QString::number(_length);
  l << _lastDate.toString(Qt::ISODate);
  return l.join(",");
}

int Bar::fromString (QString d)
{
  QStringList l = d.split(",");
  if (l.count() != 10)
    return 1;

  setDates(l.at(0), l.at(1));
  int pos = 2;
  setOpen(l.at(pos++));
  setHigh(l.at(pos++));
  setLow(l.at(pos++));
  setClose(l.at(pos++));
  setVolume(l.at(pos++));
  setOI(l.at(pos++));
  setLength(l.at(pos++).toInt());
  setLastDate(QDateTime::fromString(l.at(pos++), Qt::ISODate));
  return 0;
}

QDateTime Bar::startDate ()
{
  return _startDate;
}

void Bar::setLastDate (QDateTime d)
{
  _lastDate = d;
}

QDateTime Bar::lastDate ()
{
  return _lastDate;
}

void Bar::copy (Bar *d)
{
  d->setOpen(_open);
  d->setHigh(_high);
  d->setLow(_low);
  d->setClose(_close);
  d->setVolume(_volume);
  d->setOI(_oi);
  d->setDates(_startDate, _endDate);
  d->setLength(_length);
  d->setLastDate(_lastDate);
}

int Bar::length ()
{
  return _length;
}

void Bar::setLength (int d)
{
  _length = d;
}
