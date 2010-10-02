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
#include "Globals.h"
#include "qtstalker_defines.h"

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QFileInfo>

ExchangeDataBase::ExchangeDataBase ()
{
  _dbName = "data";
}

int ExchangeDataBase::verifyExchangeName (QString &exchange)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
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
  QString inputFile = QString("%1/qtstalker/db/exchanges.csv").arg(INSTALL_DATA_DIR);
  QFile file(inputFile);
  if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
    return 1;

  // check if last modification date matches current one
  // if they match then no changes, no need to re-create the table
  // if not, new data is in file so re-create the table
  QDateTime dt;
  Config config;
  config.getData(Config::ExchangeFileDate, dt);

  QFileInfo fi(file);
  QDateTime dt2 = fi.lastModified();

  if (dt == dt2)
  {
    file.close();
    return 0;
  }
  
  config.setData(Config::ExchangeFileDate, dt2);
  qDebug() << "ExchangeDataBase::createExchanges: creating new exchange db";

  QTextStream in(&file);
  in.readLine(); // skip past first line
 
  // delete the old table
  QSqlQuery q(QSqlDatabase::database(_dbName));
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
  
  file.close();
  return 0;
}

void ExchangeDataBase::getExchanges (QStringList &l)
{
  l.clear();
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT code FROM exchangeIndex ORDER BY code ASC";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ExchangeDataBase::getExchanges: " << q.lastError().text();

  while (q.next())
    l.append(q.value(0).toString());
}

void ExchangeDataBase::getFieldList (QString &field, QStringList &rl)
{
  rl.clear();

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT DISTINCT " + field + " FROM exchangeIndex ORDER BY " + field + " ASC";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ExchangeDataBase::getFieldList: " << q.lastError().text();

  while (q.next())
    rl.append(q.value(0).toString());
}

int ExchangeDataBase::search (QString &country, QString &city, QString &pat, QStringList &rl)
{
  rl.clear();
  
  QString s;
  int whereFlag = FALSE;
  
  if (pat.isEmpty())
    s = "SELECT name FROM exchangeIndex";
  else
  {
    whereFlag = TRUE;
    s = "SELECT name FROM exchangeIndex WHERE name LIKE '" + pat + "'";
  }

  if (! country.isEmpty())
  {
    if (whereFlag)
      s.append(" AND countryName='" + country + "'");
    else
    {
      s.append(" WHERE countryName='" + country + "'");
      whereFlag = TRUE;
    }
  }
  
  if (! city.isEmpty())
  {
    if (whereFlag)
      s.append(" AND city='" + city + "'");
    else
    {
      s.append(" WHERE city='" + city + "'");
      whereFlag = TRUE;
    }
  }

  s.append(" ORDER BY name ASC");

  QSqlQuery q(QSqlDatabase::database(_dbName));
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ExchangeDataBase::search: " << q.lastError().text();
    return 1;
  }

  while (q.next())
    rl.append(q.value(0).toString());

  return 0;
}


void ExchangeDataBase::codeFromName (QString &name, QString &code)
{
  code.clear();

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT code FROM exchangeIndex WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ExchangeDataBase::codeFromName: " << q.lastError().text();

  if(q.next())
    code = q.value(0).toString();
}

