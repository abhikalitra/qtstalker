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
#include <QList>
#include "BarData.h"
#include "IndicatorParms.h"
#include "IndicatorIndex.h"
#include "Bar.h"
#include "COSettings.h"



class DataBase
{
  public:
    DataBase ();

    // chart functions
    void getAllChartsList (QStringList &);
    void getChart (BarData *);

    // group functions
    int createGroup (QString &);
    void getAllGroupsList (QStringList &);
    void getGroupList (QString &, QStringList &);
    void deleteGroup (QString &);
    void setGroupList (QString &, QStringList &, bool flag);

    // indicator functions
    int createIndicator (QString &);
    void getIndicator (QString &, QList<IndicatorParms> &);
    void setIndicator (QString &, QList<IndicatorParms> &);
    void deleteIndicator (QString &);
    void getIndicatorList (QStringList &);
    void setIndicatorIndex (QString &, IndicatorIndex &);
    void getIndicatorIndex (QString &, IndicatorIndex &);

    // scanner functions
    void getScannerList (QStringList &);
    void deleteScanner (QString &);

    // tester functions
    void getTestList (QStringList &);
    void deleteTest (QString &);

    // chart object functions
    void deleteChartObjects (QString &symbol);
    void deleteChartObjectsIndicator (QString &indicator);
    void deleteChartObject (QString &id);
    void getChartObjects (QString &symbol, QString &indicator, QList<COSettings> &list);
    void setChartObject (COSettings &);

    
  protected:
};

#endif
