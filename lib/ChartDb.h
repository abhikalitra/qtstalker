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
#include <db.h>
#include "Setting.h"
#include "BarData.h"
#include "Bar.h"
#include "BarDate.h"

class ChartDb
{
  public:
    enum BarCompression
    {
      Minute5,
      Minute15,
      Minute30,
      Minute60,
      Daily,
      Weekly,
      Monthly
    };

    enum Detail
    {
      Symbol,
      Title,
      Type,
      FuturesType,
      FuturesMonth,
      BarType
    };
      
    ChartDb ();
    ~ChartDb ();
    int openChart (QString);
    QString getData (QString);
    void setData (QString, QString);
    void deleteData (QString);
    Bar * getBar (QString, QString);
    void setBar (Bar *);
    BarData * getHistory ();
    void dump (QString);
    void setBarCompression (ChartDb::BarCompression);
    ChartDb::BarCompression getBarCompression ();
    void setBarRange (int);
    int getBarRange ();
    QStringList getBarCompressionList ();
    Bar * getLastBar ();
    Bar * getFirstBar ();
    BarDate getPrevDate (BarDate);
    QStringList getChartObjectsList ();    
    QPtrList<Setting> * getChartObjects ();
    void setChartObject (QString, Setting *);
    void deleteChartObject (QString);
    QString getDetail (Detail);
    void setDetail (Detail, QString);

  private:
    QString dataPath;
    DB *db;
    int barRange;
    BarCompression barCompression;
};

#endif
