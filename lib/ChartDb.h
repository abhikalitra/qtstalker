/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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

#ifndef CHARTDB_HPP
#define CHARTDB_HPP

#include <qstring.h>
#include <qptrlist.h>
#include <qstringlist.h>
#include "Plugin.h"
#include "Setting.h"
#include "BarData.h"
#include "Bar.h"
#include "BarDate.h"
#include "Config.h"

class ChartDb
{
  public:
    ChartDb ();
    ~ChartDb ();
    int openChart (QString);
    void setBarCompression (BarData::BarCompression);
    void setBarRange (int);
    void dump (QString);
    Bar * getLastBar ();
    Bar * getFirstBar ();
    Bar * getBar (QString, QString);
    QStringList getChartObjectsList ();    
    QPtrList<Setting> * getChartObjects ();
    void setChartObject (QString, Setting *);
    void deleteChartObject (QString);
    QString getData (QString);
    void setData (QString, QString);
    void setBar (BarDate, double, double, double, double, double, double);
    BarData * getHistory ();
    void deleteData (QString);
    void dbPrefDialog (QString);
    void createNew (QString);
    void setPlugin (QString);
    int open (QString);
    int loadPlugin ();
    void saveDbDefaults (BarData::BarType, QString, QString, QString,
                         QString, QString, QString);
    
  private:
    DB *db;
    Plugin *plug;
    QString dbPlugin;
    Config config;
    QString path;
};

#endif
