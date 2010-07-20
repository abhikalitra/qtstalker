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

#include "Config.h"

#include <QDir>
#include <QtSql>

Config::Config ()
{
//  version = "0.37";
}

void Config::init (QString session)
{
  QString s = QDir::homePath() + "/.qtstalker/config.sqlite" + session;
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", _dbName);
  db.setHostName("me");
  db.setDatabaseName(s);
  db.setUserName("QtStalker");
  db.setPassword("QtStalker");
  if (! db.open())
  {
    qDebug() << "Config::Config:" << db.lastError().text();
    return;
  }

  QSqlQuery q(db);
  s = "CREATE TABLE IF NOT EXISTS " + _tableName + " (";
  s.append("key TEXT PRIMARY KEY UNIQUE");
  s.append(", setting TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "Config::Config: " << q.lastError().text();
}

