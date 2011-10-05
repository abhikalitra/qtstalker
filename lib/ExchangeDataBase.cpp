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

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QFileInfo>

ExchangeDataBase::ExchangeDataBase ()
{
  init();
}

void ExchangeDataBase::init ()
{
  _db = QSqlDatabase::database("data");
  if (! _db.isOpen())
  {
    QString s = QDir::homePath() + "/.qtstalker/data.sqlite";
    _db = QSqlDatabase::addDatabase("QSQLITE", "data");
    _db.setHostName("QtStalker");
    _db.setDatabaseName(s);
    _db.setUserName("QtStalker");
    _db.setPassword("QtStalker");
    if (! _db.open())
      qDebug() << "ExchangeDataBase::init:" << _db.lastError().text();
  }

  createTable();
}

int ExchangeDataBase::createExchanges ()
{
  // check if table is empty and fill it in
  QStringList l;
  getExchanges(l);
  if (! l.count())
    return create();

  // check if last modification date matches current one
  // if they match then no changes, no need to re-create the table
  // if not, new data is in file so re-create the table
  QSettings settings("QtStalker/qtstalkerrc");
  QDateTime dt = settings.value("exchange_database_file_date", QDateTime::currentDateTime()).toDateTime();
  if (! dt.isValid())
  {
    qDebug() << "ExchangeDataBase::createExchanges: invalid date" << dt;
    return 1;
  }

  QString inputFile = settings.value("db_data_directory").toString() + "exchanges.csv";
  QFileInfo fi(inputFile);
  QDateTime dt2 = fi.lastModified();

  if (dt == dt2)
    return 0;

  settings.setValue("exchange_database_file_date", dt2);
  settings.sync();

  return create();
}

int ExchangeDataBase::create ()
{
  QSettings settings("QtStalker/qtstalkerrc");

  QString inputFile = settings.value("db_data_directory").toString() + "exchanges.csv";

  QFile file(inputFile);
  if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
    return 1;

  QTextStream in(&file);
  in.readLine(); // skip past first line

  _db.transaction();

  if (deleteTable())
    return 1;

  if (createTable())
    return 1;

  qDebug() << "ExchangeDataBase::createExchanges: updating exchange db";

  // add records to the table
  QSqlQuery q(_db);
  while (! in.atEnd())
  {
    QString s = in.readLine();
    QStringList l = s.split(",", QString::SkipEmptyParts);
    int pos = 0;

    s = "REPLACE INTO exchangeIndex (code,countryName,countryCode2,name,city) VALUES(";
    s.append("'" + l.at(pos++) + "'");
    s.append(",'" + l.at(pos++) + "'");
    s.append(",'" + l.at(pos++) + "'");
    s.append(",'" + l.at(pos++) + "'");
    s.append(",'" + l.at(pos++) + "'");
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
      qDebug() << "ExchangeDataBase::createExchanges: add records" << q.lastError().text();
  }

  _db.commit();

  return 0;
}

int ExchangeDataBase::createTable ()
{
  // create the new table
  QSqlQuery q(_db);
  QString s = "CREATE TABLE IF NOT EXISTS exchangeIndex (";
  s.append("code TEXT PRIMARY KEY UNIQUE");
  s.append(", countryName TEXT");
  s.append(", countryCode2 TEXT");
  s.append(", name TEXT");
  s.append(", city TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ExchangeDataBase::createExchanges: create exchangeIndex table" << q.lastError().text();
    return 1;
  }

  return 0;
}

int ExchangeDataBase::deleteTable ()
{
  // delete the old table
  QSqlQuery q(_db);
  QString s = "DROP TABLE exchangeIndex";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ExchangeDataBase::deleteTable" << q.lastError().text();
    return 1;
  }

  return 0;
}

int ExchangeDataBase::verifyExchangeName (QString &exchange)
{
  QSqlQuery q(_db);
  QString s = "SELECT code FROM exchangeIndex WHERE code='" + exchange + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ExchangeDataBase::verifyExchangeName: " << q.lastError().text();

  int rc = 1;
  if (q.next())
    rc = 0;

  return rc;
}

void ExchangeDataBase::getExchanges (QStringList &l)
{
  l.clear();

  QSqlQuery q(_db);
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

  QSqlQuery q(_db);
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

  QSqlQuery q(_db);
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

  QSqlQuery q(_db);
  QString s = "SELECT code FROM exchangeIndex WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ExchangeDataBase::codeFromName: " << q.lastError().text();

  if(q.next())
    code = q.value(0).toString();
}
