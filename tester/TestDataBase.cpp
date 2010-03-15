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

  // create the results table
  QSqlQuery q(db);
  s = "CREATE TABLE IF NOT EXISTS results (";
  s.append("name TEXT PRIMARY KEY");
  s.append(",script TEXT");
  s.append(",shellCommand TEXT");
  s.append(",comment TEXT");
  s.append(",symbol TEXT");
  s.append(",exchange TEXT");
  s.append(",enterField INT");
  s.append(",exitField INT");
  s.append(",bars INT");
  s.append(",barLength INT");
  s.append(",entryComm REAL");
  s.append(",exitComm REAL");
  s.append(",account REAL");
  s.append(",volumePercentage REAL");
  s.append(",trailingStop REAL");
  s.append(",trailingCheck INT");

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
  QString s = "SELECT name FROM results";
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

void TestDataBase::getTest (QString &name, QSqlQuery &q)
{
  QSqlQuery tq(QSqlDatabase::database("testData"));
  QString s = "SELECT * FROM results WHERE name='" + name + "'";
  tq.exec(s);
  if (tq.lastError().isValid())
  {
    qDebug() << "TestDataBase::getTest: " << tq.lastError().text();
    return;
  }

  q = tq;
}

void TestDataBase::setTest (QString &d)
{
  transaction();

  QSqlQuery q(QSqlDatabase::database("testData"));
  q.exec(d);
  if (q.lastError().isValid())
  {
    qDebug() << "TestDataBase::setTest:" << q.lastError().text();
    qDebug() << d;
  }

  commit();
}

void TestDataBase::deleteTest (QString &name)
{
  transaction();

  QSqlQuery q(QSqlDatabase::database("testData"));
  QString s = "DELETE FROM results WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "TestDataBase::deleteTest:" << q.lastError().text();

  commit();
}

void TestDataBase::getRankings (QSqlQuery &rq)
{
  transaction();

  QSqlQuery q(QSqlDatabase::database("testData"));
  QString s = "SELECT name,winLossRatio,grossProfit,netProfit FROM results ORDER BY netProfit DESC";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "TestDataBase::deleteTest:" << q.lastError().text();

  rq = q;

  commit();
}

