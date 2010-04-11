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

#include "ExchangeDataBase.h"
#include "Config.h"

#include <QDebug>
#include <QtSql>
#include <QFile>
#include <QTextStream>
#include <QStringList>

ExchangeDataBase::ExchangeDataBase ()
{
  dbName = "quotes";
}

int ExchangeDataBase::verifyExchangeName (QString &exchange)
{
  QSqlQuery q(QSqlDatabase::database(dbName));
  QString s = "SELECT code FROM exchangeIndex WHERE code='" + exchange + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ExchangeDataBase::verifyExchangeName: " << q.lastError().text();

  int rc = 1;
  if (q.next())
    rc = 0;
  
  return rc;
}

int ExchangeDataBase::createExchanges ()
{
  QFile file("/usr/local/share/qtstalker/db/exchanges.csv");
  if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
    return 1;

  // check if last file size and current file size match
  // if they match then no changes, no need to re-create the table
  // if not, new data is in file so re-create the table
  Config config;
  qint64 oldSize = (qint64) config.getInt(Config::ExchangeFileSize);
  qint64 size = file.size();
  if (size == oldSize)
  {
    file.close();
    return 0;
  }
  
  config.setData(Config::ExchangeFileSize, size);
  qDebug() << "ExchangeDataBase::createExchanges: creating new exchange db";

  QTextStream in(&file);
  in.readLine(); // skip past first line
 
  transaction();
  
  // delete the old table
  QSqlQuery q(QSqlDatabase::database(dbName));
  QString s = "DROP TABLE exchangeIndex";
  q.exec(s);

  // create the new table
  s = "CREATE TABLE IF NOT EXISTS exchangeIndex (";
  s.append("code TEXT PRIMARY KEY UNIQUE");
  s.append(", countryName TEXT");
  s.append(", countryCode2 TEXT");
  s.append(", name TEXT");
  s.append(", city TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ExchangeDataBase::createExchanges: create exchangeIndex table" << q.lastError().text();

  // add records to the table
  while (! in.atEnd())
  {
    s = in.readLine();
    QStringList l = s.split(",");
   
    s = "REPLACE INTO exchangeIndex (code,countryName,countryCode2,name,city) VALUES(";
    s.append("'" + l[0] + "'");
    s.append(",'" + l[1] + "'");
    s.append(",'" + l[2] + "'");
    s.append(",'" + l[3] + "'");
    s.append(",'" + l[4] + "'");
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
      qDebug() << "ExchangeDataBase::createExchanges: add records" << q.lastError().text();
  }
  
  commit();
  file.close();
  return 0;
}


