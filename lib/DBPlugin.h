/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#ifndef DB_PLUGIN_HPP
#define DB_PLUGIN_HPP

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QHash>

#include "BarData.h"
#include "DataBase.h"
#include "Indicator.h"

class DBPlugin : public DataBase
{
  public:
    DBPlugin ();
    virtual ~DBPlugin ();
    virtual void getBars (BarData &);
    virtual void dialog ();
    virtual int scriptCommand (QStringList &, Indicator &, QByteArray &);
    virtual int deleteSymbol (BarData *);
    virtual int setDetail (QString &key, BarData *bd, QString &data);
    virtual int setDetail (int key, BarData *bd, QString &data);
    virtual int detail (QString &key, BarData *bd, QString &data);
    virtual int detail (int key, BarData *bd, QString &data);
    
    void getFirstDate (QString &table, QDateTime &date);
    void getLastDate (QString &table, QDateTime &date);
    void barErrorMessage (int);
    
  protected:
    QString plugin;
    QHash<QString, BarData *> quotes;
};

#endif
