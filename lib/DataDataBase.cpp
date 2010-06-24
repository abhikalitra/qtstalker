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

#include "DataDataBase.h"

#include <QDir>
#include <QtSql>
#include <QDebug>

DataDataBase::DataDataBase ()
{
  _dbName = "data";
  
  QString dbFile = QDir::homePath() + "/.qtstalker/data.sqlite";

  QSqlDatabase db = QSqlDatabase::database(_dbName, FALSE);
  if (! db.isOpen())
  {
    db = QSqlDatabase::addDatabase("QSQLITE", _dbName);
    db.setHostName("localhost");
    db.setDatabaseName(dbFile);
    db.setUserName("QtStalker");
    db.setPassword("QtStalker");
    if (! db.open())
      qDebug() << "DataDataBase:: data db open failed";
  }
}

