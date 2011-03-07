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
  _db = QSqlDatabase::database("test");
  if (! _db.isOpen())
  {
    QString s = QDir::homePath() + "/.qtstalker/test.sqlite";
    _db = QSqlDatabase::addDatabase("QSQLITE", "test");
    _db.setHostName("QtStalker");
    _db.setDatabaseName(s);
    _db.setUserName("QtStalker");
    _db.setPassword("QtStalker");
    if (! _db.open())
      qDebug() << "TestDataBase::init:" << _db.lastError().text();
  }

  QSqlQuery q(_db);
  QString s = "CREATE TABLE IF NOT EXISTS testIndex (";
  s.append("a INTEGER PRIMARY KEY");
  s.append(", name TEXT");
  s.append(", version TEXT");
  s.append(", tradeTable TEXT");
  s.append(", symbol TEXT");
  s.append(", profitFactor REAL");
  s.append(", payoffRatio REAL");
  s.append(", equityGain REAL");
  s.append(", equity REAL");
  s.append(", totalProfit REAL");
  s.append(", profitableTrades REAL");
  s.append(", totalTrades REAL");
  s.append(", winTrades REAL");
  s.append(", loseTrades REAL");
  s.append(", maxDrawdown REAL");
  s.append(", avgProfitLoss REAL");
  s.append(", totalWinTrades REAL");
  s.append(", totalLoseTrades REAL");
  s.append(", avgBarsHeld REAL");
  s.append(", minBarsHeld REAL");
  s.append(", maxBarsHeld REAL");
  s.append(", totalCommissions REAL");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "TestDataBase::init: " << q.lastError().text();
}

int TestDataBase::trades (Setting parms, QList<Setting> &trades)
{
  trades.clear();

  QString table;
  if (tableName(parms, table))
    return 1;
  
  QSqlQuery q(_db);
  QString s = "SELECT ";
  QStringList l;
  l << "type" << "enterDate" << "enterPrice" << "volume" << "exitDate" << "exitPrice" << "profit";
  l << "signal" << "equity" << "drawDown" << "barsHeld" << "enterComm" << "exitComm";
  s.append(l.join(","));
  s.append(" FROM " + table);
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

int TestDataBase::saveTrades (QString table, QList<Setting *> &trades)
{
  // create the trades table
  QSqlQuery q(_db);
  QString s = "CREATE TABLE IF NOT EXISTS " + table + " (";
  s.append("a INTEGER PRIMARY KEY");
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
  {
    qDebug() << "TestDataBase::saveTrades: " << q.lastError().text();
    return 1;
  }

  int loop = 0;
  for (; loop < trades.count(); loop++)
  {
    Setting *trade = trades.at(loop);
    
    QString s = "INSERT INTO " + table + " VALUES (";
    s.append("NULL"); // auto increment 
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
    }
  }

  return 0;
}

int TestDataBase::names (QStringList &l)
{
  QSqlQuery q(_db);

  l.clear();

  QString s = "SELECT DISTINCT name FROM testIndex";
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

int TestDataBase::versions (QString name, QStringList &l)
{
  QSqlQuery q(_db);

  l.clear();

  QString s = "SELECT DISTINCT version FROM testIndex";
  s.append(" WHERE name='" + name + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "TestDataBase::versions: " << q.lastError().text();
    return 1;
  }

  while (q.next())
    l << q.value(0).toString();

  l.sort();

  return 0;
}

int TestDataBase::saveSummary (Setting report, QList<Setting *> &trades)
{
  // delete old one
  deleteSummary(report);

  QString table;
  if (newTableName(table))
    return 1;

  QSqlQuery q(_db);
  QString s = "INSERT INTO testIndex VALUES (";
  s.append("NULL"); // auto increment
  s.append(",'" + report.data("NAME") + "'");
  s.append(",'" + report.data("VERSION") + "'");
  s.append(",'" + table + "'");
  s.append(",'" + report.data("SYMBOL") + "'");
  s.append("," + report.data("PROFIT_FACTOR"));
  s.append("," + report.data("PAYOFF_RATIO"));
  s.append("," + report.data("EQUITY_GAIN"));
  s.append("," + report.data("EQUITY"));
  s.append("," + report.data("TOTAL_PROFIT"));
  s.append("," + report.data("PROFITABLE_TRADES"));
  s.append("," + report.data("TOTAL_TRADES"));
  s.append("," + report.data("WIN_TRADES"));
  s.append("," + report.data("LOSE_TRADES"));
  s.append("," + report.data("MAX_DRAWDOWN"));
  s.append("," + report.data("AVG_PROFIT_LOSS"));
  s.append("," + report.data("TOTAL_WIN_TRADES"));
  s.append("," + report.data("TOTAL_LOSE_TRADES"));
  s.append("," + report.data("AVG_BARS_HELD"));
  s.append("," + report.data("MIN_BARS_HELD"));
  s.append("," + report.data("MAX_BARS_HELD"));
  s.append("," + report.data("TOTAL_COMMISSIONS"));
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "TestDataBase::saveSummary: " << q.lastError().text();
    qDebug() << s;
    return 1;
  }

  if (saveTrades(table, trades))
    return 1;

  return 0;
}

int TestDataBase::summaries (Setting parms, QList<Setting> &reports)
{
  QSqlQuery q(_db);

  reports.clear();

  QString s = "SELECT ";
  QStringList l;
  l << "version" << "symbol" << "profitFactor" << "payoffRatio" << "equityGain" << "equity" << "totalProfit";
  l << "profitableTrades" << "totalTrades" << "winTrades" << "loseTrades" << "maxDrawdown";
  l << "avgProfitLoss" << "totalWinTrades" << "totalLoseTrades" << "avgBarsHeld";
  l << "minBarsHeld" << "maxBarsHeld" << "totalCommissions";
  s.append(l.join(","));
  s.append(" FROM testIndex");
  s.append(" WHERE name='" + parms.data("NAME") + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "TestDataBase::summaries: " << q.lastError().text();
    return 1;
  }

  while (q.next())
  {
    int pos = 0;
    Setting report;
    report.setData("VERSION", q.value(pos++).toString());
    report.setData("SYMBOL", q.value(pos++).toString());
    report.setData("PROFIT_FACTOR", q.value(pos++).toString());
    report.setData("PAYOFF_RATIO", q.value(pos++).toString());
    report.setData("EQUITY_GAIN", q.value(pos++).toString());
    report.setData("EQUITY", q.value(pos++).toString());
    report.setData("TOTAL_PROFIT", q.value(pos++).toString());
    report.setData("PROFITABLE_TRADES", q.value(pos++).toString());
    report.setData("TOTAL_TRADES", q.value(pos++).toString());
    report.setData("WIN_TRADES", q.value(pos++).toString());
    report.setData("LOSE_TRADES", q.value(pos++).toString());
    report.setData("MAX_DRAWDOWN", q.value(pos++).toString());
    report.setData("AVG_PROFIT_LOSS", q.value(pos++).toString());
    report.setData("TOTAL_WIN_TRADES", q.value(pos++).toString());
    report.setData("TOTAL_LOSE_TRADES", q.value(pos++).toString());
    report.setData("AVG_BARS_HELD", q.value(pos++).toString());
    report.setData("MIN_BARS_HELD", q.value(pos++).toString());
    report.setData("MAX_BARS_HELD", q.value(pos++).toString());
    report.setData("TOTAL_COMMISSIONS", q.value(pos++).toString());
    reports.append(report);
  }

  return 0;
}

int TestDataBase::deleteSummary (Setting parms)
{
  QString table;
  if (tableName(parms, table))
    return 1;

  if (! table.isEmpty())
  {
    if (deleteTable(table))
      return 1;
  }

  QSqlQuery q(_db);
  QString s = "DELETE FROM testIndex";
  s.append(" WHERE name='" + parms.data("NAME") + "'");
  s.append(" AND version='" + parms.data("VERSION") + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "TestDataBase::deleteSummary: " << q.lastError().text();
    return 1;
  }

  return 0;
}

int TestDataBase::deleteName (QString name)
{
  QSqlQuery q(_db);

  QStringList l;
  versions(name, l);

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    Setting parms;
    parms.setData("NAME", name);
    parms.setData("VERSION", l.at(loop));

    QString table;
    if (tableName(parms, table))
      continue;

    if (deleteTable(table))
      continue;
  }

  // delete index records
  QString s = "DELETE FROM testIndex";
  s.append(" WHERE name='" + name + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "TestDataBase::deleteName: " << q.lastError().text();
    return 1;
  }

  return 0;
}

int TestDataBase::newTableName (QString &table)
{
  table.clear();

  QSqlQuery q(_db);

  while (1)
  {
    table = "T" + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
    QString s = "SELECT name FROM sqlite_master WHERE name='" + table + "'";
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "TestDataBase::newTableName:" + q.lastError().text();
      return 1;
    }

    if (! q.next())
      break;
  }

  return 0;
}

int TestDataBase::tableName (Setting &parms, QString &table)
{
  table.clear();
  
  QSqlQuery q(_db);

  QString s = "SELECT tradeTable FROM testIndex";
  s.append(" WHERE name='" + parms.data("NAME") + "'");
  s.append(" AND version='" + parms.data("VERSION") + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "TestDataBase::tableName: " << q.lastError().text();
    return 1;
  }

  if (! q.next())
    return 0;

  table = q.value(0).toString();

  return 0;
}

int TestDataBase::deleteTable (QString table)
{
  QSqlQuery q(_db);

  // drop trade table
  QString s = "DROP TABLE " + table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "TestDataBase::deleteTable:" + q.lastError().text();
    return 1;
  }

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
