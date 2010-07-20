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

#ifndef BAR_HPP
#define BAR_HPP

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QHash>

class Bar
{
  public:
    enum BarField
    {
      BarFieldOpen,
      BarFieldHigh,
      BarFieldLow,
      BarFieldClose,
      BarFieldVolume,
      BarFieldOI
    };

    Bar ();
    int setDates (QString &start, QString &end);
    int setData (BarField, QString &);
    double getData (BarField);
    void getDateString (QString &);
    void getDateTimeString (QString &);
    void getTimeString (QString &);
    int count ();
    void getRangeKey (QString &);
    QDateTime & date ();

    double getOpen ();
    double getHigh ();
    double getLow ();
    double getClose ();
    double getVolume ();

  protected:
    QHash<int, double> _data;
    QDateTime _startDate;
    QDateTime _endDate;
};

#endif
