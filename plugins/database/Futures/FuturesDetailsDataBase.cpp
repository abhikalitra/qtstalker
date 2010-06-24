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

#include "FuturesDetailsDataBase.h"
#include "QuoteIndexDataBase.h"

#include <QtSql>
#include <QtDebug>

FuturesDetailsDataBase::FuturesDetailsDataBase ()
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "CREATE TABLE IF NOT EXISTS futuresDetails (";
  s.append("tableName TEXT PRIMARY KEY UNIQUE");
  s.append(", name TEXT");
  s.append(", code TEXT");
  s.append(", month TEXT");
  s.append(", year REAL");
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
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "FuturesDetailsDataBase::FuturesDetailsDataBase:" << q.lastError().text();

  _detailsList << "NAME" << "CODE" << "MONTH" << "YEAR";
}

int FuturesDetailsDataBase::setDetail (QString &key, BarData *bd, QString &data)
{
  return setDetail(_detailsList.indexOf(key), bd, data);
}

int FuturesDetailsDataBase::setDetail (int key, BarData *bd, QString &data)
{
  QuoteIndexDataBase idb;
  if (idb.getIndexData(bd))
  {
    QString table = bd->getTableName();
    if (table.isEmpty())
    {
      qDebug() << "FuturesDetailsDataBase::setDetail: symbol not found";
      return 1;
    }
  }

  QString field;
  int numFlag = 0;
  switch ((FuturesDetail) key)
  {
    case Name:
      field = "name";
      break;
    case Code:
      field = "code";
      break;
    case Month:
      field = "month";
      break;
    case Year:
      field = "year";
      numFlag = 1;
      break;
    default:
      return 1;
      break;
  }

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "INSERT OR REPLACE INTO futuresDetails (tableName," + field + ") VALUES(";
  s.append("'" + bd->getTableName() + "'");
  if (numFlag)
    s.append("," + data);
  else
    s.append(",'" + data + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "FuturesDetailsDataBase::setDetail:" << q.lastError().text();
    return 1;
  }

  return 0;
}

int FuturesDetailsDataBase::detail (QString &key, BarData *bd, QString &data)
{
  return detail(_detailsList.indexOf(key), bd, data);
}

int FuturesDetailsDataBase::detail (int key, BarData *bd, QString &data)
{
  data.clear();

  QuoteIndexDataBase idb;
  if (idb.getIndexData(bd))
  {
    QString table = bd->getTableName();
    if (table.isEmpty())
    {
      qDebug() << "FuturesDetailsDataBase::detail: symbol not found";
      return 1;
    }
  }

  QString field;
  switch ((FuturesDetail) key)
  {
    case Name:
      field = "name";
      break;
    case Code:
      field = "code";
      break;
    case Month:
      field = "month";
      break;
    case Year:
      field = "year";
      break;
    default:
      return 1;
      break;
  }

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT " + field + " FROM futuresDetails WHERE tableName='" + bd->getTableName() + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "FuturesDetailsDataBase::detail: " << q.lastError().text();
    return 1;
  }

  if (q.next())
    data = q.value(0).toString();
  else
    return 1;

  return 0;
}

int FuturesDetailsDataBase::remove (BarData *symbol)
{
  // remove parms record
  QString s = "DELETE FROM futuresDetails WHERE tableName='" + symbol->getTableName() + "'";
  if (command(s, QString("FuturesDetailsDataBase::remove")))
    return 1;

  return 0;
}

