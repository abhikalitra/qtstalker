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

#include "DataBase.h"

#include <QDebug>
#include <QDir>

DataBase::DataBase ()
{
}

void DataBase::transaction ()
{
  QSqlDatabase db = QSqlDatabase::database(_dbName);
  db.transaction();
}

void DataBase::commit ()
{
  QSqlDatabase db = QSqlDatabase::database(_dbName);
  db.commit();
}

int DataBase::command (QString &sql, QString errMess)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
  q.exec(sql);
  if (q.lastError().isValid())
  {
    qDebug() << errMess << q.lastError().text();
    qDebug() << sql;
    return 1;
  }

  return 0;
}

void DataBase::create (QString session)
{
  QString basePath = QDir::homePath() + "/.qtstalker/";

  // create the config database
  QString s = basePath + "config.sqlite" + session;
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "config");
  db.setHostName("me");
  db.setDatabaseName(s);
  db.setUserName("QtStalker");
  db.setPassword("QtStalker");
  if (! db.open())
    qDebug() << "DataBase::create: config" << db.lastError().text();

  // create the data database
  s = basePath + "data.sqlite";
  QSqlDatabase db2 = QSqlDatabase::addDatabase("QSQLITE", "data");
  db2.setHostName("localhost");
  db2.setDatabaseName(s);
  db2.setUserName("QtStalker");
  db2.setPassword("QtStalker");
  if (! db2.open())
    qDebug() << "DataBase::create: data" << db2.lastError().text();
}

