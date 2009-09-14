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

#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QHash>

#include "BarData.h"
#include "Indicator.h"
#include "COSettings.h"



class DataBase
{
  public:
    DataBase ();

    // chart functions
    void getAllChartsList (QStringList &);
    void getSearchList (QString &pat, QStringList &l);
    void getChart (BarData *);
    void getFirstDate (QDateTime &date, QString &symbol);
    void getLastDate (QDateTime &date, QString &symbol);
    void setStartEndDates (QDateTime &startDate, QDateTime &endDate, BarData::BarLength barLength);
    void getDateOffset (QDateTime &dt, BarData::BarLength barLength);

    // group functions
    void getAllGroupsList (QStringList &);
    void getGroupList (QString &, QStringList &);
    void deleteGroup (QString &);
    void setGroupList (QString &, QStringList &);

    // indicator functions
    void getIndicator (Indicator &);
    void setIndicator (Indicator &);
    void deleteIndicator (QString &);
    void getIndicatorList (QStringList &);
    void getActiveIndicatorList (QStringList &);
    void getSearchIndicatorList (QString &pattern, QStringList &list);

    // scanner functions
//    void getScannerList (QStringList &);
//    void deleteScanner (QString &);
//    void getScanner (ScannerRule &);
//    void setScanner (ScannerRule &);

    // tester functions
//    void getTestList (QStringList &);
//    void deleteTest (QString &);
//    void getTest (TesterRule &);
//    void setTest (TesterRule &);

    // chart object functions
    void deleteChartObjects (QString &symbol);
    void deleteChartObjectsIndicator (QString &indicator);
    void deleteChartObject (QString &id);
    void getChartObjects (QString &symbol, QString &indicator, QHash<QString, COSettings *> &list);
    void setChartObject (COSettings *);
};

#endif
