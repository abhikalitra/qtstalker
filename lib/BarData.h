/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#ifndef BARDATA_HPP
#define BARDATA_HPP

#include <QList>
#include <QHash>
#include <QStringList>
#include <QDateTime>
#include "Bar.h"
#include "PlotLine.h"

class BarData
{
  public:
    enum InputType
    {
      Open,
      High,
      Low,
      Close,
      Volume,
      OpenInterest,
      Day,
      Week,
      Month,
      DayOfWeek
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
    
    BarData (QString &);
    ~BarData ();
    int count ();
    void getDate (int, QDateTime &);
    double getOpen (int);
    double getHigh (int);
    double getLow (int);
    double getClose (int);
    double getVolume (int);
    double getOI (int);
    void prepend (Bar &bar);
    void prependRaw (Bar &bar);
    void appendRaw (Bar &bar);
    int getX (QDateTime &);
    double getMax ();
    double getMin ();
    void createDateList ();
    void getInputFields (QStringList &);
    PlotLine * getInput (BarData::InputType);
    BarData::InputType getInputType (QString &);
    void getBarLengthList (QStringList &);
    void getBar (int, Bar &);
    void setMinMax ();
    void clear();
    void setBarLength (BarData::BarLength);
    BarData::BarLength getBarLength ();
    void setStartEndDates (QDateTime &);
    void setBar (int, Bar &);
    void getSymbol (QString &);
    void setSymbol (QString &);
    void getName (QString &);
    void setName (QString &);
    void getType (QString &);
    void setType (QString &);
    int getBarsRequested ();
    void setBarsRequested (int);
    void getDateOffset (QDateTime &);

    
  protected:
    QList<Bar> barList;
    QHash<QString, int> dateList;
    double high;
    double low;
    BarData::BarLength barLength;
    QDateTime startDate;
    QDateTime endDate;
    Bar currentBar;
    QString symbol;
    QString name;
    QString type;
    int barsRequested;
};

#endif
