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

#include "TesterReportDataBase.h"

#include <QtDebug>

TesterReportDataBase::TesterReportDataBase ()
{
  _dbName = "data";
  
  // create the Tester report table
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "CREATE TABLE IF NOT EXISTS TesterPluginReport (";
  s.append("name TEXT PRIMARY KEY UNIQUE");
  s.append(", tradeList TEXT");
  s.append(", profitFactor REAL");
  s.append(", payoffRatio REAL");
  s.append(", equityGain REAL");
  s.append(", equity REAL");
  s.append(", totalProfit REAL");
  s.append(", profitableTrades REAL");
  s.append(", totalTrades INT");
  s.append(", winTrades INT");
  s.append(", loseTrades INT");
  s.append(", maxDrawdown REAL");
  s.append(", avgProfitLoss REAL");
  s.append(", totalWinTrades REAL");
  s.append(", totalLoseTrades REAL");
  s.append(", avgBarsHeld INT");
  s.append(", minBarsHeld INT");
  s.append(", maxBarsHeld INT");
  s.append(", totalCommissions REAL");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "TesterReportDataBase::TesterReportDataBase: " << q.lastError().text();
}

void TesterReportDataBase::getReports (QStringList &l)
{
  l.clear();
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT name FROM TesterPluginReport ORDER BY name ASC";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "TesterReportDataBase::getReport: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());
}

int TesterReportDataBase::getReport (TesterReport &report)
{
  if (report.name().isEmpty())
    return 1;
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString k = "SELECT ";
  k.append("tradeList,profitFactor,payoffRatio,equityGain,equity,totalProfit,profitableTrades");
  k.append(",totalTrades,winTrades,loseTrades,maxDrawdown,avgProfitLoss,totalWinTrades");
  k.append(",totalLoseTrades,avgBarsHeld,minBarsHeld,maxBarsHeld,totalCommissions");
  k.append(" FROM TesterPluginReport WHERE name='" + report.name() + "'");
  q.exec(k);
  if (q.lastError().isValid())
  {
    qDebug() << "TesterReportDataBase::getReport" << q.lastError().text();
    return 1;
  }

  if (! q.next())
    return 1;
  
  int pos = 0;
  report.setTrades(q.value(pos++).toString().split("|"));
  report.setProfitFactor(q.value(pos++).toDouble());
  report.setPayoffRatio(q.value(pos++).toDouble());
  report.setEquityGain(q.value(pos++).toDouble());
  report.setEquity(q.value(pos++).toDouble());
  report.setTotalProfit(q.value(pos++).toDouble());
  report.setProfitableTrades(q.value(pos++).toDouble());
  report.setTotalTrades(q.value(pos++).toInt());
  report.setWinTrades(q.value(pos++).toInt());
  report.setLoseTrades(q.value(pos++).toInt());
  report.setMaxDrawdown(q.value(pos++).toDouble());
  report.setAvgProfitLoss(q.value(pos++).toDouble());
  report.setTotalWinTrades(q.value(pos++).toDouble());
  report.setTotalLoseTrades(q.value(pos++).toDouble());
  report.setAvgBarsHeld(q.value(pos++).toInt());
  report.setMinBarsHeld(q.value(pos++).toInt());
  report.setMaxBarsHeld(q.value(pos++).toInt());
  report.setTotalCommissions(q.value(pos++).toDouble());
  
  return 0;
}

void TesterReportDataBase::setReport (TesterReport &report)
{
  if (report.name().isEmpty())
  {
    qDebug() << "TesterReportDataBase::setReport: no name";
    return;
  }
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "INSERT OR REPLACE INTO TesterPluginReport (";
  s.append("name,tradeList,profitFactor,payoffRatio,equityGain,equity,totalProfit,profitableTrades");
  s.append(",totalTrades,winTrades,loseTrades,maxDrawdown,avgProfitLoss,totalWinTrades");
  s.append(",totalLoseTrades,avgBarsHeld,minBarsHeld,maxBarsHeld,totalCommissions");
  s.append(") VALUES (");
  s.append("'" + report.name() + "'");
  s.append(",'" + report.trades().join("|") + "'");
  s.append("," + QString::number(report.profitFactor()));
  s.append("," + QString::number(report.payoffRatio()));
  s.append("," + QString::number(report.equityGain()));
  s.append("," + QString::number(report.equity()));
  s.append("," + QString::number(report.totalProfit()));
  s.append("," + QString::number(report.profitableTrades()));
  s.append("," + QString::number(report.totalTrades()));
  s.append("," + QString::number(report.winTrades()));
  s.append("," + QString::number(report.loseTrades()));
  s.append("," + QString::number(report.maxDrawdown()));
  s.append("," + QString::number(report.avgProfitLoss()));
  s.append("," + QString::number(report.totalWinTrades()));
  s.append("," + QString::number(report.totalLoseTrades()));
  s.append("," + QString::number(report.avgBarsHeld()));
  s.append("," + QString::number(report.minBarsHeld()));
  s.append("," + QString::number(report.maxBarsHeld()));
  s.append("," + QString::number(report.totalCommissions()));
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "TesterReportDataBase::setReport" << q.lastError().text();
    qDebug() << s;
  }
}

void TesterReportDataBase::deleteReport (QString &name)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "DELETE FROM TesterPluginReport WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "TesterReportDataBase::deleteReport" << q.lastError().text();
}

void TesterReportDataBase::reportRatings (QList<TesterReport> &l)
{
  l.clear();

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT name,equityGain FROM TesterPluginReport ORDER BY equityGain DESC";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "TesterReportDataBase::getReport: " << q.lastError().text();
    return;
  }

  while (q.next())
  {
    int pos = 0;
    TesterReport report;
    report.setName(q.value(pos++).toString());
    report.setEquityGain(q.value(pos++).toDouble());

    l.append(report);
  }
}

