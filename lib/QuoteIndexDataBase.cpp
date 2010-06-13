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

#include "QuoteIndexDataBase.h"
#include "CODataBase.h"

#include <QDebug>
#include <QtSql>

QuoteIndexDataBase::QuoteIndexDataBase ()
{
  _dbName = "quotes";
}

int QuoteIndexDataBase::deleteSymbol (BarData *symbol)
{
  transaction();

  if (getIndexData(symbol))
    return 1;

  // delete any chart objects
  CODataBase db;
  db.deleteChartObjects(symbol);

  // drop quote table
  QString s = "DROP TABLE " + symbol->getTableName();
  if (command(s, QString("QuoteIndexDataBase::deleteSymbol: drop quotes table")))
    return 1;

  // remove index record
  s = "DELETE FROM symbolIndex";
  s.append(" WHERE symbol='" + symbol->getSymbol() + "'");
  s.append(" AND exchange='" + symbol->getExchange() + "'");
  if (command(s, QString("QuoteIndexDataBase::deleteSymbol: remove symbolIndex record")))
    return 1;

  commit();

  return 0;
}

void QuoteIndexDataBase::init (QString &dbFile)
{
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", _dbName);
  db.setHostName("localhost");
  db.setDatabaseName(dbFile);
  db.setUserName("QtStalker");
  db.setPassword("QtStalker");
  if (! db.open())
    qDebug() << "QuoteIndexDataBase::init: quotes db open failed";

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
    qDebug() << "QuoteIndexDataBase::init:" << q.lastError().text();
}

void QuoteIndexDataBase::getSearchList (QString &ex, QString &pat, Group &l)
{
  // if exchange and pat is empty then get all symbols from all exchanges will be returned
  QString s;
  if (ex.isEmpty() && pat.isEmpty())
    s = "SELECT symbol,name,exchange,plugin FROM symbolIndex ORDER BY symbol,exchange ASC";

  // if exchange is empty then get all symbols from all exchanges that match pat
  if (ex.isEmpty() && ! pat.isEmpty())
    s = "SELECT symbol,name,exchange,plugin FROM symbolIndex WHERE symbol LIKE '" + pat + "'" + "  ORDER BY symbol,exchange ASC";

  // exchange = yes and pat = no, get the entire exchange list
  if (! ex.isEmpty() && pat.isEmpty())
    s = "SELECT symbol,name,exchange,plugin FROM symbolIndex WHERE exchange='" + ex + "'" + " ORDER BY symbol,exchange ASC";
    
  // exchange = yes and pat = yes, get pat from the exchange
  if (! ex.isEmpty() && ! pat.isEmpty())
  {
    s = "SELECT symbol,name,exchange,plugin FROM symbolIndex WHERE symbol LIKE";
    s.append(" '" + pat + "'");
    s.append(" AND exchange='" + ex + "'");
    s.append(" ORDER BY symbol,exchange ASC");
  }
  
  if (s.isEmpty())
    return;
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteIndexDataBase::getSearchList: " << q.lastError().text();
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
    
    s = q.value(pos++).toString();
    bd->setPlugin(s);

    l.append(bd);
  }
}

int QuoteIndexDataBase::getIndexData (BarData *data)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT name,tableName,plugin FROM symbolIndex";
  s.append(" WHERE symbol='" + data->getSymbol() + "'");
  s.append(" AND exchange='" + data->getExchange() + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteIndexDataBase::getIndexData: " << q.lastError().text();
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

int QuoteIndexDataBase::setIndexData (BarData *data)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "UPDATE symbolIndex SET ";
  s.append("name='" + data->getName() + "'");
  s.append(" WHERE exchange='" + data->getExchange() + "'");
  s.append(" AND symbol='" + data->getSymbol() + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteIndexDataBase::setIndexData:" << q.lastError().text();
    return 1;
  }

  return 0;
}

int QuoteIndexDataBase::addSymbolIndex (BarData *bars)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));

  // we use the maximum record column and add 1 to it to generate a new table name
  QString table;
  QString ts = "SELECT max(record) FROM symbolIndex";
  q.exec(ts);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteIndexDataBase::addSymbolIndex: get record max" << q.lastError().text();
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
    qDebug() << "QuoteIndexDataBase::addSymbolIndex: create new symbolIndex record" << q.lastError().text();
    return 1;
  }
  
  bars->setTableName(table);
  
  return 0;
}

void QuoteIndexDataBase::getExchangeList (QStringList &l)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT DISTINCT exchange FROM symbolIndex ORDER BY exchange ASC";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteIndexDataBase::getExchangeList: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());
}

int QuoteIndexDataBase::rename (BarData *oldSymbol, BarData *newSymbol)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
  
  // make sure old symbol exists
  if (getIndexData(oldSymbol))
    return 1;

  // make sure new symbol does not exists
  if (getIndexData(newSymbol))
    return 1;
  if (newSymbol->getTableName().length())
    return 1;

  QString s = "UPDATE symbolIndex";
  s.append(" SET symbol='" + newSymbol->getSymbol() + "'");
  s.append(", exchange='" + newSymbol->getExchange() + "'");
  s.append(" WHERE symbol='" + oldSymbol->getSymbol() + "'");
  s.append(" AND exchange='" + oldSymbol->getExchange() + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteIndexDataBase::rename:" << q.lastError().text();
    return 1;
  }

  CODataBase codb;
  if (codb.renameSymbol(oldSymbol, newSymbol))
    return 1;
  
  return 0;
}
