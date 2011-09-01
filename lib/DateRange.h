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

// *************************************************************************************************
// Input the Range and input datetime and the output datetime will be a ranged back from the
// input. eg. Range is Day, output datetime will be one day back from the input datetime
// *************************************************************************************************

#ifndef DATE_RANGE_HPP
#define DATE_RANGE_HPP

#include <QDateTime>
#include <QStringList>

#include "BarLength.h"

class DateRange
{
  public:
    enum Range
    {
      _DAY,
      _WEEK,
      _MONTH,
      _MONTH3,
      _MONTH6,
      _YEAR,
      _YEAR2,
      _YEAR5,
      _YEAR10,
      _YEAR25,
      _YEAR50,
      _ALL
    };

    DateRange ();
    int dateRange (DateRange::Range, QDateTime &input, QDateTime &output);
    QStringList list ();
    int toType (QString);
    void dateRangeText (DateRange::Range, QString &);
    void dateInterval (QDateTime dt, BarLength::Length length, QDateTime &startDate, QDateTime &endDate);
    QString rangeKey (QDateTime sd, QDateTime ed);

  private:
    QStringList _list;
};

#endif
