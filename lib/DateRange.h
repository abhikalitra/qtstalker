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

#include "TypeBarLength.h"
#include "TypeDateRange.h"

class DateRange
{
  public:
    DateRange ();
    int dateRange (TypeDateRange::Key, QDateTime &input, QDateTime &output);
    void dateInterval (QDateTime dt, TypeBarLength::Key length, QDateTime &startDate, QDateTime &endDate);
    QString rangeKey (QDateTime sd, QDateTime ed);
};

#endif
