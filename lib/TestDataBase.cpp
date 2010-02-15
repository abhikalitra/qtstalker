/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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
#include <QtSql>
#include <QDir>


TestDataBase::TestDataBase ()
{
}

void TestDataBase::init ()
{
  QDir dir(QDir::homePath());
  QString home = dir.absolutePath();

  // create the home directory where all local data is stored
  home.append("/QtStalkerTest");
  if (! dir.exists(home))
  {
    if (! dir.mkdir(home))
      qDebug() << "Unable to create" << home <<  "directory.";
  }

  QString s = home + "/data.sqlite";
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "testData");
  db.setHostName("me");
  db.setDatabaseName(s);
  db.setUserName("QtStalkerTest");
  db.setPassword("QtStalkerTest");
  if (! db.open())
  {
    qDebug() << "TestDataBase::TestDataBase:" << db.lastError().text();
    return;
  }

  // create the index table
  QSqlQuery q(db);
  s = "CREATE TABLE IF NOT EXISTS testIndex (";
  s.append("name TEXT PRIMARY KEY");
  s.append(",script TEXT");
  s.append(",symbol TEXT");
  s.append(",enterField INT");
  s.append(",exitField INT");
  s.append(",bars INT");
  s.append(",barLength INT");
  s.append(",entryComm REAL");
  s.append(",exitComm REAL");
  s.append(",delay INT");
  s.append(",account REAL");
  s.append(",volumePercentage REAL");

  // summary data
  s.append(",grossProfit REAL");
  s.append(",netProfit REAL");
  s.append(",maxDrawDown REAL");
  s.append(",avgDrawDown REAL");
  s.append(",commissions REAL");
  s.append(",winLossRatio REAL");
  s.append(",trades INT");
  s.append(",profitable REAL");
  s.append(",winTrades INT");
  s.append(",lossTrades INT");
  s.append(",maxWinTrade REAL");
  s.append(",maxLossTrade REAL");
  s.append(",avgWinTrade REAL");
  s.append(",avgLossTrade REAL");
  s.append(",maxWinLong REAL");
  s.append(",maxLossLong REAL");
  s.append(",maxWinShort REAL");
  s.append(",maxLossShort REAL");
  s.append(",tradeLog TEXT");
  s.append(",balance REAL");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "TestDataBase::createIndexTable: " << q.lastError().text();
}

void TestDataBase::transaction ()
{
  QSqlDatabase db = QSqlDatabase::database("testData");
  db.transaction();
}

void TestDataBase::commit ()
{
  QSqlDatabase db = QSqlDatabase::database("testData");
  db.commit();
}

void TestDataBase::getTests (QStringList &l)
{
  l.clear();

  QSqlQuery q(QSqlDatabase::database("testData"));
  QString s = "SELECT name FROM testIndex";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "TestDataBase::getTests: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());

  l.sort();
}

void TestDataBase::getTest (Test &test)
{
  QString name;
  test.getName(name);

  QSqlQuery q(QSqlDatabase::database("testData"));
  QString s = "SELECT * FROM testIndex WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "TestDataBase::getTest: " << q.lastError().text();
    return;
  }

  while (q.next())
  {
    int col = 1;
    s = q.value(col++).toString();
    test.setScript(s);

    s = q.value(col++).toString();
    test.setSymbol(s);

    test.setEnterField(q.value(col++).toInt());
    test.setExitField(q.value(col++).toInt());
    test.setBars(q.value(col++).toInt());
    test.setBarLength(q.value(col++).toInt());
    test.setEntryComm(q.value(col++).toDouble());
    test.setExitComm(q.value(col++).toDouble());
    test.setDelay(q.value(col++).toInt());
    test.setAccount(q.value(col++).toDouble());
    test.setVolumePercentage(q.value(col++).toDouble());

    // summary stuff
    test.setGrossProfit(q.value(col++).toDouble());
    test.setNetProfit(q.value(col++).toDouble());
    test.setMaxDrawDown(q.value(col++).toDouble());
    test.setAvgDrawDown(q.value(col++).toDouble());
    test.setCommissions(q.value(col++).toDouble());
    test.setWinLossRatio(q.value(col++).toDouble());
    test.setTrades(q.value(col++).toInt());
    test.setProfitable(q.value(col++).toDouble());
    test.setWinTrades(q.value(col++).toInt());
    test.setLossTrades(q.value(col++).toInt());
    test.setMaxWinTrade(q.value(col++).toDouble());
    test.setMaxLossTrade(q.value(col++).toDouble());
    test.setAvgWinTrade(q.value(col++).toDouble());
    test.setAvgLossTrade(q.value(col++).toDouble());
    test.setMaxWinLong(q.value(col++).toDouble());
    test.setMaxLossLong(q.value(col++).toDouble());
    test.setMaxWinShort(q.value(col++).toDouble());
    test.setMaxLossShort(q.value(col++).toDouble());

    s = q.value(col++).toString();
    test.setTradeLog(s);

    test.setBalance(q.value(col++).toDouble());
  }
}

void TestDataBase::setTest (Test &test)
{
  QString name, ts;
  test.getName(name);

  transaction();

  QSqlQuery q(QSqlDatabase::database("testData"));
  QString s = "INSERT OR REPLACE INTO testIndex  VALUES (";

  s.append("'" + name + "'");

  test.getScript(ts);
  s.append(",'" + ts + "'");

  test.getSymbol(ts);
  s.append(",'" + ts + "'");

  s.append("," + QString::number(test.getEnterField()));
  s.append("," + QString::number(test.getExitField()));
  s.append("," + QString::number(test.getBars()));
  s.append("," + QString::number(test.getBarLength()));
  s.append("," + QString::number(test.getEntryComm()));
  s.append("," + QString::number(test.getExitComm()));
  s.append("," + QString::number(test.getDelay()));
  s.append("," + QString::number(test.getAccount()));
  s.append("," + QString::number(test.getVolumePercentage()));

  // summary stuff
  s.append("," + QString::number(test.getGrossProfit()));
  s.append("," + QString::number(test.getNetProfit()));
  s.append("," + QString::number(test.getMaxDrawDown()));
  s.append("," + QString::number(test.getAvgDrawDown()));
  s.append("," + QString::number(test.getCommissions()));
  s.append("," + QString::number(test.getWinLossRatio()));
  s.append("," + QString::number(test.getTrades()));
  s.append("," + QString::number(test.getProfitable()));
  s.append("," + QString::number(test.getWinTrades()));
  s.append("," + QString::number(test.getLossTrades()));
  s.append("," + QString::number(test.getMaxWinTrade()));
  s.append("," + QString::number(test.getMaxLossTrade()));
  s.append("," + QString::number(test.getAvgWinTrade()));
  s.append("," + QString::number(test.getAvgLossTrade()));
  s.append("," + QString::number(test.getMaxWinLong()));
  s.append("," + QString::number(test.getMaxLossLong()));
  s.append("," + QString::number(test.getMaxWinShort()));
  s.append("," + QString::number(test.getMaxLossShort()));

  test.getTradeLog(ts);
  s.append(",'" + ts + "'");

  s.append("," + QString::number(test.getBalance()));

  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "TestDataBase::setTest:" << q.lastError().text();

  commit();
}

void TestDataBase::deleteTest (Test &test)
{
  transaction();

  QString name;
  test.getName(name);

  QSqlQuery q(QSqlDatabase::database("testData"));
  QString s = "DELETE FROM testIndex WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "TestDataBase::deleteTest:" << q.lastError().text();

  commit();
}

