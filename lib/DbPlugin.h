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
#include <stdio.h>
#include "Setting.h"
#include "BarData.h"
#include "Bar.h"
#include "BarDate.h"

#define SSSIZE 5
#define SSIZE 25
#define TITLESIZE 100
#define PATHSIZE 250
#define MSIZE 250
#define COSIZE 10240
#define LSIZE 1000

typedef struct
{
  double firstDate;
  double lastDate;
  int records;
  bool bool1;
  bool bool2;
  bool bool3;
  int int1;
  int int2;
  int int3;
  double double1;
  double double2;
  double double3;
  int barType;
  char plugin[SSIZE];
  char symbol[SSIZE];
  char type[SSIZE];
  char futuresType[SSSIZE];
  char futuresMonth[SSSIZE];
  char title[TITLESIZE];
  char path[PATHSIZE];
  char co[COSIZE];
  char svar1[SSIZE];
  char svar2[SSIZE];
  char svar3[SSIZE];
  char mvar1[MSIZE];
  char lvar1[LSIZE];
  
  char tspace[LSIZE]; // this is extra space in case we need it later

} ChartHeader;


class DbPlugin
{
  public:
    DbPlugin ();
    virtual ~DbPlugin ();
    void setBarCompression (BarData::BarCompression);
    void setBarRange (int);
    void setDb (FILE *, ChartHeader *);
    void setDbPath (QString);
    QString getPluginName ();
    QString getHelpFile ();
    void close ();
    QStringList getChartObjectsList ();    
    QPtrList<Setting> getChartObjects ();
    void setChartObject (QString, Setting *);
    void deleteChartObject (QString);
    void dumpHeader (QTextStream &);
    bool findRecord (QString);
    Bar * getBar (QString);
    Bar * getLastBar ();
    void setBar (Bar *);
    
    void setSymbol (QString);
    void setTitle (QString);
    void setType (QString);
    void setFuturesType (QString);
    void setHeaderCO (QString);
    void setFuturesMonth (QString);
    void setHeaderFundamental (QString);
    
    void getDailyHistory ();
    void getWeeklyHistory ();
    void getMonthlyHistory ();
    void getTickHistory (int);
    void getDailyTickHistory ();
    
    void setFirstRecord (Bar *);
    void setAppendRecord (Bar *);
    void setInsertRecord (Bar *);
    
    void setHeader (Setting *);
    
    virtual void dbPrefDialog ();
    virtual QString createNew ();
    virtual void saveDbDefaults (Setting *);
    virtual BarData * getHistory ();
    virtual void dump (QString, bool);
    virtual void deleteBar (QString);
    virtual int readRecord ();
    virtual bool getRecordState ();
    virtual void fillBar (Bar *);
    virtual double getRecordDate ();
    virtual int writeRecord ();
    virtual void fillRecord (Bar *);
    virtual void clearRecord ();
    virtual void setRecordDate (double);
    virtual int writeTempRecord ();
    virtual void setBarString (QString);
    
  protected:
    ChartHeader *header;
    FILE *db;
    FILE *tdb;
    int barRange;
    BarData::BarCompression barCompression;
    QString path;
    QString pluginName;
    QString helpFile;
    bool saveFlag;
    int recordSize;
    BarData *barData;
};

#endif
