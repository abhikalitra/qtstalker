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

#include <QtDebug>

TestDataBase::TestDataBase ()
{
  _table = "testIndex";
  _table2 = "testTrades";
  open();
  init();
}

void TestDataBase::init ()
{
  QSqlQuery q(_db);
  QString s = "CREATE TABLE IF NOT EXISTS " + _table + " (";
  s.append("name TEXT PRIMARY KEY");
  s.append(", symbol TEXT");
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

  s = "CREATE TABLE IF NOT EXISTS " + _table2 + " (";
  s.append("a INTEGER PRIMARY KEY");
  s.append(", name TEXT");
  s.append(", type TEXT");
  s.append(", enterDate TEXT");
  s.append(", enterPrice REAL");
  s.append(", volume INT");
  s.append(", exitDate TEXT");
  s.append(", exitPrice REAL");
  s.append(", profit REAL");
  s.append(", signal INT");
  s.append(", drawDown REAL");
  s.append(", barsHeld INT");
  s.append(", enterComm REAL");
  s.append(", exitComm REAL");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "TestDataBase::init: " << q.lastError().text();
}

int TestDataBase::trades (QString name, QList<TestTrade *> &trades)
{
  if (name.isEmpty())
    return 1;

  qDeleteAll(trades);
  trades.clear();

  QSqlQuery q(_db);
  QString s = "SELECT ";
  QStringList l;
  l << "type" << "enterDate" << "enterPrice" << "volume" << "exitDate" << "exitPrice" << "profit";
  l << "signal" << "drawDown" << "barsHeld" << "enterComm" << "exitComm";
  s.append(l.join(","));
  s.append(" FROM " + _table2);
  s.append(" WHERE name='" + name + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "TestDataBase::trades: " << q.lastError().text();
    return 1;
  }

  while (q.next())
  {
    int pos = 0;
    TestTrade *trade = new TestTrade;
    trade->setType(q.value(pos++).toString());
    trade->setEnterDate(q.value(pos++).toDateTime());
    trade->setEnterPrice(q.value(pos++).toDouble());
    trade->setVolume(q.value(pos++).toDouble());
    trade->setExitDate(q.value(pos++).toDateTime());
    trade->setExitPrice(q.value(pos++).toDouble());
    trade->setProfit(q.value(pos++).toDouble());
    trade->setSignal(q.value(pos++).toInt());
    trade->setDrawdown(q.value(pos++).toDouble());
    trade->setBarsHeld(q.value(pos++).toInt());
    trade->setEnterComm(q.value(pos++).toDouble());
    trade->setExitComm(q.value(pos++).toDouble());
    trades.append(trade);
  }

  return 0;
}

int TestDataBase::saveTrades (QString name, QList<TestTrade *> &trades)
{
  if (name.isEmpty())
    return 1;

  QSqlQuery q(_db);
  int loop = 0;
  for (; loop < trades.count(); loop++)
  {
    TestTrade *trade = trades.at(loop);

    QString s = "INSERT INTO " + _table2 + " VALUES (";
    s.append("NULL"); // auto increment
    s.append(",'" + trade->name() + "'");
    s.append(",'" + trade->type() + "'");
    s.append(",'" + trade->enterDate().toString(Qt::ISODate) + "'");
    s.append("," + QString::number(trade->enterPrice()));
    s.append("," + QString::number(trade->volume()));
    s.append(",'" + trade->exitDate().toString(Qt::ISODate) + "'");
    s.append("," + QString::number(trade->exitPrice()));
    s.append("," + QString::number(trade->profit()));
    s.append("," + QString::number(trade->signal()));
    s.append("," + QString::number(trade->drawdown()));
    s.append("," + QString::number(trade->barsHeld()));
    s.append("," + QString::number(trade->enterComm()));
    s.append("," + QString::number(trade->exitComm()));
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

  QString s = "SELECT name FROM " + _table;
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

int TestDataBase::saveSummary (TestSummary *report, QList<TestTrade *> &trades)
{
  // delete old one
  deleteSummary(report->name());

  QSqlQuery q(_db);
  QString s = "INSERT INTO " + _table + " VALUES (";
  s.append("'" + report->name() + "'");
  s.append(",'" + report->symbol() + "'");
  s.append("," + QString::number(report->equityGain()));
  s.append("," + QString::number(report->equity()));
  s.append("," + QString::number(report->totalProfit()));
  s.append("," + QString::number(report->profitableTrades()));
  s.append("," + QString::number(report->totalTrades()));
  s.append("," + QString::number(report->winTrades()));
  s.append("," + QString::number(report->loseTrades()));
  s.append("," + QString::number(report->maxDrawdown()));
  s.append("," + QString::number(report->avgProfitLoss()));
  s.append("," + QString::number(report->winTradesTotal()));
  s.append("," + QString::number(report->loseTradesTotal()));
  s.append("," + QString::number(report->avgBarsHeld()));
  s.append("," + QString::number(report->minBarsHeld()));
  s.append("," + QString::number(report->maxBarsHeld()));
  s.append("," + QString::number(report->commTotal()));
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "TestDataBase::saveSummary: " << q.lastError().text();
    qDebug() << s;
    return 1;
  }

  if (saveTrades(report->name(), trades))
    return 1;

  return 0;
}

int TestDataBase::summaries (QList<TestSummary *> &reports)
{
  QSqlQuery q(_db);

  qDeleteAll(reports);
  reports.clear();

  QString s = "SELECT ";
  QStringList l;
  l << "name" << "symbol" << "equityGain" << "equity" << "totalProfit";
  l << "profitableTrades" << "totalTrades" << "winTrades" << "loseTrades" << "maxDrawdown";
  l << "avgProfitLoss" << "totalWinTrades" << "totalLoseTrades" << "avgBarsHeld";
  l << "minBarsHeld" << "maxBarsHeld" << "totalCommissions";
  s.append(l.join(","));
  s.append(" FROM " + _table);
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "TestDataBase::summaries: " << q.lastError().text();
    return 1;
  }

  while (q.next())
  {
    int pos = 0;
    TestSummary *report = new TestSummary;
    report->setName(q.value(pos++).toString());
    report->setSymbol(q.value(pos++).toString());
    report->setEquityGain(q.value(pos++).toDouble());
    report->setEquity(q.value(pos++).toDouble());
    report->setTotalProfit(q.value(pos++).toDouble());
    report->setProfitableTrades(q.value(pos++).toDouble());
    report->setTotalTrades(q.value(pos++).toInt());
    report->setWinTrades(q.value(pos++).toInt());
    report->setLoseTrades(q.value(pos++).toInt());
    report->setMaxDrawdown(q.value(pos++).toDouble());
    report->setAvgProfitLoss(q.value(pos++).toDouble());
    report->setWinTradesTotal(q.value(pos++).toDouble());
    report->setLoseTradesTotal(q.value(pos++).toDouble());
    report->setAvgBarsHeld(q.value(pos++).toInt());
    report->setMinBarsHeld(q.value(pos++).toInt());
    report->setMaxBarsHeld(q.value(pos++).toInt());
    report->setCommTotal(q.value(pos++).toDouble());
    reports << report;
  }

  return 0;
}

int TestDataBase::deleteSummary (QString name)
{
  QSqlQuery q(_db);
  QString s = "DELETE FROM " + _table + " WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "TestDataBase::deleteSummary: " << q.lastError().text();
    return 1;
  }

  s = "DELETE FROM " + _table2 + " WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "TestDataBase::deleteSummary: " << q.lastError().text();
    return 1;
  }

  return 0;
}
