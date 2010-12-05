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

#include "QuoteBar.h"

#include <QtDebug>

QuoteBar::QuoteBar ()
{
  _length = DailyBar;
  _type = "Stock";
}

void QuoteBar::setDateRange (QDateTime &dt, QuoteBar::BarLength l)
{
  _length = l;
  _startDate = dt;

  switch (_length)
  {
    case QuoteBar::Minute1:
      _startDate.setTime(QTime(_startDate.time().hour(), _startDate.time().minute(), 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(60);
      break;
    case QuoteBar::Minute5:
    {
      int tint = _startDate.time().minute() / 5;
      _startDate.setTime(QTime(_startDate.time().hour(), tint * 5, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(300);
      break;
    }
    case QuoteBar::Minute10:
    {
      int tint = _startDate.time().minute() / 10;
      _startDate.setTime(QTime(_startDate.time().hour(), tint * 10, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(600);
      break;
    }
    case QuoteBar::Minute15:
    {
      int tint = _startDate.time().minute() / 15;
      _startDate.setTime(QTime(_startDate.time().hour(), tint * 15, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(900);
      break;
    }
    case QuoteBar::Minute30:
    {
      int tint = _startDate.time().minute() / 30;
      _startDate.setTime(QTime(_startDate.time().hour(), tint * 30, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(1800);
      break;
    }
    case QuoteBar::Minute60:
      _startDate.setTime(QTime(_startDate.time().hour(), 0, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(3600);
      break;
    case QuoteBar::DailyBar:
      _startDate.setTime(QTime(0, 0, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addDays(1);
      break;
    case QuoteBar::WeeklyBar:
      _startDate.setTime(QTime(0, 0, 0, 0));
      _startDate = _startDate.addDays(- _startDate.date().dayOfWeek());
      _endDate = _startDate;
      _endDate = _endDate.addDays(7);
      break;
    case QuoteBar::MonthlyBar:
      _startDate.setTime(QTime(0, 0, 0, 0));
      _startDate = _startDate.addDays(- (_startDate.date().day() - 1));
      _endDate = _startDate;
      _endDate = _endDate.addDays(_endDate.date().daysInMonth());
      break;
    default:
      break;
  }
}

QDateTime & QuoteBar::startDate ()
{
  return _startDate;
}

QDateTime & QuoteBar::endDate ()
{
  return _endDate;
}

void QuoteBar::setDate (QDateTime &d)
{
  _startDate = d;
}

int QuoteBar::setDate (QString s)
{
  _startDate = QDateTime::fromString(s, QString("yyyyMMddHHmmss"));
  if (! _startDate.isValid())
    return 1;

  return 0;
}

int QuoteBar::setOpen (QString s)
{
  bool ok;
  s.toDouble(&ok);
  if (! ok)
    return 1;

  _open = s;

  return 0;
}

QString & QuoteBar::open ()
{
  return _open;
}

int QuoteBar::setHigh (QString s)
{
  bool ok;
  s.toDouble(&ok);
  if (! ok)
    return 1;

  _high = s;

  return 0;
}

QString & QuoteBar::high ()
{
  return _high;
}

int QuoteBar::setLow (QString s)
{
  bool ok;
  s.toDouble(&ok);
  if (! ok)
    return 1;

  _low = s;

  return 0;
}

QString & QuoteBar::low ()
{
  return _low;
}

int QuoteBar::setClose (QString s)
{
  bool ok;
  s.toDouble(&ok);
  if (! ok)
    return 1;

  _close = s;

  return 0;
}

QString & QuoteBar::close ()
{
  return _close;
}

int QuoteBar::setVolume (QString s)
{
  bool ok;
  s.toDouble(&ok);
  if (! ok)
    return 1;

  _volume = s;

  return 0;
}

QString & QuoteBar::volume ()
{
  return _volume;
}

int QuoteBar::setOI (QString s)
{
  bool ok;
  s.toDouble(&ok);
  if (! ok)
    return 1;

  _oi = s;

  return 0;
}

QString & QuoteBar::oi ()
{
  return _oi;
}

void QuoteBar::rangeKey (QString &d)
{
  d.clear();
  d = _startDate.toString(QString("yyyyMMddHHmmss")) + _endDate.toString(QString("yyyyMMddHHmmss"));
}

void QuoteBar::string (QString &d)
{
  d.clear();

  QStringList l;
  
  l << _startDate.toString(QString("yyyyMMddHHmmss"));

  switch (_length)
  {
    case QuoteBar::Minute1:
    case QuoteBar::Minute5:
    case QuoteBar::Minute10:
    case QuoteBar::Minute15:
    case QuoteBar::Minute30:
    case QuoteBar::Minute60:
      l << _endDate.toString(QString("yyyyMMddHHmmss"));
      break;
    case QuoteBar::DailyBar:
    case QuoteBar::WeeklyBar:
    case QuoteBar::MonthlyBar:
    {
      QDateTime dt = _endDate.addSecs(-1);
      l << dt.toString(QString("yyyyMMddHHmmss"));
      break;
    }
    default:
      break;
  }

  l << _open << _high << _low << _close << _volume << _oi;

  d = l.join(",");
}

void QuoteBar::lengthList (QStringList &l)
{
  l.clear();
  l << "1" << "5" << "10" << "15" << "30" << "60" << "D" << "W" << "M";
}

int QuoteBar::verify ()
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

  double oi = _oi.toDouble();
  if (oi < 0)
    return 1;

  return 0;
}

void QuoteBar::setExchange (QString s)
{
  _exchange = s;
}

QString & QuoteBar::exchange ()
{
  return _exchange;
}

void QuoteBar::setSymbol (QString s)
{
  _symbol = s;
  _name = s;
}

QString & QuoteBar::symbol ()
{
  return _symbol;
}

void QuoteBar::setName (QString s)
{
  _name = s;
}

QString & QuoteBar::name ()
{
  return _name;
}

void QuoteBar::setType (QString s)
{
  _type = s;
}

QString & QuoteBar::type ()
{
  return _type;
}
