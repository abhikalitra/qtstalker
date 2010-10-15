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

#include "TesterDataBase.h"

#include <QtDebug>

TesterDataBase::TesterDataBase ()
{
  _dbName = "data";
  
  // create the Tester table
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "CREATE TABLE IF NOT EXISTS TesterPlugin (";
  s.append("name TEXT PRIMARY KEY UNIQUE");
  s.append(", equity REAL");
  s.append(", period INT");
  s.append(", dateRange INT");
  s.append(", positionSize REAL");
  s.append(", futuresMode INT");
  s.append(", commission INT");
  s.append(", commissionValue REAL");
  s.append(", long INT");
  s.append(", longBuyPrice INT");
  s.append(", longSellPrice INT");
  s.append(", short INT");
  s.append(", shortBuyPrice INT");
  s.append(", shortSellPrice INT");
  s.append(", maximumLossStop INT");
  s.append(", maximumLossType INT");
  s.append(", maximumLossValue REAL");
  s.append(", maximumLossExit INT");
  s.append(", profitTargetStop INT");
  s.append(", profitTargetType INT");
  s.append(", profitTargetValue REAL");
  s.append(", profitTargetExit INT");
  s.append(", trailingStop INT");
  s.append(", trailingType INT");
  s.append(", trailingValue REAL");
  s.append(", trailingExit INT");
  s.append(", barsStop INT");
  s.append(", barsStopValue REAL");
  s.append(", barsStopExit INT");
  s.append(", elIndicator TEXT");
  s.append(", xlIndicator TEXT");
  s.append(", esIndicator TEXT");
  s.append(", xsIndicator TEXT");
  s.append(", enterLong TEXT");
  s.append(", exitLong TEXT");
  s.append(", enterShort TEXT");
  s.append(", exitShort TEXT");
  s.append(", elSettings TEXT");
  s.append(", xlSettings TEXT");
  s.append(", esSettings TEXT");
  s.append(", xsSettings TEXT");
  s.append(", symbols TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "TesterDataBase::TesterDataBase: " << q.lastError().text();
}

void TesterDataBase::getRules (QStringList &l)
{
  l.clear();
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT name FROM TesterPlugin ORDER BY name ASC";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "TesterDataBase::getRules: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());
}

int TesterDataBase::getRule (TesterSettings &rule)
{
  if (rule.name().isEmpty())
    return 1;
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString k = "SELECT ";
  k.append("equity,period,dateRange,positionSize,futuresMode,commission,commissionValue");
  k.append(",long,longBuyPrice,longSellPrice,short,shortBuyPrice,shortSellPrice,maximumLossStop");
  k.append(",maximumLossType,maximumLossValue,maximumLossExit,profitTargetStop,profitTargetType");
  k.append(",profitTargetValue,profitTargetExit,trailingStop,trailingType,trailingValue,trailingExit");
  k.append(",barsStop,barsStopValue,barsStopExit,elIndicator,xlIndicator,esIndicator,xsIndicator");
  k.append(",enterLong,exitLong,enterShort,exitShort");
  k.append(",elSettings,xlSettings,esSettings,xsSettings");
  k.append(",symbols");
  k.append(" FROM TesterPlugin WHERE name='" + rule.name() + "'");
  q.exec(k);
  if (q.lastError().isValid())
  {
    qDebug() << "TesterDataBase::getRule" << q.lastError().text();
    return 1;
  }

  if (! q.next())
    return 1;
  
  int pos = 0;
  rule.setEquity(q.value(pos++).toDouble());
  rule.setPeriod(q.value(pos++).toInt());
  rule.setDateRange(q.value(pos++).toInt());
  rule.setPositionSize(q.value(pos++).toDouble());
  rule.setFuturesMode(q.value(pos++).toInt());
  rule.setCommission(q.value(pos++).toInt());
  rule.setCommissionValue(q.value(pos++).toDouble());
  rule.setLong(q.value(pos++).toInt());
  rule.setLongBuyPrice(q.value(pos++).toInt());
  rule.setLongSellPrice(q.value(pos++).toInt());
  rule.setShort(q.value(pos++).toInt());
  rule.setShortBuyPrice(q.value(pos++).toInt());
  rule.setShortSellPrice(q.value(pos++).toInt());
  rule.setMaximumLossStop(q.value(pos++).toInt());
  rule.setMaximumLossType(q.value(pos++).toInt());
  rule.setMaximumLossValue(q.value(pos++).toDouble());
  rule.setMaximumLossExit(q.value(pos++).toInt());
  rule.setProfitTargetStop(q.value(pos++).toInt());
  rule.setProfitTargetType(q.value(pos++).toInt());
  rule.setProfitTargetValue(q.value(pos++).toDouble());
  rule.setProfitTargetExit(q.value(pos++).toInt());
  rule.setTrailingStop(q.value(pos++).toInt());
  rule.setTrailingType(q.value(pos++).toInt());
  rule.setTrailingValue(q.value(pos++).toDouble());
  rule.setTrailingExit(q.value(pos++).toInt());
  rule.setBarsStop(q.value(pos++).toInt());
  rule.setBarsStopValue(q.value(pos++).toDouble());
  rule.setBarsStopExit(q.value(pos++).toInt());
  rule.setELIndicator(q.value(pos++).toString());
  rule.setXLIndicator(q.value(pos++).toString());
  rule.setESIndicator(q.value(pos++).toString());
  rule.setXSIndicator(q.value(pos++).toString());
  rule.setEnterLong(q.value(pos++).toString().split(":"));
  rule.setExitLong(q.value(pos++).toString().split(":"));
  rule.setEnterShort(q.value(pos++).toString().split(":"));
  rule.setExitShort(q.value(pos++).toString().split(":"));
  
  QString d = q.value(pos++).toString();
  Setting set;
  set.parse(d);
  rule.setELSettings(set);
  
  d = q.value(pos++).toString();
  set.parse(d);
  rule.setXLSettings(set);

  d = q.value(pos++).toString();
  set.parse(d);
  rule.setESSettings(set);

  d = q.value(pos++).toString();
  set.parse(d);
  rule.setXSSettings(set);

  rule.setSymbols(q.value(pos++).toString().split(","));
  
  return 0;
}

void TesterDataBase::setRule (TesterSettings &rule)
{
  if (rule.name().isEmpty())
  {
    qDebug() << "TesterDataBase::setRule: no name";
    return;
  }
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "INSERT OR REPLACE INTO TesterPlugin (";
  s.append("name,equity,period,dateRange,positionSize,futuresMode,commission,commissionValue");
  s.append(",long,longBuyPrice,longSellPrice,short,shortBuyPrice,shortSellPrice,maximumLossStop");
  s.append(",maximumLossType,maximumLossValue,maximumLossExit,profitTargetStop,profitTargetType");
  s.append(",profitTargetValue,profitTargetExit,trailingStop,trailingType,trailingValue,trailingExit");
  s.append(",barsStop,barsStopValue,barsStopExit,elIndicator,xlIndicator,esIndicator,xsIndicator");
  s.append(",enterLong,exitLong,enterShort,exitShort");
  s.append(",elSettings,xlSettings,esSettings,xsSettings");
  s.append(",symbols");
  s.append(") VALUES (");
  s.append("'" + rule.name() + "'");
  s.append("," + QString::number(rule.equity()));
  s.append("," + QString::number(rule.period()));
  s.append("," + QString::number(rule.dateRange()));
  s.append("," + QString::number(rule.positionSize()));
  s.append("," + QString::number(rule.futuresMode()));
  s.append("," + QString::number(rule.commission()));
  s.append("," + QString::number(rule.commissionValue()));
  s.append("," + QString::number(rule.getLong()));
  s.append("," + QString::number(rule.longBuyPrice()));
  s.append("," + QString::number(rule.longSellPrice()));
  s.append("," + QString::number(rule.getShort()));
  s.append("," + QString::number(rule.shortBuyPrice()));
  s.append("," + QString::number(rule.shortSellPrice()));
  s.append("," + QString::number(rule.maximumLossStop()));
  s.append("," + QString::number(rule.maximumLossType()));
  s.append("," + QString::number(rule.maximumLossValue()));
  s.append("," + QString::number(rule.maximumLossExit()));
  s.append("," + QString::number(rule.profitTargetStop()));
  s.append("," + QString::number(rule.profitTargetType()));
  s.append("," + QString::number(rule.profitTargetValue()));
  s.append("," + QString::number(rule.profitTargetExit()));
  s.append("," + QString::number(rule.trailingStop()));
  s.append("," + QString::number(rule.trailingType()));
  s.append("," + QString::number(rule.trailingValue()));
  s.append("," + QString::number(rule.trailingExit()));
  s.append("," + QString::number(rule.barsStop()));
  s.append("," + QString::number(rule.barsStopValue()));
  s.append("," + QString::number(rule.barsStopExit()));
  s.append(",'" + rule.eLIndicator() + "'");
  s.append(",'" + rule.xLIndicator() + "'");
  s.append(",'" + rule.eSIndicator() + "'");
  s.append(",'" + rule.xSIndicator() + "'");
  s.append(",'" + rule.enterLong().join(":") + "'");
  s.append(",'" + rule.exitLong().join(":") + "'");
  s.append(",'" + rule.enterShort().join(":") + "'");
  s.append(",'" + rule.exitShort().join(":") + "'");
  
  Setting set = rule.eLSettings();
  QString d;
  set.getString(d);
  s.append(",'" + d + "'");
  
  set = rule.xLSettings();
  set.getString(d);
  s.append(",'" + d + "'");

  set = rule.eSSettings();
  set.getString(d);
  s.append(",'" + d + "'");

  set = rule.xSSettings();
  set.getString(d);
  s.append(",'" + d + "'");

  s.append(",'" + rule.symbols().join(",") + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "TesterDataBase::setRule" << q.lastError().text();
}

void TesterDataBase::deleteRule (QString &name)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "DELETE FROM TesterPlugin WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "TesterDataBase::deleteRule" << q.lastError().text();
}

