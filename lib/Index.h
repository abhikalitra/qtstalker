/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2006 Stefan S. Stratigakos
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

#ifndef INDEX_HPP
#define INDEX_HPP

#include <qstring.h>
#include <qdatetime.h>
#include <qdict.h>
#include "BarData.h"
#include "Bar.h"

class Index
{
  public:
    Index ();
    ~Index ();
    void getHistory (BarData *, QDateTime &, QString &, int, BarData::BarLength);
    void loadIndexData (QString &symbol, QDict<Bar> &lookup, QDateTime &startDate, float weight,
                        int barRange, BarData::BarLength barLength);
};

#endif
