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

#include "DBPlugin.h"

#include <QDebug>
#include <QtSql>

DBPlugin::DBPlugin ()
{
  dbName = "quotes";
}

// virtual
DBPlugin::~DBPlugin ()
{
}

// virtual
void DBPlugin::getBars (BarData &)
{
}

// virtual
void DBPlugin::setBars (BarData &)
{
}

// virtual
void DBPlugin::dialog ()
{
}

// virtual
int DBPlugin::scriptCommand (QStringList &)
{
  return 0;
}

/*********************************************************/
/*********************************************************/
/*********************************************************/

void DBPlugin::init (QString &dbFile)
{
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", dbName);
  db.setHostName("localhost");
  db.setDatabaseName(dbFile);
  db.setUserName("QtStalker");
  db.setPassword("QtStalker");
  if (! db.open())
    qDebug() << "DBPlugin::init: quotes db open failed";

  QSqlQuery q(db);
  QString s = "CREATE TABLE IF NOT EXISTS symbolIndex (";
  s.append(" record INTEGER PRIMARY KEY AUTOINCREMENT");
  s.append(", symbol TEXT");
  s.append(", exchange TEXT");
  s.append(", tableName TEXT");
  s.append(", plugin TEXT");
  s.append(", name TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DBPlugin::init:" << q.lastError().text();

  s = "CREATE TABLE IF NOT EXISTS exchangeIndex (";
  s.append("name TEXT PRIMARY KEY UNIQUE");
  s.append(", country TEXT");
  s.append(", gmt REAL");
  s.append(", currency TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DBPlugin::init:" << q.lastError().text();
}

void DBPlugin::transaction ()
{
  QSqlDatabase db = QSqlDatabase::database(dbName);
  db.transaction();
}

void DBPlugin::commit ()
{
  QSqlDatabase db = QSqlDatabase::database(dbName);
  db.commit();
}

void DBPlugin::getSearchList (QString &ex, QString &pat, QList<BarData *> &l)
{
  // if exchange is empty then get all symbols from all exchanges that match pat
  QString s;
  if (ex.isEmpty() && ! pat.isEmpty())
    s = "SELECT symbol,name FROM symbolIndex WHERE symbol LIKE '" + pat + "'";

  // exchange = yes and pat = no, get the entire exchange list
  if (! ex.isEmpty() && pat.isEmpty())
    s = "SELECT symbol,name FROM symbolIndex WHERE exchange='" + ex + "'";
    
  // exchange = yes and pat = yes, get pat from the exchange
  if (ex.isEmpty() && pat.isEmpty())
  {
    s = "SELECT symbol,name FROM symbolIndex WHERE symbol LIKE";
    s.append(" '" + pat + "'");
    s.append(" AND exchange='" + ex + "'");
  }
  
  if (s.isEmpty())
    return;
  
  QSqlQuery q(QSqlDatabase::database(dbName));
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBPlugin::getSearchList: " << q.lastError().text();
    return;
  }

  while (q.next())
  {
    BarData *bd = new BarData;
    
    bd->setExchange(ex);
    
    s = q.value(0).toString();
    bd->setSymbol(s);
    
    s = q.value(1).toString();
    bd->setName(s);
    
    l.append(bd);
  }
}

void DBPlugin::getFirstDate (QString &table, QDateTime &date)
{
  QSqlQuery q(QSqlDatabase::database(dbName));
  QString s = "SELECT min(date) FROM " + table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBPlugin::getFirstDate:" << q.lastError().text();
    return;
  }

  if (q.next())
    date = q.value(0).toDateTime();
}

void DBPlugin::getLastDate (QString &table, QDateTime &date)
{
  QSqlQuery q(QSqlDatabase::database(dbName));
  QString s = "SELECT max(date) FROM " + table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBPlugin::getLastDate:" << q.lastError().text();
    return;
  }

  if (q.next())
    date = q.value(0).toDateTime();
}

void DBPlugin::setStartEndDates (QDateTime &date, QDateTime &startDate, QDateTime &endDate,
				      BarData::BarLength barLength)
{
  QString s, s2;
  int tint = 0;
  startDate = date;

  switch (barLength)
  {
    case BarData::Minute1:
      startDate.setTime(QTime(startDate.time().hour(), startDate.time().minute(), 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(60);
      break;
    case BarData::Minute5:
      tint = startDate.time().minute() / 5;
      startDate.setTime(QTime(startDate.time().hour(), tint * 5, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(300);
      break;
    case BarData::Minute10:
      tint = startDate.time().minute() / 10;
      startDate.setTime(QTime(startDate.time().hour(), tint * 10, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(600);
      break;
    case BarData::Minute15:
      tint = startDate.time().minute() / 15;
      startDate.setTime(QTime(startDate.time().hour(), tint * 15, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(900);
      break;
    case BarData::Minute30:
      tint = startDate.time().minute() / 30;
      startDate.setTime(QTime(startDate.time().hour(), tint * 30, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(1800);
      break;
    case BarData::Minute60:
      startDate.setTime(QTime(startDate.time().hour(), 0, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(3600);
      break;
    case BarData::DailyBar:
      startDate.setTime(QTime(0, 0, 0, 0));
      endDate = startDate;
      endDate = endDate.addDays(1);
      break;
    case BarData::WeeklyBar:
      startDate.setTime(QTime(0, 0, 0, 0));
      startDate = startDate.addDays(- startDate.date().dayOfWeek());
      endDate = startDate;
      endDate = endDate.addDays(7);
      break;
    case BarData::MonthlyBar:
      startDate.setTime(QTime(0, 0, 0, 0));
      startDate = startDate.addDays(- (startDate.date().day() - 1));
      endDate = startDate;
      endDate = endDate.addDays(endDate.date().daysInMonth());
      break;
    default:
      break;
  }
}

int DBPlugin::getIndexData (BarData &data)
{
  QSqlQuery q(QSqlDatabase::database(dbName));
  QString s = "SELECT name,tableName,plugin FROM symbolIndex";
  s.append(" WHERE symbol='" + data.getSymbol() + "'");
  s.append(" AND exchange='" + data.getExchange() + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBPlugin::getTableName: " << q.lastError().text();
    return 1;
  }

  if (q.next())
  {
    int pos = 0;
    s = q.value(pos++).toString();
    data.setName(s);

    s = q.value(pos++).toString();
    data.setTableName(s);
    
    s = q.value(pos++).toString();
    data.setPlugin(s);
  }

  return 0;
}

int DBPlugin::setIndexData (BarData &data)
{
  QSqlQuery q(QSqlDatabase::database(dbName));
  QString s = "UPDATE symbolIndex SET ";
  s.append("name='" + data.getName() + "'");
  s.append(" WHERE exchange='" + data.getExchange() + "'");
  s.append(" AND symbol='" + data.getSymbol() + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBPlugin::setIndexData:" << q.lastError().text();
    return 1;
  }

  return 0;
}

int DBPlugin::command (QString &sql, QString errMess)
{
  QSqlQuery q(QSqlDatabase::database(dbName));
  q.exec(sql);
  if (q.lastError().isValid())
  {
    qDebug() << errMess << q.lastError().text();
    return 1;
  }

  return 0;
}

int DBPlugin::addSymbolIndex (BarData &bars)
{
  QSqlQuery q(QSqlDatabase::database(dbName));

  // we use the maximum record column and add 1 to it to generate a new table name
  QString table;
  QString ts = "SELECT max(record) FROM symbolIndex";
  q.exec(ts);
  if (q.lastError().isValid())
  {
    qDebug() << "DBPlugin::addSymbolIndex: get record max" << q.lastError().text();
    return 1;
  }
  
  if (q.next())
    table = "T" + QString::number(q.value(0).toInt() + 1);
    
  // add new symbol entry into the symbolIndex table
  ts = "INSERT OR REPLACE INTO symbolIndex (symbol,tableName,exchange,plugin) VALUES(";
  ts.append("'" + bars.getSymbol() + "'");
  ts.append(",'" + table + "'");
  ts.append(",'" + bars.getExchange() + "'");
  ts.append(",'" + bars.getPlugin() + "'");
  ts.append(")");
  q.exec(ts);
  if (q.lastError().isValid())
  {
    qDebug() << "DBPlugin::addSymbolIndex: create new symbolIndex record" << q.lastError().text();
    return 1;
  }
  
  bars.setTableName(table);
  
  return 0;
}

void DBPlugin::getExchangeList (QStringList &l)
{
  QSqlQuery q(QSqlDatabase::database(dbName));
  QString s = "SELECT DISTINCT exchange FROM symbolIndex";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBPlugin::getExchangeList: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());

  l.sort();
}

int DBPlugin::verifyExchangeName (QString &exchange)
{
  QSqlQuery q(QSqlDatabase::database(dbName));
  QString s = "SELECT name FROM exchangeIndex WHERE name='" + exchange + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DBPlugin::verifyExchangeName: " << q.lastError().text();

  int rc = 1;
  if (q.next())
    rc = 0;
  
  return rc;
}


