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
#include "Plugin.h"
#include "Setting.h"
#include "BarData.h"
#include "Bar.h"
#include "BarDate.h"

class DbPlugin : public Plugin
{
  public:
    DbPlugin ();
    ~DbPlugin ();
    void setBarCompression (BarData::BarCompression);
    void setBarRange (int);
    Bar * getLastBar ();
    Bar * getFirstBar ();
    QStringList getChartObjectsList ();    
    QPtrList<Setting> * getChartObjects ();
    void setChartObject (QString, Setting *);
    void deleteChartObject (QString);
    QString getData (QString);
    void setData (QString, QString);
    BarData * getHistory ();
    void deleteData (QString);
    BarDate getPrevDate (BarDate);
    void setDb (DB *);
    void setDbPath (QString);
    
  protected:
    DB *db;
    int barRange;
    BarData::BarCompression barCompression;
    QString path;
};

#endif
