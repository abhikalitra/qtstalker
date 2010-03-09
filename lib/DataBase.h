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
#include "Script.h"
#include "ChartObject.h"


class DataBase
{
  public:
    DataBase ();
    void init (); // called only at qtstalker startup, initializes database tables
    void transaction ();
    void commit ();

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
    void setIndicatorEnable (Indicator &);

    // chart object functions
    void deleteChartObjects (QString &symbol);
    void deleteChartObjectsIndicator (QString &indicator);
    void deleteChartObject (QString &id);
    void getChartObjects (QString &symbol, QString &indicator, Indicator &);
    void setChartObject (ChartObject *);

    // script functions
    void getScripts (QStringList &);
    void getScript (Script &);
    void deleteScript (Script &);
    void setScript (Script &);
    void getScriptSearch (QString &pattern, QStringList &list);
};

#endif
