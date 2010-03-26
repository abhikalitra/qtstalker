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
#include "Bar.h"

#include <QDebug>
#include <QtSql>

DBPlugin::DBPlugin ()
{
  dbName = "quotes";
}

// virtual
DBPlugin::~DBPlugin ()
{
  if (quotes.count())
    qDeleteAll(quotes);
}

// virtual
void DBPlugin::getBars (BarData &)
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
}

void DBPlugin::getSearchList (QString &ex, QString &pat, Group &l)
{
  // if exchange and pat is empty then get all symbols from all exchanges will be returned
  QString s;
  if (ex.isEmpty() && pat.isEmpty())
    s = "SELECT symbol,name,exchange FROM symbolIndex ORDER BY symbol,exchange ASC";

  // if exchange is empty then get all symbols from all exchanges that match pat
  if (ex.isEmpty() && ! pat.isEmpty())
    s = "SELECT symbol,name,exchange FROM symbolIndex WHERE symbol LIKE '" + pat + "'" + "  ORDER BY symbol,exchange ASC";

  // exchange = yes and pat = no, get the entire exchange list
  if (! ex.isEmpty() && pat.isEmpty())
    s = "SELECT symbol,name,exchange FROM symbolIndex WHERE exchange='" + ex + "'" + " ORDER BY symbol,exchange ASC";
    
  // exchange = yes and pat = yes, get pat from the exchange
  if (! ex.isEmpty() && ! pat.isEmpty())
  {
    s = "SELECT symbol,name,exchange FROM symbolIndex WHERE symbol LIKE";
    s.append(" '" + pat + "'");
    s.append(" AND exchange='" + ex + "'");
    s.append(" ORDER BY symbol,exchange ASC");
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
    int pos = 0;
    BarData *bd = new BarData;
    
    s = q.value(pos++).toString();
    bd->setSymbol(s);
    
    s = q.value(pos++).toString();
    bd->setName(s);
    
    s = q.value(pos++).toString();
    bd->setExchange(s);
    
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

int DBPlugin::getIndexData (BarData *data)
{
  QSqlQuery q(QSqlDatabase::database(dbName));
  QString s = "SELECT name,tableName,plugin FROM symbolIndex";
  s.append(" WHERE symbol='" + data->getSymbol() + "'");
  s.append(" AND exchange='" + data->getExchange() + "'");
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
    data->setName(s);

    s = q.value(pos++).toString();
    data->setTableName(s);
    
    s = q.value(pos++).toString();
    data->setPlugin(s);
  }

  return 0;
}

int DBPlugin::setIndexData (BarData *data)
{
  QSqlQuery q(QSqlDatabase::database(dbName));
  QString s = "UPDATE symbolIndex SET ";
  s.append("name='" + data->getName() + "'");
  s.append(" WHERE exchange='" + data->getExchange() + "'");
  s.append(" AND symbol='" + data->getSymbol() + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DBPlugin::setIndexData:" << q.lastError().text();
    return 1;
  }

  return 0;
}

int DBPlugin::addSymbolIndex (BarData *bars)
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
  ts.append("'" + bars->getSymbol() + "'");
  ts.append(",'" + table + "'");
  ts.append(",'" + bars->getExchange() + "'");
  ts.append(",'" + bars->getPlugin() + "'");
  ts.append(")");
  q.exec(ts);
  if (q.lastError().isValid())
  {
    qDebug() << "DBPlugin::addSymbolIndex: create new symbolIndex record" << q.lastError().text();
    return 1;
  }
  
  bars->setTableName(table);
  
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

void DBPlugin::barErrorMessage (int rc)
{
  QString function = "::scriptCommand:";
  
  switch ((Bar::RC) rc)
  {
    case Bar::RC_InvalidOpen:
      qDebug() << plugin << function << "invalid open";
      break;
    case Bar::RC_InvalidHigh:
      qDebug() << plugin << function << "invalid high";
      break;
    case Bar::RC_InvalidLow:
      qDebug() << plugin << function << "invalid low";
      break;
    case Bar::RC_InvalidClose:
      qDebug() << plugin << function << "invalid close";
      break;
    case Bar::RC_InvalidVolume:
      qDebug() << plugin << function << "invalid volume";
      break;
    case Bar::RC_InvalidOI:
      qDebug() << plugin << function << "invalid oi";
      break;
    case Bar::RC_OGTH: // open > high
      qDebug() << plugin << function << "open > high";
      break;
    case Bar::RC_OLTL: // open < low
      qDebug() << plugin << function << "open < low";
      break;
    case Bar::RC_CGTH: // close > high
      qDebug() << plugin << function << "close > high";
      break;
    case Bar::RC_CLTL: // close < low
      qDebug() << plugin << function << "close < low";
      break;
    case Bar::RC_VLT0: // volume < 0
      qDebug() << plugin << function << "volume < 0";
      break;
    case Bar::RC_LGTH: // low > high
      qDebug() << plugin << function << "low > high";
      break;
    case Bar::RC_HLTL: // high < low
      qDebug() << plugin << function << "high < low";
      break;
    case Bar::RC_OILT0: // oi < 0
      qDebug() << plugin << function << "oi < 0";
      break;
    default:
      break;
  }
}

