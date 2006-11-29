/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2006 Stefan S. Stratigakos
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
#include <qstringlist.h>
#include <db.h>
#include <qdatetime.h>
#include <qobject.h>
#include "Setting.h"
#include "BarData.h"
#include "Bar.h"

class DbPlugin : public QObject
{
  Q_OBJECT
  
  signals:
    void signalProgMessage (int, int);
    void signalStatusMessage (QString);

  public:
  
    enum DbType
    {
      Stock,
      Futures,
      Spread1,
      Index1,
      CC1
    };

    enum HeaderField
    {
      BarType,
      Fundamentals,
      Symbol,
      Type,
      Title,
      Path,
      CO,
      LocalIndicators,
      QuotePlugin,
      SpreadFirstSymbol,
      SpreadSecondSymbol,
      IndexList,
      CCAdjustment,
      FuturesType,
      FuturesMonth
    };
  
    DbPlugin ();
    virtual ~DbPlugin ();
    int openChart (QString &);
    void setBarLength (BarData::BarLength);
    void setBarRange (int);
    void getHelpFile (QString &);
    void getChartObjectsList (QStringList &);    
    void getChartObjects (QStringList &);
    void setChartObject (QString &, Setting &);
    void deleteChartObject (QString &);
    void dump (QString &, bool);
    void getLastBar (Bar &);
    void getFirstBar (Bar &);
    void getData (QString &, QString &);
    void setData (QString &, QString &);
    void deleteData (QString &);
    void getBar (QString &, Bar &);
    void setHeaderField (int, QString &);
    void getHeaderField (int, QString &);
    void close ();
    void deleteIndicator (QString &); // local indicators
    void addIndicator (QString &); // local indicators
    void getAllBars (BarData *);
    void loadType ();
    void setBar (Bar &);
    void getBar (QString &, QString &, Bar &);
    void getHistory (BarData *, QDateTime &);
    void createNew (DbPlugin::DbType);
    void createNew (QString &);
    void dbPrefDialog ();
    void getHeaderKey (int, QString &);
    DbPlugin::DbType getType (QString &);
    void getSymbol (QString &);
    void getPrevBar (QDateTime &, Bar &);
    void getNextBar (QDateTime &, Bar &);
    void getSearchBar (QDateTime &, Bar &);

// remove this later
    
  protected:
    DB *db;
    int barRange;
    BarData::BarLength barLength;
    QString helpFile;
    DbPlugin::DbType type;
    QString symbol;
};

#endif


