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

#ifndef SPREAD_HPP
#define SPREAD_HPP

#include "DbPlugin.h"
#include "Bar.h"
#include "Setting.h"
#include <qstring.h>
#include <qdict.h>

typedef struct
{
  bool state;
  double date;
  float open;
  float high;
  float low;
  float close;

} SpreadRecord;


class Spread : public DbPlugin
{
  public:
    Spread ();
    ~Spread ();
    BarData * getHistory ();
    void updateSpread ();
    void loadData (QString, QString);
    QString createNew ();
    void dbPrefDialog ();
    void saveDbDefaults (Setting *);
    void dump (QString, bool);
    void deleteBar (QString);
    int readRecord ();
    int writeRecord ();
    bool getRecordState ();
    void fillBar (Bar *);
    double getRecordDate ();
    void fillRecord (Bar *);
    void clearRecord ();
    void setRecordDate (double);
    int writeTempRecord ();
    void setBarString (QString);
    
  private:
    QDict<Bar> data;
    SpreadRecord record;
    double fdate;
};

extern "C"
{
  DbPlugin * createDbPlugin ();
}

#endif
