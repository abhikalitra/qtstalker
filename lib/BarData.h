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

    enum InputType
    {
      Open,
      High,
      Low,
      Close,
      Volume,
      OI,
      AveragePrice,
      MedianPrice,
      TypicalPrice,
      WeightedClosePrice
    };

    BarData ();
    ~BarData ();
    void clear ();
    int count ();
    void prepend (Bar *bar);
    void append (Bar *bar);
    double max ();
    double min ();
    void inputFields (QStringList &);
    Curve * input (BarData::InputType);
    BarData::InputType inputType (QString);
    Bar * bar (int);
    void setMinMax ();
    void setBarLength (BarLength);
    void setBarLength (QString);
    BarData::BarLength & barLength ();
    QString & symbol ();
    void setSymbol (QString);
    QString & name ();
    void setName (QString);
    double avgPrice (int);
    double medianPrice (int);
    double typicalPrice (int);
    QString & exchange ();
    void setExchange (QString);
    void barLengthList (QStringList &);
    void barLengthText (BarData::BarLength, QString &);
    int setKey (QString);
    QString key ();
    void parse (QString &);
    void stringSettings (QString &);
    int setStringSettings (QString &);
    int range ();
    void setRange (int);

  protected:
    QList<Bar *> _barList;
    double _high;
    double _low;
    BarLength _length;
    QString _symbol;
    QString _name;
    QString _exchange;
    int _range;
};

// this is for passing Indicator data between threads
Q_DECLARE_METATYPE(BarData)

#endif


