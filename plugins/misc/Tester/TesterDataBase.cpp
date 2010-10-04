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
  s.append(", position INT");
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
  s.append(", report TEXT");
  s.append(", indicator TEXT");
  s.append(", plotNames TEXT");
  s.append(", enterLong TEXT");
  s.append(", exitLong TEXT");
  s.append(", enterShort TEXT");
  s.append(", exitShort TEXT");
  s.append(", indicatorSettings TEXT");
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

int TesterDataBase::getRule (Setting &rule)
{
  QString name;
  rule.getData(_Name, name);
  if (name.isEmpty())
    return 1;
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString k = "SELECT ";
  k.append("equity,position,period,dateRange,positionSize,futuresMode,commission,commissionValue");
  k.append(",long,longBuyPrice,longSellPrice,short,shortBuyPrice,shortSellPrice,maximumLossStop");
  k.append(",maximumLossType,maximumLossValue,maximumLossExit,profitTargetStop,profitTargetType");
  k.append(",profitTargetValue,profitTargetExit,trailingStop,trailingType,trailingValue,trailingExit");
  k.append(",barsStop,barsStopValue,barsStopExit,report,indicator,plotNames");
  k.append(",enterLong,exitLong,enterShort,exitShort,indicatorSettings,symbols");
  k.append(" FROM TesterPlugin WHERE name='" + name + "'");
  q.exec(k);
  if (q.lastError().isValid())
  {
    qDebug() << "TesterDataBase::getRule" << q.lastError().text();
    return 1;
  }

  if (! q.next())
    return 1;
  
  int pos = 0;
  rule.setData(_Equity, q.value(pos++).toDouble());
  rule.setData(_Position, q.value(pos++).toInt());
  rule.setData(_Period, q.value(pos++).toInt());
  rule.setData(_DateRange, q.value(pos++).toInt());
  rule.setData(_PositionSize, q.value(pos++).toDouble());
  rule.setData(_FuturesMode, q.value(pos++).toInt());
  rule.setData(_Commission, q.value(pos++).toInt());
  rule.setData(_CommissionValue, q.value(pos++).toDouble());
  rule.setData(_Long, q.value(pos++).toInt());
  rule.setData(_LongBuyPrice, q.value(pos++).toInt());
  rule.setData(_LongSellPrice, q.value(pos++).toInt());
  rule.setData(_Short, q.value(pos++).toInt());
  rule.setData(_ShortBuyPrice, q.value(pos++).toInt());
  rule.setData(_ShortSellPrice, q.value(pos++).toInt());
  rule.setData(_MaximumLossStop, q.value(pos++).toInt());
  rule.setData(_MaximumLossType, q.value(pos++).toInt());
  rule.setData(_MaximumLossValue, q.value(pos++).toDouble());
  rule.setData(_MaximumLossExit, q.value(pos++).toInt());
  rule.setData(_ProfitTargetStop, q.value(pos++).toInt());
  rule.setData(_ProfitTargetType, q.value(pos++).toInt());
  rule.setData(_ProfitTargetValue, q.value(pos++).toDouble());
  rule.setData(_ProfitTargetExit, q.value(pos++).toInt());
  rule.setData(_TrailingStop, q.value(pos++).toInt());
  rule.setData(_TrailingType, q.value(pos++).toInt());
  rule.setData(_TrailingValue, q.value(pos++).toDouble());
  rule.setData(_TrailingExit, q.value(pos++).toInt());
  rule.setData(_BarsStop, q.value(pos++).toInt());
  rule.setData(_BarsStopValue, q.value(pos++).toDouble());
  rule.setData(_BarsStopExit, q.value(pos++).toInt());
  rule.setData(_Report, q.value(pos++).toString());
  rule.setData(_Indicator, q.value(pos++).toString());
  rule.setData(_PlotNames, q.value(pos++).toString());
  rule.setData(_EnterLong, q.value(pos++).toString());
  rule.setData(_ExitLong, q.value(pos++).toString());
  rule.setData(_EnterShort, q.value(pos++).toString());
  rule.setData(_ExitShort, q.value(pos++).toString());
  rule.setData(_IndicatorSettings, q.value(pos++).toString());
  rule.setData(_Symbols, q.value(pos++).toString());
  
  return 0;
}

void TesterDataBase::setRule (Setting &rule)
{
  QString name;
  rule.getData(_Name, name);
  if (name.isEmpty())
  {
    qDebug() << "TesterDataBase::setRule: no name";
    return;
  }
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "INSERT OR REPLACE INTO TesterPlugin (";
  s.append("name,equity,position,period,dateRange,positionSize,futuresMode,commission,commissionValue");
  s.append(",long,longBuyPrice,longSellPrice,short,shortBuyPrice,shortSellPrice,maximumLossStop");
  s.append(",maximumLossType,maximumLossValue,maximumLossExit,profitTargetStop,profitTargetType");
  s.append(",profitTargetValue,profitTargetExit,trailingStop,trailingType,trailingValue,trailingExit");
  s.append(",barsStop,barsStopValue,barsStopExit,report,indicator,plotNames");
  s.append(",enterLong,exitLong,enterShort,exitShort,indicatorSettings,symbols");
  s.append(") VALUES (");
  s.append("'" + name + "'");
  s.append("," + QString::number(rule.getDouble(_Equity)));
  s.append("," + QString::number(rule.getInt(_Position)));
  s.append("," + QString::number(rule.getInt(_Period)));
  s.append("," + QString::number(rule.getInt(_DateRange)));
  s.append("," + QString::number(rule.getDouble(_PositionSize)));
  s.append("," + QString::number(rule.getInt(_FuturesMode)));
  s.append("," + QString::number(rule.getInt(_Commission)));
  s.append("," + QString::number(rule.getDouble(_CommissionValue)));
  s.append("," + QString::number(rule.getInt(_Long)));
  s.append("," + QString::number(rule.getInt(_LongBuyPrice)));
  s.append("," + QString::number(rule.getInt(_LongSellPrice)));
  s.append("," + QString::number(rule.getInt(_Short)));
  s.append("," + QString::number(rule.getInt(_ShortBuyPrice)));
  s.append("," + QString::number(rule.getInt(_ShortSellPrice)));
  s.append("," + QString::number(rule.getInt(_MaximumLossStop)));
  s.append("," + QString::number(rule.getInt(_MaximumLossType)));
  s.append("," + QString::number(rule.getDouble(_MaximumLossValue)));
  s.append("," + QString::number(rule.getInt(_MaximumLossExit)));
  s.append("," + QString::number(rule.getInt(_ProfitTargetStop)));
  s.append("," + QString::number(rule.getInt(_ProfitTargetType)));
  s.append("," + QString::number(rule.getDouble(_ProfitTargetValue)));
  s.append("," + QString::number(rule.getInt(_ProfitTargetExit)));
  s.append("," + QString::number(rule.getInt(_TrailingStop)));
  s.append("," + QString::number(rule.getInt(_TrailingType)));
  s.append("," + QString::number(rule.getDouble(_TrailingValue)));
  s.append("," + QString::number(rule.getInt(_TrailingExit)));
  s.append("," + QString::number(rule.getInt(_BarsStop)));
  s.append("," + QString::number(rule.getDouble(_BarsStopValue)));
  s.append("," + QString::number(rule.getInt(_BarsStopExit)));

  QString d;
  rule.getData(_Report, d);
  s.append(",'" + d + "'");
  
  rule.getData(_Indicator, d);
  s.append(",'" + d + "'");
  
  rule.getData(_PlotNames, d);
  s.append(",'" + d + "'");
  
  rule.getData(_EnterLong, d);
  s.append(",'" + d + "'");
  
  rule.getData(_ExitLong, d);
  s.append(",'" + d + "'");
  
  rule.getData(_EnterShort, d);
  s.append(",'" + d + "'");
  
  rule.getData(_ExitShort, d);
  s.append(",'" + d + "'");
  
  rule.getData(_IndicatorSettings, d);
  s.append(",'" + d + "'");

  rule.getData(_Symbols, d);
  s.append(",'" + d + "'");

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

