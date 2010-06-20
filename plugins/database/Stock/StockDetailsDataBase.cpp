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

#include "StockDetailsDataBase.h"
#include "QuoteIndexDataBase.h"

#include <QDebug>
#include <QtSql>

StockDetailsDataBase::StockDetailsDataBase ()
{
  _dbName = "quotes";

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "CREATE TABLE IF NOT EXISTS stockDetails (";
  s.append("tableName TEXT PRIMARY KEY UNIQUE");
  s.append(", name TEXT");
  s.append(", sector TEXT");
  s.append(", t1 TEXT"); // unused text
  s.append(", t2 TEXT"); // unused text
  s.append(", t3 TEXT"); // unused text
  s.append(", t4 TEXT"); // unused text
  s.append(", t5 TEXT"); // unused text
  s.append(", t6 TEXT"); // unused text
  s.append(", t7 TEXT"); // unused text
  s.append(", t8 TEXT"); // unused text
  s.append(", t9 TEXT"); // unused text
  s.append(", t10 TEXT"); // unused text
  s.append(", d1 REAL"); // unused decimal
  s.append(", d2 REAL"); // unused decimal
  s.append(", d3 REAL"); // unused decimal
  s.append(", d4 REAL"); // unused decimal
  s.append(", d5 REAL"); // unused decimal
  s.append(", d6 REAL"); // unused decimal
  s.append(", d7 REAL"); // unused decimal
  s.append(", d8 REAL"); // unused decimal
  s.append(", d9 REAL"); // unused decimal
  s.append(", d10 REAL"); // unused decimal
  s.append(", d11 REAL"); // unused decimal
  s.append(", d12 REAL"); // unused decimal
  s.append(", d13 REAL"); // unused decimal
  s.append(", d14 REAL"); // unused decimal
  s.append(", d15 REAL"); // unused decimal
  s.append(", d16 REAL"); // unused decimal
  s.append(", d17 REAL"); // unused decimal
  s.append(", d18 REAL"); // unused decimal
  s.append(", d19 REAL"); // unused decimal
  s.append(", d20 REAL"); // unused decimal
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "StockDetailsDataBase::init:" << q.lastError().text();

  _detailsList << "NAME" << "SECTOR";
}

int StockDetailsDataBase::setDetail (QString &key, BarData *bd, QString &data)
{
  return setDetail(_detailsList.indexOf(key), bd, data);
}

int StockDetailsDataBase::setDetail (int key, BarData *bd, QString &data)
{
  QuoteIndexDataBase idb;
  if (idb.getIndexData(bd))
  {
    QString table = bd->getTableName();
    if (table.isEmpty())
    {
      qDebug() << "StockDetailsDataBase::setDetail: symbol not found";
      return 1;
    }
  }

  QString field;
  int numFlag = 0;
  switch ((StockDetail) key)
  {
    case Name:
      field = "name";
      break;
    case Sector:
      field = "sector";
      break;
    default:
      return 1;
      break;
  }

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "INSERT OR REPLACE INTO stockDetails (tableName," + field + ") VALUES(";
  s.append("'" + bd->getTableName() + "'");
  if (numFlag)
    s.append("," + data);
  else
    s.append(",'" + data + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "StockDetailsDataBase::setDetail:" << q.lastError().text();
    return 1;
  }

  return 0;
}

int StockDetailsDataBase::detail (QString &key, BarData *bd, QString &data)
{
  return detail(_detailsList.indexOf(key), bd, data);
}

int StockDetailsDataBase::detail (int key, BarData *bd, QString &data)
{
  data.clear();
  
  QuoteIndexDataBase idb;
  if (idb.getIndexData(bd))
  {
    QString table = bd->getTableName();
    if (table.isEmpty())
    {
      qDebug() << "StockDetailsDataBase::detail: symbol not found";
      return 1;
    }
  }

  QString field;
  switch ((StockDetail) key)
  {
    case Name:
      field = "name";
      break;
    case Sector:
      field = "sector";
      break;
    default:
      return 1;
      break;
  }

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT " + field + " FROM stockDetails WHERE tableName='" + bd->getTableName() + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "StockDetailsDataBase::detail: " << q.lastError().text();
    return 1;
  }

  if (q.next())
    data = q.value(0).toString();
  else
    return 1;

  return 0;
}

int StockDetailsDataBase::remove (BarData *symbol)
{
  // remove parms record
  QString s = "DELETE FROM stockDetails WHERE tableName='" + symbol->getTableName() + "'";
  if (command(s, QString("StockDetailsDataBase::remove")))
    return 1;

  return 0;
}

