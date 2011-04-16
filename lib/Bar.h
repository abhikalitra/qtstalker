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

// ******************************************************
// This class is used for holding 1 bar or quote of data.
// ******************************************************

#ifndef BAR_HPP
#define BAR_HPP

#include <QString>
#include <QDateTime>

#include "BarLength.h"

class Bar
{
  public:
    Bar ();
    void setDateRange (QDateTime, BarLength::Length);
    int setDates (QString start, QString end);
    int setDates (QDateTime, QDateTime);
    void dateString (QString &);
    void dateTimeString (QString &);
    void timeString (QString &);
    void rangeKey (QString &);
    QDateTime & date ();
    QString string ();
    QDateTime startDate ();
    void setLastDate (QDateTime);
    QDateTime lastDate ();
    int length ();
    void setLength (int);

    int setOpen (QString);
    void setOpen (double);
    double open ();
    
    int setHigh (QString);
    void setHigh (double);
    double high ();
    
    int setLow (QString);
    void setLow (double);
    double low ();
    
    int setClose (QString);
    void setClose (double);
    double close ();
    
    int setVolume (QString);
    void setVolume (double);
    double volume ();

    int setOI (QString);
    void setOI (double);
    double oi ();

    void copy (Bar *);

  protected:
    double _open;
    double _high;
    double _low;
    double _close;
    double _volume;
    double _oi;
    QDateTime _startDate;
    QDateTime _endDate;
    int _length;
    QDateTime _lastDate;
};

#endif
