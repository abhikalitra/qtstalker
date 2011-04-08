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

// *************************************************************************************************
// *************************************************************************************************


#ifndef DATA_DATA_BASE_HPP
#define DATA_DATA_BASE_HPP

#include <QtSql>
#include <QStringList>
#include <QList>

#include "Setting.h"

class DataDataBase
{
  public:
    DataDataBase (QString);
    void init ();
    int save (QString name, Setting *);
    int save (QString name, QString key, QString data);
    int save (QString name, QStringList &data);
    int load (QString name, Setting *);
    int load (QString name, QString key, QString &data);
    int load (QString name, QStringList &data);
    int names (QStringList &);
    int removeName (QString);
    int removeKey (QString, QString);
    int search (QString key, QString data, QStringList &);
    int dump (QString);
    int import (QString);
    void transaction ();
    void commit ();

  private:
    QSqlDatabase _db;
    QString _table;
};

#endif
