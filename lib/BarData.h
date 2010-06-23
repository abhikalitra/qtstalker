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

#ifndef BAR_DATA_HPP
#define BAR_DATA_HPP

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
      OI,
      AveragePrice,
      MedianPrice,
      TypicalPrice,
      WeightedClosePrice
    };

    BarData ();
    ~BarData ();
    int count ();
    void prepend (Bar *bar);
    void append (Bar *bar);
    double getMax ();
    double getMin ();
    void getInputFields (QStringList &);
    PlotLine * getInput (BarData::InputType, int, QColor &);
    PlotLine * getInput (BarData::InputType);
    BarData::InputType getInputType (QString &);
    Bar *getBar (int);
    void setMinMax ();
    void setBarLength (Bar::BarLength);
    void setBarLength (QString &);
    Bar::BarLength getBarLength ();
    QString & getSymbol ();
    void setSymbol (QString &);
    QString & getName ();
    void setName (QString &);
    double getAvgPrice (int);
    double getMedianPrice (int);
    double getTypicalPrice (int);
    QString & getTableName ();
    void setTableName (QString &);
    QString & getPlugin ();
    void setPlugin (QString &);
    QString & getExchange ();
    void setExchange (QString &);
    void setDateRange (int);
    int dateRange ();
    void setDateRangeOverride (int);
    int dateRangeOverride ();
    void setStartDate (QDateTime &);
    QDateTime & startDate ();
    void setEndDate (QDateTime &);
    QDateTime & endDate ();

  protected:
    QList<Bar *> _barList;
    double _high;
    double _low;
    Bar::BarLength _length;
    QString _symbol;
    QString _name;
    QString _tableName;
    QString _plugin;
    QString _exchange;
    int _dateRange;
    int _dateRangeOverride;
    QDateTime _startDate;
    QDateTime _endDate;
};

#endif
