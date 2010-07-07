/*
 *  QuoteServer
 *
 *  Copyright (C) 2010 Stefan S. Stratigakos
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
  _open = "0";
  _high = "0";
  _low = "0";
  _close = "0";
  _volume = "0";
  _oi = "0";
  _length = DailyBar;
  _type = Stock;
}

void Bar::setDateRange (QDateTime &dt, Bar::BarLength l)
{
  _length = l;
  _startDate = dt;

  switch (_length)
  {
    case Bar::Minute1:
      _startDate.setTime(QTime(_startDate.time().hour(), _startDate.time().minute(), 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(60);
      break;
    case Bar::Minute5:
    {
      int tint = _startDate.time().minute() / 5;
      _startDate.setTime(QTime(_startDate.time().hour(), tint * 5, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(300);
      break;
    }
    case Bar::Minute10:
    {
      int tint = _startDate.time().minute() / 10;
      _startDate.setTime(QTime(_startDate.time().hour(), tint * 10, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(600);
      break;
    }
    case Bar::Minute15:
    {
      int tint = _startDate.time().minute() / 15;
      _startDate.setTime(QTime(_startDate.time().hour(), tint * 15, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(900);
      break;
    }
    case Bar::Minute30:
    {
      int tint = _startDate.time().minute() / 30;
      _startDate.setTime(QTime(_startDate.time().hour(), tint * 30, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(1800);
      break;
    }
    case Bar::Minute60:
      _startDate.setTime(QTime(_startDate.time().hour(), 0, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(3600);
      break;
    case Bar::DailyBar:
      _startDate.setTime(QTime(0, 0, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addDays(1);
//      _endDate = _endDate.addSecs(-1);
      break;
    case Bar::WeeklyBar:
      _startDate.setTime(QTime(0, 0, 0, 0));
      _startDate = _startDate.addDays(- _startDate.date().dayOfWeek());
      _endDate = _startDate;
      _endDate = _endDate.addDays(7);
//      _endDate = _endDate.addSecs(-1);
      break;
    case Bar::MonthlyBar:
      _startDate.setTime(QTime(0, 0, 0, 0));
      _startDate = _startDate.addDays(- (_startDate.date().day() - 1));
      _endDate = _startDate;
      _endDate = _endDate.addDays(_endDate.date().daysInMonth());
//      _endDate = _endDate.addSecs(-1);
      break;
    default:
      break;
  }
}

QDateTime & Bar::startDate ()
{
  return _startDate;
}

QDateTime & Bar::endDate ()
{
  return _endDate;
}

void Bar::setDate (QDateTime &d)
{
  _startDate = d;
}

int Bar::setDate (QString &s)
{
  _startDate = QDateTime::fromString(s, QString("yyyyMMddHHmmss"));
  if (! _startDate.isValid())
    return 1;

  return 0;
}

int Bar::setOpen (QString s)
{
  bool ok;
  s.toDouble(&ok);
  if (! ok)
    return 1;

  _open = s;

  return 0;
}

QString & Bar::open ()
{
  return _open;
}

int Bar::setHigh (QString s)
{
  bool ok;
  s.toDouble(&ok);
  if (! ok)
    return 1;

  _high = s;

  return 0;
}

QString & Bar::high ()
{
  return _high;
}

int Bar::setLow (QString s)
{
  bool ok;
  s.toDouble(&ok);
  if (! ok)
    return 1;

  _low = s;

  return 0;
}

QString & Bar::low ()
{
  return _low;
}

int Bar::setClose (QString s)
{
  bool ok;
  s.toDouble(&ok);
  if (! ok)
    return 1;

  _close = s;

  return 0;
}

QString & Bar::close ()
{
  return _close;
}

int Bar::setVolume (QString s)
{
  bool ok;
  s.toDouble(&ok);
  if (! ok)
    return 1;

  _volume = s;

  return 0;
}

QString & Bar::volume ()
{
  return _volume;
}

int Bar::setOI (QString s)
{
  bool ok;
  s.toDouble(&ok);
  if (! ok)
    return 1;

  _oi = s;

  return 0;
}

QString & Bar::oi ()
{
  return _oi;
}

void Bar::rangeKey (QString &d)
{
  d.clear();
  d = _startDate.toString(QString("yyyyMMddHHmmss")) + _endDate.toString(QString("yyyyMMddHHmmss"));
}

void Bar::string (QString &d)
{
  d.clear();

  QStringList l;
  
  switch (_length)
  {
    case Bar::Minute1:
    case Bar::Minute5:
    case Bar::Minute10:
    case Bar::Minute15:
    case Bar::Minute30:
    case Bar::Minute60:
      l << _endDate.toString(QString("yyyyMMddHHmmss"));
      break;
    case Bar::DailyBar:
    case Bar::WeeklyBar:
    case Bar::MonthlyBar:
    {
      QDateTime dt = _endDate.addSecs(-1);
      l << dt.toString(QString("yyyyMMddHHmmss"));
      break;
    }
    default:
      break;
  }

  l << _open << _high << _low << _close << _volume;
  if (_type == Futures)
    l << _oi;

  d = l.join(",");
}

void Bar::lengthList (QStringList &l)
{
  l.clear();
  l << "1" << "5" << "10" << "15" << "30" << "60" << "D" << "W" << "M";
}

int Bar::verify ()
{
  double o = _open.toDouble();
  double h = _high.toDouble();
  double l = _low.toDouble();
  
  if (o > h || o < l)
    return 1;
  
  if (h < l || l > h)
    return 1;

  double c = _close.toDouble();
  
  if (c > h || c < l)
    return 1;

  double v = _volume.toDouble();
  if (v < 0)
    return 1;

  if (_type == Futures)
  {
    double oi = _oi.toDouble();
    if (oi < 0)
      return 1;
  }

  return 0;
}

void Bar::setBarType (Bar::BarType d)
{
  _type = d;
}

Bar::BarType Bar::barType ()
{
  return _type;
}

void Bar::typeList (QStringList &l)
{
  l.clear();
  l << "S" << "F";
}

