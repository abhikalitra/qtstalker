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

#include "QSDateRange.h"

#include <QDebug>

QSDateRange::QSDateRange ()
{
}

int QSDateRange::dateRange (QSDateRange::Range type, QDateTime &input, QDateTime &output)
{
  if (! input.isValid())
    return 1;
  
  switch (type)
  {
    case D1:
      output = input.addDays(-1);
      break;
    case W1:
      output = input.addDays(-7);
      break;
    case M1:
      output = input.addDays(-31);
      break;
    case M3:
      output = input.addDays(-63);
      break;
    case M6:
      output = input.addDays(-186);
      break;
    case Y1:
      output = input.addYears(-1);
      break;
    case Y2:
      output = input.addYears(-2);
      break;
    case Y5:
      output = input.addYears(-5);
      break;
    case Y10:
      output = input.addYears(-10);
      break;
    case Y25:
      output = input.addYears(-25);
      break;
    case Y50:
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

void QSDateRange::list (QStringList &l)
{
  l << "D1" << "W1" << "M1" << "M3" << "M6" << "Y1" << "Y2" << "Y5" << "Y10" << "Y25" << "Y50" << "*";
}

