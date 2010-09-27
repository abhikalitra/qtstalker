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

#include "FuturesDataBase.h"
#include "Config.h"
#include "Globals.h"

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QFileInfo>
#include <QDebug>

FuturesDataBase::FuturesDataBase ()
{
  _dbName = "data";
}

int FuturesDataBase::createFutures ()
{
  QFile file("/usr/local/share/qtstalker/db/futures.csv");
  if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
    return 1;

  // check if last modification date matches current one
  // if they match then no changes, no need to re-create the table
  // if not, new data is in file so re-create the table
  QDateTime dt;
  Config config;
  config.getData(Config::FuturesFileDate, dt);

  QFileInfo fi(file);
  QDateTime dt2 = fi.lastModified();

  if (dt == dt2)
  {
    file.close();
    return 0;
  }
  
  config.setData(Config::FuturesFileDate, dt2);
  qDebug() << "FuturesDataBase::createFutures: creating new futures db";

  QTextStream in(&file);
  in.readLine(); // skip past first line
 
  // delete the old table
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "DROP TABLE futuresIndex";
  q.exec(s);

  // create the new table
  s = "CREATE TABLE IF NOT EXISTS futuresIndex (";
  s.append("code TEXT PRIMARY KEY UNIQUE");
  s.append(", name TEXT");
  s.append(", exchange TEXT");
  s.append(", tick REAL");
  s.append(", tickValue REAL");
  s.append(", F INT");
  s.append(", G INT");
  s.append(", H INT");
  s.append(", J INT");
  s.append(", K INT");
  s.append(", M INT");
  s.append(", N INT");
  s.append(", Q INT");
  s.append(", U INT");
  s.append(", V INT");
  s.append(", X INT");
  s.append(", Z INT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "FuturesDataBase::createFutures: create futuresIndex table" << q.lastError().text();

  // add records to the table
  while (! in.atEnd())
  {
    s = in.readLine();
    QStringList l = s.split(",");
   
    int pos = 0;
    s = "REPLACE INTO futuresIndex (code,name,exchange,tick,tickValue,F,G,H,J,K,M,N,Q,U,V,X,Z) VALUES(";
    s.append("'" + l[pos++] + "'"); // code
    s.append(",'" + l[pos++] + "'"); // name
    s.append(",'" + l[pos++] + "'"); // exchange
    s.append("," + l[pos++]); // tick
    s.append("," + l[pos++]); // tickValue
    s.append("," + l[pos++]); // F
    s.append("," + l[pos++]); // G
    s.append("," + l[pos++]); // H
    s.append("," + l[pos++]); // J
    s.append("," + l[pos++]); // K
    s.append("," + l[pos++]); // M
    s.append("," + l[pos++]); // N
    s.append("," + l[pos++]); // Q
    s.append("," + l[pos++]); // U
    s.append("," + l[pos++]); // V
    s.append("," + l[pos++]); // X
    s.append("," + l[pos++]); // Z
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
      qDebug() << "FuturesDataBase::createFutures: add records" << q.lastError().text();
  }
  
  file.close();
  return 0;
}

void FuturesDataBase::getCodeList (QStringList &l)
{
  l.clear();
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT DISTINCT code FROM futuresIndex";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "FuturesDataBase::getCodeList: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());

  l.sort();
}

