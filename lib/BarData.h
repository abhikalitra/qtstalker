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
// Holds all the Bar.h bars for a financial symbol
// also holds some basic info for exchange, symbol, name etc.
// ******************************************************

#ifndef BAR_DATA_HPP
#define BAR_DATA_HPP

#include <QList>
#include <QStringList>
#include <QMetaType>

#include "Bar.h"
#include "Curve.h"

class BarData
{
  public:
    BarData ();
    ~BarData ();
    void clear ();
    int count ();
    void prepend (Bar *bar);
    void append (Bar *bar);
    double max ();
    double min ();
    Bar * bar (int);
    void setMinMax ();
    void setBarLength (int);
    int barLength ();
    QString & symbol ();
    void setSymbol (QString);
    QString & name ();
    void setName (QString);
    QString & exchange ();
    void setExchange (QString);
    int setKey (QString);
    QString key ();
//    void parse (QString &);
//    void stringSettings (QString &);
//    int setStringSettings (QString &);
    int maxBars ();
    void setMaxBars (int);
    void setStartDate (QDateTime);
    QDateTime startDate ();
    QString startDateString ();
    void setEndDate (QDateTime);
    QDateTime endDate ();
    QString endDateString ();
    void setTable (QString);
    QString table ();
    void setType (QString);
    QString type ();
    int range ();
    void setRange (int);

  protected:
    QList<Bar *> _barList;
    double _high;
    double _low;
    int _length;
    QString _symbol;
    QString _name;
    QString _exchange;
    int _maxBars;
    int _range;
    QDateTime _startDate;
    QDateTime _endDate;
    QString _table;
    QString _type;
};

// this is for passing Indicator data between threads
Q_DECLARE_METATYPE(BarData)

#endif


