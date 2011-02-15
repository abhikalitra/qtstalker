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

int DateRange::dateRange (DateRange::Range type, QDateTime &input, QDateTime &output)
{
  if (! input.isValid())
    return 1;
  
  switch (type)
  {
    case Day:
      output = input.addDays(-1);
      break;
    case Week:
      output = input.addDays(-7);
      break;
    case Month:
      output = input.addDays(-31);
      break;
    case Month3:
      output = input.addDays(-63);
      break;
    case Month6:
      output = input.addDays(-186);
      break;
    case Year:
      output = input.addYears(-1);
      break;
    case Year2:
      output = input.addYears(-2);
      break;
    case Year5:
      output = input.addYears(-5);
      break;
    case Year10:
      output = input.addYears(-10);
      break;
    case Year25:
      output = input.addYears(-25);
      break;
    case Year50:
      output = input.addYears(-50);
      break;
    case All:
      output = input.addYears(-1000);
      break;
    default:
      return 1;
      break;
  }

  return 0;
}

void DateRange::list (QStringList &l)
{
  l << "1 Day" << "1 Week" << "1 Month" << "3 Months" << "6 Months" << "1 Year";
  l << "2 Years" << "5 Years" << "10 Years" << "25 Years" << "50 Years" << "All";
}

int DateRange::toType (QString d)
{
  QStringList l;
  list(l);
  return l.indexOf(d);
}

void DateRange::dateRangeText (DateRange::Range k, QString &d)
{
  d.clear();

  switch (k)
  {
    case Day:
      d = "1D";
      break;
    case Week:
      d = "1W";
      break;
    case Month:
      d = "1M";
      break;
    case Month3:
      d = "3M";
      break;
    case Month6:
      d = "6M";
      break;
    case Year:
      d = "1Y";
      break;
    case Year2:
      d = "2Y";
      break;
    case Year5:
      d = "5Y";
      break;
    case Year10:
      d = "10Y";
      break;
    case Year25:
      d = "25Y";
      break;
    case Year50:
      d = "50Y";
      break;
    case All:
      d = "All";
      break;
    default:
      break;
  }
}

