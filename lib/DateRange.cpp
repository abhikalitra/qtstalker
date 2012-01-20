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

#include "DateRange.h"

#include <QDebug>

DateRange::DateRange ()
{
}

int DateRange::dateRange (TypeDateRange::Key type, QDateTime &input, QDateTime &output)
{
  if (! input.isValid())
    return 1;

  switch (type)
  {
    case TypeDateRange::_DAY:
      output = input.addDays(-1);
      break;
    case TypeDateRange::_WEEK:
      output = input.addDays(-7);
      break;
    case TypeDateRange::_MONTH:
      output = input.addDays(-31);
      break;
    case TypeDateRange::_MONTH3:
      output = input.addDays(-63);
      break;
    case TypeDateRange::_MONTH6:
      output = input.addDays(-186);
      break;
    case TypeDateRange::_YEAR:
      output = input.addYears(-1);
      break;
    case TypeDateRange::_YEAR2:
      output = input.addYears(-2);
      break;
    case TypeDateRange::_YEAR5:
      output = input.addYears(-5);
      break;
    case TypeDateRange::_YEAR10:
      output = input.addYears(-10);
      break;
    case TypeDateRange::_YEAR25:
      output = input.addYears(-25);
      break;
    case TypeDateRange::_YEAR50:
      output = input.addYears(-50);
      break;
    case TypeDateRange::_ALL:
      output = input.addYears(-1000);
      break;
    default:
      return 1;
      break;
  }

  return 0;
}

void DateRange::dateInterval (QDateTime dt, TypeBarLength::Key length, QDateTime &startDate, QDateTime &endDate)
{
  startDate = dt;

  switch ((TypeBarLength::Key) length)
  {
    case TypeBarLength::_NONE:
      endDate = startDate;
      break;
    case TypeBarLength::_MINUTE1:
      startDate.setTime(QTime(startDate.time().hour(), startDate.time().minute(), 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(60);
      endDate = endDate.addSecs(-1);
      break;
    case TypeBarLength::_MINUTE5:
    {
      int tint = startDate.time().minute() / 5;
      startDate.setTime(QTime(startDate.time().hour(), tint * 5, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(300);
      endDate = endDate.addSecs(-1);
      break;
    }
    case TypeBarLength::_MINUTE10:
    {
      int tint = startDate.time().minute() / 10;
      startDate.setTime(QTime(startDate.time().hour(), tint * 10, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(600);
      endDate = endDate.addSecs(-1);
      break;
    }
    case TypeBarLength::_MINUTE15:
    {
      int tint = startDate.time().minute() / 15;
      startDate.setTime(QTime(startDate.time().hour(), tint * 15, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(900);
      endDate = endDate.addSecs(-1);
      break;
    }
    case TypeBarLength::_MINUTE30:
    {
      int tint = startDate.time().minute() / 30;
      startDate.setTime(QTime(startDate.time().hour(), tint * 30, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(1800);
      endDate = endDate.addSecs(-1);
      break;
    }
    case TypeBarLength::_MINUTE60:
      startDate.setTime(QTime(startDate.time().hour(), 0, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(3600);
      endDate = endDate.addSecs(-1);
      break;
    case TypeBarLength::_DAILY:
      startDate.setTime(QTime(0, 0, 0, 0));
      endDate = startDate;
      endDate = endDate.addDays(1);
      endDate = endDate.addSecs(-1);
      break;
    case TypeBarLength::_WEEKLY:
      startDate.setTime(QTime(0, 0, 0, 0));
      startDate = startDate.addDays(- startDate.date().dayOfWeek());
      endDate = startDate;
      endDate = endDate.addDays(7);
      endDate = endDate.addSecs(-1);
      break;
    case TypeBarLength::_MONTHLY:
      startDate.setTime(QTime(0, 0, 0, 0));
      startDate = startDate.addDays(- (startDate.date().day() - 1));
      endDate = startDate;
      endDate = endDate.addDays(endDate.date().daysInMonth());
      endDate = endDate.addSecs(-1);
      break;
    default:
      break;
  }
}

QString DateRange::rangeKey (QDateTime sd, QDateTime ed)
{
  QString s = sd.toString(Qt::ISODate) + ed.toString(Qt::ISODate);
  return s;
}
