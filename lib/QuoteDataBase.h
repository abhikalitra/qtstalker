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

#ifndef QUOTEDATABASE_HPP
#define QUOTEDATABASE_HPP

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QHash>
#include <QList>

#include "BarData.h"
#include "Bar.h"


class QuoteDataBase
{
  public:
    QuoteDataBase ();
    void init (QString &dbFile);
    void transaction ();
    void commit ();
    void getAllChartsList (QStringList &);
    void getSearchList (QString &pat, QStringList &l);
    void getChart (BarData *);
    void getFirstDate (QString &table, QDateTime &date);
    void getLastDate (QString &table, QDateTime &date);
    void setStartEndDates (QDateTime &date, QDateTime &startDate, QDateTime &endDate,
			   BarData::BarLength barLength);
    int getTableName (QString &symbol, QString &table);
    void setChart (QList<Bar> *bars);
};

#endif
