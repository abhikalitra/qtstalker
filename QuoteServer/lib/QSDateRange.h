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

#ifndef QS_DATE_RANGE_HPP
#define QS_DATE_RANGE_HPP

#include <QDateTime>
#include <QStringList>

class QSDateRange
{
  public:
    enum Range
    {
      D1,
      W1,
      M1,
      M3,
      M6,
      Y1,
      Y2,
      Y5,
      Y10,
      Y25,
      Y50,
      All
    };
    
    QSDateRange ();
    int dateRange (QSDateRange::Range, QDateTime &input, QDateTime &output);
    void list (QStringList &);
};

#endif