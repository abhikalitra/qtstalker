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

#ifndef DBPLUGIN_HPP
#define DBPLUGIN_HPP

#include <qstring.h>
#include <qptrlist.h>
#include <qstringlist.h>
#include <db.h>
#include "Setting.h"
#include "BarData.h"
#include "Bar.h"
#include "BarDate.h"


class DbPlugin
{
  public:
  
    enum HeaderField
    {
      BarType,
      Plugin,
      Symbol,
      Type,
      Title,
      Path,
      CO,
      LocalIndicators,
      QuotePlugin
    };
  
    DbPlugin ();
    virtual ~DbPlugin ();
    int openChart (QString &);
    void setBarCompression (BarData::BarCompression);
    void setBarRange (int);
    void getHelpFile (QString &);
    void getChartObjectsList (QStringList &);    
    void getChartObjects (QStringList &);
    void setChartObject (QString &, Setting &);
    void deleteChartObject (QString &);
    void dump (QString &, bool);
    Bar * getLastBar ();
    Bar * getFirstBar ();
    void getData (QString &, QString &);
    void setData (QString &, QString &);
    void deleteData (QString &);
    Bar * getBar (QString &);
    void getDailyHistory ();
    void getWeeklyHistory ();
    void getMonthlyHistory ();
    void getTickHistory (int);
    void getDailyTickHistory ();
    void setHeaderField (int, QString &);
    void getHeaderField (int, QString &);
    void close ();
    void deleteIndicator (QString &);
    void addIndicator (QString &);
    void setIndicator (QString &, QString &);
    
    virtual void dbPrefDialog ();
    virtual void createNew ();
    virtual void getHistory (BarData *);
    virtual void setBar (Bar &);
    virtual Bar * getBar (QString &, QString &);
    
  protected:
    DB *db;
    int barRange;
    BarData::BarCompression barCompression;
    QString helpFile;
    BarData *barData;
};

#endif
