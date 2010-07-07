/*
 *  QuoteServer
 *
 *  Copyright (C) 2010 Stefan S. Stratigakos
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
#include <QtSql>

DataBase::DataBase ()
{
  _dbName = "quotes";
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
    return 1;
  }

  return 0;
}

