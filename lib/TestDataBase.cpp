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

#include "TestDataBase.h"
#include "Globals.h"

#include <QtDebug>

TestDataBase::TestDataBase ()
{
  init();
}

void TestDataBase::init ()
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
      qDebug() << "TestDataBase::init:" << _db.lastError().text();
  }

  QSqlQuery q(_db);
  QString s = "CREATE TABLE IF NOT EXISTS testTrades (";
  s.append("a INTEGER PRIMARY KEY");
  s.append(", name TEXT");
  s.append(", symbol TEXT");
  s.append(", type TEXT");
  s.append(", enterDate TEXT");
  s.append(", enterPrice REAL");
  s.append(", volume INT");
  s.append(", exitDate TEXT");
  s.append(", exitPrice REAL");
  s.append(", profit REAL");
  s.append(", signal TEXT");
  s.append(", equity REAL");
  s.append(", drawDown REAL");
  s.append(", barsHeld INT");
  s.append(", enterComm REAL");
  s.append(", exitComm REAL");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "TestDataBase::init: " << q.lastError().text();
}

int TestDataBase::trades (Setting parms, QList<Setting> &trades)
{
  QSqlQuery q(_db);

  trades.clear();
  
  QString s = "SELECT symbol,type,enterDate,enterPrice,volume,exitDate,exitPrice,profit,signal,equity,drawDown,barsHeld,enterComm,exitComm FROM testTrades";
  s.append(" WHERE name='" + parms.data("NAME") + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "TestDataBase::trades: " << q.lastError().text();
    return 1;
  }

  while (q.next())
  {
    int pos = 0;
    Setting trade;
    trade.setData("SYMBOL", q.value(pos++).toString());
    trade.setData("TYPE", q.value(pos++).toString());
    trade.setData("ENTER_DATE", q.value(pos++).toString());
    trade.setData("ENTER_PRICE", q.value(pos++).toString());
    trade.setData("VOLUME", q.value(pos++).toString());
    trade.setData("EXIT_DATE", q.value(pos++).toString());
    trade.setData("EXIT_PRICE", q.value(pos++).toString());
    trade.setData("PROFIT", q.value(pos++).toString());
    trade.setData("SIGNAL", q.value(pos++).toString());
    trade.setData("EQUITY", q.value(pos++).toString());
    trade.setData("DRAWDOWN", q.value(pos++).toString());
    trade.setData("BARS_HELD", q.value(pos++).toString());
    trade.setData("ENTER_COMM", q.value(pos++).toString());
    trade.setData("EXIT_COMM", q.value(pos++).toString());
    trades.append(trade);
  }

  return 0;
}

int TestDataBase::deleteTrades (Setting parms)
{
  QSqlQuery q(_db);

  // delete index records
  QString s = "DELETE FROM testTrades";
  s.append(" WHERE name='" + parms.data("NAME") + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "TestDataBase::deleteTrades: " << q.lastError().text();
    return 1;
  }

  return 0;
}

int TestDataBase::saveTrades (Setting parms, QList<Setting *> &trades)
{
  // remove old trades if we are replacing with the same version
  deleteTrades(parms);
  
  QSqlQuery q(_db);

  int loop = 0;
  for (; loop < trades.count(); loop++)
  {
    Setting *trade = trades.at(loop);
    
    QString s = "INSERT INTO testTrades VALUES (";
    s.append("NULL"); // auto increment 
    s.append(",'" + parms.data("NAME") + "'");
    s.append(",'" + trade->data("SYMBOL") + "'");
    s.append(",'" + trade->data("TYPE") + "'");
    s.append(",'" + trade->data("ENTER_DATE") + "'");
    s.append("," + trade->data("ENTER_PRICE"));
    s.append("," + trade->data("VOLUME"));
    s.append(",'" + trade->data("EXIT_DATE") + "'");
    s.append("," + trade->data("EXIT_PRICE"));
    s.append("," + trade->data("PROFIT"));
    s.append(",'" + trade->data("SIGNAL") + "'");
    s.append("," + trade->data("EQUITY"));
    s.append("," + trade->data("DRAWDOWN"));
    s.append("," + trade->data("BARS_HELD"));
    s.append("," + trade->data("ENTER_COMM"));
    s.append("," + trade->data("EXIT_COMM"));
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "TestDataBase::saveTrades: " << q.lastError().text();
      qDebug() << s;
//      return 1;
    }
  }

  return 0;
}

int TestDataBase::names (QStringList &l)
{
  QSqlQuery q(_db);

  l.clear();

  QString s = "SELECT DISTINCT name FROM testTrades";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "TestDataBase::names: " << q.lastError().text();
    return 1;
  }

  while (q.next())
    l << q.value(0).toString();

  l.sort();

  return 0;
}

void TestDataBase::transaction ()
{
  _db.transaction();
}

void TestDataBase::commit ()
{
  _db.commit();
}
