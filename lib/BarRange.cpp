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

#include "BarRange.h"

#include <QtDebug>

BarRange::BarRange ()
{
}

void BarRange::setBarRange (QDateTime &dt, BarData::BarLength l)
{
  _startDate = dt;

  switch ((BarData::BarLength) l)
  {
    case BarData::Minute1:
      _startDate.setTime(QTime(_startDate.time().hour(), _startDate.time().minute(), 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(60);
      break;
    case BarData::Minute5:
    {
      int tint = _startDate.time().minute() / 5;
      _startDate.setTime(QTime(_startDate.time().hour(), tint * 5, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(300);
      break;
    }
    case BarData::Minute10:
    {
      int tint = _startDate.time().minute() / 10;
      _startDate.setTime(QTime(_startDate.time().hour(), tint * 10, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(600);
      break;
    }
    case BarData::Minute15:
    {
      int tint = _startDate.time().minute() / 15;
      _startDate.setTime(QTime(_startDate.time().hour(), tint * 15, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(900);
      break;
    }
    case BarData::Minute30:
    {
      int tint = _startDate.time().minute() / 30;
      _startDate.setTime(QTime(_startDate.time().hour(), tint * 30, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(1800);
      break;
    }
    case BarData::Minute60:
      _startDate.setTime(QTime(_startDate.time().hour(), 0, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addSecs(3600);
      break;
    case BarData::DailyBar:
      _startDate.setTime(QTime(0, 0, 0, 0));
      _endDate = _startDate;
      _endDate = _endDate.addDays(1);
      _endDate = _endDate.addSecs(-1);
      break;
    case BarData::WeeklyBar:
      _startDate.setTime(QTime(0, 0, 0, 0));
      _startDate = _startDate.addDays(- _startDate.date().dayOfWeek());
      _endDate = _startDate;
      _endDate = _endDate.addDays(7);
      _endDate = _endDate.addSecs(-1);
      break;
    case BarData::MonthlyBar:
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

QDateTime & BarRange::startDate ()
{
  return _startDate;
}

QDateTime & BarRange::endDate ()
{
  return _endDate;
}

void BarRange::key (QString &d)
{
  d.clear();
  d = _startDate.toString(Qt::ISODate) + _endDate.toString(Qt::ISODate);
}

