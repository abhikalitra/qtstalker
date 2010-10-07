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

#include "TesterConfig.h"

#include <QtDebug>

TesterConfig::TesterConfig ()
{
  _dbName = "data";
  _tableName = "TesterPluginConfig";

  QSqlDatabase db = QSqlDatabase::database(_dbName);
  QSqlQuery q(db);
  QString s = "CREATE TABLE IF NOT EXISTS " + _tableName + " (";
  s.append("key INT PRIMARY KEY UNIQUE");
  s.append(", setting TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "TesterConfig::TesterConfig: " << q.lastError().text();
}
