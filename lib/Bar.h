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

    enum RC
    {
      RC_None,
      RC_InvalidOpen,
      RC_InvalidHigh,
      RC_InvalidLow,
      RC_InvalidClose,
      RC_InvalidVolume,
      RC_InvalidOI,
      RC_OGTH, // open > high
      RC_OLTL, // open < low
      RC_CGTH, // close > high
      RC_CLTL, // close < low
      RC_VLT0, // volume < 0
      RC_LGTH, // low > high
      RC_HLTL, // high < low
      RC_OILT0 // oi < 0
    };

    enum BarLength
    {
      Minute1,
      Minute5,
      Minute10,
      Minute15,
      Minute30,
      Minute60,
      DailyBar,
      WeeklyBar,
      MonthlyBar
    };

    Bar ();
    int setDate (QDateTime &);
    void setDateRange (QDateTime &, Bar::BarLength);
    QDateTime & getDate ();
    void setOpen (double);
    void setOpen (QString &);
    double getOpen ();
    void setHigh (double);
    void setHigh (QString &);
    double getHigh ();
    void setLow (double);
    void setLow (QString &);
    double getLow ();
    void setClose (double);
    void setClose (QString &);
    double getClose ();
    void setVolume (double);
    void setVolume (QString &);
    double getVolume ();
    void setOI (double);
    void setOI (QString &);
    double getOI ();
    void getDateString (QString &);
    void getDateTimeString (QString &);
    void getTimeString (QString &);
    void verify ();
    int getError ();
    void getRangeKey (QString &);
    void getBarLengthList (QStringList &);
    int count ();

  protected:
    QHash<int, double> data;
    QDateTime startDate;
    QDateTime endDate;
    QDateTime displayDate;
    int error;
    int rangeFlag;
    BarLength length;
};

#endif
