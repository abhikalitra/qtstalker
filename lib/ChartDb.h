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
#include <qlist.h>
#include <qstringlist.h>
#include <qdatetime.h>
#include <db.h>
#include "Setting.h"
#include "BarData.h"

#define DATE_FORMAT "yyyyMMddhhmmss"

class ChartDb
{
  public:

    enum Compression
    {
      Daily,
      Weekly,
      Monthly
    };

    ChartDb ();
    ~ChartDb ();
    int openChart (QString);
    QString getData (QString);
    void setData (QString, QString);
    void deleteData (QString);
    BarData * getDailyHistory ();
    BarData * getWeeklyHistory ();
    BarData * getMonthlyHistory ();
    QDateTime getDateTime (QString);
    Setting * getRecord (QString, QString);
    void deleteRecord (Setting *);
    void setRecord (Setting *);
    BarData * getHistory (Compression, QDateTime, BarData::BarType);
    void loadDetails ();
    Setting * getDetails ();
    void saveDetails ();
    void dump (QString);
    void setFormat ();

    QStringList getChartObjects ();
    Setting * getChartObject (QString);
    void setChartObject (QString, Setting *);
    void deleteChartObject (QString);

  private:
    QString dataPath;
    DB *db;
    QDateTime startDate;
    Compression compression;
    Setting *details;
    QStringList format;
    BarData::BarType barType;
};

#endif
