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

#include "DataBase.h"
#include "Bar.h"
#include "Config.h"

#include <QtDebug>
#include <QtSql>
#include <QVariant>


DataBase::DataBase ()
{
}

/********************************************************************************/
/********************* chart functions *******************************************/
/********************************************************************************/

void DataBase::getAllChartsList (QStringList &l)
{
  QString indexTable, symbolColumn;
  Config config;
  config.getData(Config::DbIndexTable, indexTable);
  config.getData(Config::DbSymbolColumn, symbolColumn);

  l.clear();
  QSqlQuery q(QSqlDatabase::database("quotes"));
  QString s = "SELECT " + symbolColumn + " FROM " + indexTable;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getAllChartsList: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());

  l.sort();
}

void DataBase::getSearchList (QString &pat, QStringList &l)
{
  QString indexTable, symbolColumn;
  Config config;
  config.getData(Config::DbIndexTable, indexTable);
  config.getData(Config::DbSymbolColumn, symbolColumn);

  l.clear();

  QSqlQuery q(QSqlDatabase::database("quotes"));
  QString s = "SELECT " + symbolColumn + " FROM " + indexTable + " WHERE " + symbolColumn + " LIKE '" + pat + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getSearchList: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());

  l.sort();
}

void DataBase::getChart (BarData *data)
{
  QSqlQuery q(QSqlDatabase::database("quotes"));

  QString symbol, ts, ts2, dateColumn, openColumn, highColumn, lowColumn, closeColumn, volumeColumn, oiColumn;
  QString nameColumn, indexTable, symbolColumn;
  data->getSymbol(symbol);
  
  Config config;
  config.getData(Config::DbIndexTable, indexTable);
  config.getData(Config::DbSymbolColumn, symbolColumn);
  config.getData(Config::DbNameColumn, nameColumn);

  // get the index details
  QString s = "SELECT " + nameColumn + " FROM " + indexTable + " WHERE " + symbolColumn + "='" + symbol + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getChart: " << q.lastError().text();
    return;
  }

  if (q.next())
  {
    s = q.value(0).toString();
    data->setName(s);
  }
  else
  {
    qDebug() << "DataBase::getChart: no symbolIndex record found";
    return;
  }

  QDateTime firstDate;
  getFirstDate(firstDate, symbol);

  QDateTime lastDate;
  getLastDate(lastDate, symbol);

  QDateTime sdate = lastDate;
  QDateTime edate = lastDate;

  setStartEndDates(sdate, edate, data->getBarLength());
  if (sdate < firstDate)
    return;
  
  config.getData(Config::DbDateColumn, dateColumn);
  config.getData(Config::DbOpenColumn, openColumn);
  config.getData(Config::DbHighColumn, highColumn);
  config.getData(Config::DbLowColumn, lowColumn);
  config.getData(Config::DbCloseColumn, closeColumn);
  config.getData(Config::DbVolumeColumn, volumeColumn);
  config.getData(Config::DbOIColumn, oiColumn);

  while (1)
  {
    QString sd = sdate.toString("yyyy-MM-dd HH:mm:ss");
    QString ed = edate.toString("yyyy-MM-dd HH:mm:ss");

    QString s = "SELECT " + dateColumn;
    s.append("," + openColumn);
    s.append("," + highColumn);
    s.append("," + lowColumn);
    s.append("," + closeColumn);
    s.append("," + volumeColumn);
    s.append("," + oiColumn);
    s.append(" FROM " + symbol + " WHERE " + dateColumn + " >='" + sd + "' AND " + dateColumn + " < '" + ed + "'");
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "DataBase::getChart:" << q.lastError().text();
      break;
    }

    int openFlag = FALSE;
    Bar *bar = new Bar;
    while (q.next())
    {
      QDateTime dt = q.value(0).toDateTime();
      bar->setDate(dt);

      // check if this is first data for bar construction, we need to set the open
      if (! openFlag)
      {
        QVariant v = q.value(1);
	if (v.isValid())
	  bar->setOpen(v.toDouble());
	
        v = q.value(2);
	if (v.isValid())
	  bar->setHigh(v.toDouble());
	
        v = q.value(3);
	if (v.isValid())
	  bar->setLow(v.toDouble());
	
        v = q.value(4);
	if (v.isValid())
	  bar->setClose(v.toDouble());
	
        v = q.value(5);
	if (v.isValid())
	  bar->setVolume(v.toDouble());
	
        v = q.value(6);
	if (v.isValid())
	  bar->setOI(v.toDouble());
	
        openFlag = TRUE;
        continue;
      }

      QVariant v = q.value(2);
      if (v.isValid())
      {
        double h = v.toDouble();
        if (h > bar->getHigh())
          bar->setHigh(h);
      }

      v = q.value(3);
      if (v.isValid())
      {
        double l = v.toDouble();
        if (l < bar->getLow())
          bar->setLow(l);
      }

      v = q.value(4);
      if (v.isValid())
        bar->setClose(v.toDouble());

      v = q.value(5);
      if (v.isValid())
      {
        double v1 = v.toDouble();
        double v2 = bar->getVolume();
        bar->setVolume(v1 + v2);
      }

      v = q.value(6);
      if (v.isValid())
        bar->setOI(v.toDouble());
    }

    if (bar->getEmptyFlag())
      delete bar;
    else
      data->prepend(bar);

    if (data->count() == data->getBarsRequested())
      break;

    edate = sdate;
    getDateOffset(sdate, data->getBarLength());
    if (sdate < firstDate)
      break;
  }

  data->createDateList();
}

void DataBase::getFirstDate (QDateTime &date, QString &symbol)
{
  QString dateColumn;
  Config config;
  config.getData(Config::DbDateColumn, dateColumn);

  QSqlQuery q(QSqlDatabase::database("quotes"));
  QString s = "SELECT " + dateColumn + " FROM " + symbol + " ORDER BY " + dateColumn + " ASC LIMIT 1";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getFirstDate:" << q.lastError().text();
    return;
  }

  if (q.next())
    date = q.value(0).toDateTime();
}

void DataBase::getLastDate (QDateTime &date, QString &symbol)
{
  QString dateColumn;
  Config config;
  config.getData(Config::DbDateColumn, dateColumn);

  QSqlQuery q(QSqlDatabase::database("quotes"));
  QString s = "SELECT " + dateColumn + " FROM " + symbol + " ORDER BY " + dateColumn + " DESC LIMIT 1";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getLastDate:" << q.lastError().text();
    return;
  }

  if (q.next())
    date = q.value(0).toDateTime();

}

void DataBase::setStartEndDates (QDateTime &startDate, QDateTime &endDate, BarData::BarLength barLength)
{
  QString s, s2;
  int tint = 0;
  QDateTime dt = endDate;

  switch (barLength)
  {
    case BarData::Minute1:
      dt.setTime(QTime(dt.time().hour(), dt.time().minute(), 0, 0));
      startDate = dt;
      dt = dt.addSecs(60);
      endDate = dt;
      break;
    case BarData::Minute5:
      tint = dt.time().minute() / 5;
      dt.setTime(QTime(dt.time().hour(), tint * 5, 0, 0));
      startDate = dt;
      dt = dt.addSecs(300);
      endDate = dt;
      break;
    case BarData::Minute10:
      tint = dt.time().minute() / 10;
      dt.setTime(QTime(dt.time().hour(), tint * 10, 0, 0));
      startDate = dt;
      dt = dt.addSecs(600);
      endDate = dt;
      break;
    case BarData::Minute15:
      tint = dt.time().minute() / 15;
      dt.setTime(QTime(dt.time().hour(), tint * 15, 0, 0));
      startDate = dt;
      dt = dt.addSecs(900);
      endDate = dt;
      break;
    case BarData::Minute30:
      tint = dt.time().minute() / 30;
      dt.setTime(QTime(dt.time().hour(), tint * 30, 0, 0));
      startDate = dt;
      dt = dt.addSecs(1800);
      endDate = dt;
      break;
    case BarData::Minute60:
      dt.setTime(QTime(dt.time().hour(), 0, 0, 0));
      startDate = dt;
      dt = dt.addSecs(3600);
      endDate = dt;
      break;
    case BarData::DailyBar:
      dt.setTime(QTime(0, 0, 0, 0));
      startDate = dt;
      dt = dt.addDays(1);
      endDate = dt;
      break;
    case BarData::WeeklyBar:
      dt.setTime(QTime(0, 0, 0, 0));
      dt = dt.addDays(- dt.date().dayOfWeek());
      startDate = dt;
      dt = dt.addDays(7);
      endDate = dt;
      break;
    case BarData::MonthlyBar:
      dt.setTime(QTime(0, 0, 0, 0));
      dt = dt.addDays(- (dt.date().day() - 1));
      startDate = dt;
      dt = dt.addDays(dt.date().daysInMonth());
      endDate = dt;
      break;
    default:
      break;
  }
}

void DataBase::getDateOffset (QDateTime &dt, BarData::BarLength barLength)
{
  switch (barLength)
  {
    case BarData::Minute1:
      dt = dt.addSecs(-(60));
      break;
    case BarData::Minute5:
      dt = dt.addSecs(-(300));
      break;
    case BarData::Minute10:
      dt = dt.addSecs(-(600));
      break;
    case BarData::Minute15:
      dt = dt.addSecs(-(900));
      break;
    case BarData::Minute30:
      dt = dt.addSecs(-(1800));
      break;
    case BarData::Minute60:
      dt = dt.addSecs(-(3600));
      break;
    case BarData::DailyBar:
      dt = dt.addDays(-(1));
      break;
    case BarData::WeeklyBar:
      dt = dt.addDays(-(7));
      break;
    case BarData::MonthlyBar:
      dt = dt.addMonths(-(1));
      break;
    default:
      break;
  }
}

/********************************************************************************/
/********************* group functions *******************************************/
/********************************************************************************/

void DataBase::getAllGroupsList (QStringList &l)
{
  l.clear();
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT name FROM groupIndex";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getGroupsList: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());

  l.sort();
}

void DataBase::getGroupList (QString &n, QStringList &l)
{
  l.clear();
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT parms FROM groupIndex WHERE name='" + n + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getGroupList: " << q.lastError().text();
    return;
  }

  if (q.next())
    l = q.value(0).toString().split(",");
}

void DataBase::setGroupList (QString &n, QStringList &l)
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "INSERT OR REPLACE INTO groupIndex VALUES ('" + n + "','" + l.join(",") + "')";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::setGroupList: " << q.lastError().text();
}

void DataBase::deleteGroup (QString &n)
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "DELETE FROM groupIndex WHERE name='" + n + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::deleteGroup: " << q.lastError().text();
}

/********************************************************************************/
/********************* indicator functions *******************************************/
/********************************************************************************/

void DataBase::getIndicator (Indicator &parms)
{
  QString name;
  parms.getName(name);

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT * FROM indicatorIndex WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getIndicator: " << q.lastError().text();
    return;
  }

  if (! q.next())
    return;

  parms.setEnable(q.value(1).toInt());
  parms.setTabRow(q.value(2).toInt());
  parms.setDate(q.value(3).toInt());
  parms.setLog(q.value(4).toInt());
  s = q.value(5).toString();
  parms.setCommand(s);
}

void DataBase::setIndicator (Indicator &i)
{
  QString name, enable, tabRow, date, log, command;
  
  i.getName(name);
  enable = QString::number(i.getEnable());
  tabRow = QString::number(i.getTabRow());
  date = QString::number(i.getDate());
  log = QString::number(i.getLog());
  i.getCommand(command);

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "INSERT OR REPLACE INTO indicatorIndex VALUES ('" + name + "'," + enable + "," + tabRow + "," + date + "," + log + ",'" + command + "')";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::setIndicator: " << q.lastError().text();
}

void DataBase::deleteIndicator (QString &name)
{
  // delete any chart objects tied to this indicator before we delete it
  deleteChartObjectsIndicator(name);

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "DELETE FROM indicatorIndex WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::deleteIndicatorIndex: " << q.lastError().text();
}

void DataBase::getIndicatorList (QStringList &l)
{
  l.clear();
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT name FROM indicatorIndex";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getIndicatorList: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());

  l.sort();
}

void DataBase::getActiveIndicatorList (QStringList &l)
{
  l.clear();

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT name FROM indicatorIndex WHERE enable=1";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getActiveIndicatorList: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());

  l.sort();
}

void DataBase::getSearchIndicatorList (QString &pattern, QStringList &list)
{
  list.clear();

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT name FROM indicatorIndex WHERE name LIKE '" + pattern + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getSearchIndicatorList: " << q.lastError().text();
    return;
  }

  while (q.next())
    list.append(q.value(0).toString());

  list.sort();
}

/********************************************************************************/
/****************************** scanner functions *********************************/
/********************************************************************************/

void DataBase::deleteScanner (QString &name)
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "DELETE FROM scanners WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::deleteScanner: " << q.lastError().text();
}

void DataBase::getScannerList (QStringList &l)
{
  l.clear();

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT name FROM scanners";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getScannerList: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());

  l.sort();
}

void DataBase::getScanner (ScannerRule &rule)
{
  QString ts;
  rule.getName(ts);

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT * FROM scanners WHERE name='" + ts + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getScanner: " << q.lastError().text();
    return;
  }

  if (q.next())
  {
    QString s = q.value(1).toString();
    Indicator i;
    i.setCommand(s);
    rule.setIndicator(i);

    s = q.value(2).toString();
    rule.setAllSymbols(s);

    QStringList l = q.value(3).toString().split(",");
    rule.setFileList(l);

    s = q.value(4).toString();
    rule.setBarLength(s);

    s = q.value(5).toString();
    rule.setBars(s);
  }
}

void DataBase::setScanner (ScannerRule &rule)
{
  QString name;
  rule.getName(name);

  Indicator i;
  rule.getIndicator(i);
  QString parms;
  i.getCommand(parms);

  QString allSymbols;
  rule.getAllSymbols(allSymbols);

  QStringList l;
  rule.getFileList(l);
  QString fileList = l.join(",");

  QString barLength;
  rule.getBarLength(barLength);

  QString bars;
  rule.getBars(bars);

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "INSERT OR REPLACE INTO scanners VALUES ('" + name + "','" + parms + "'," + allSymbols + ",'" + fileList + "','" + barLength + "'," + bars + ")";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::setSacnner: " << q.lastError().text();
}

/********************************************************************************/
/****************************** tester functions *********************************/
/********************************************************************************/

void DataBase::deleteTest (QString &name)
{
  // delete indicators linked to the tester
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "DELETE FROM testers WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::deleteTest: " << q.lastError().text();
}

void DataBase::getTestList (QStringList &l)
{
  l.clear();

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT name FROM testers";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getTestList: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());

  l.sort();
}

void DataBase::getTest (TesterRule &rule)
{
  QString name;
  rule.getName(name);

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT * FROM testers WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getTestList: " << q.lastError().text();
    return;
  }

  if (q.next())
  {
    s = q.value(0).toString();
    rule.setName(s);

    s = q.value(1).toString();
    rule.setParms(s);

    s = q.value(2).toString();
    rule.setTrades(s);

    s = q.value(3).toString();
    rule.setSummary(s);

    s = q.value(4).toString();
    Indicator i;
    i.setCommand(s);
    rule.setEnterLong(i);

    s = q.value(5).toString();
    i.setCommand(s);
    rule.setExitLong(i);

    s = q.value(6).toString();
    i.setCommand(s);
    rule.setEnterShort(i);

    s = q.value(7).toString();
    i.setCommand(s);
    rule.setExitShort(i);

    s = q.value(8).toString();
    i.setCommand(s);
    rule.setCustomLongStop(i);

    s = q.value(9).toString();
    i.setCommand(s);
    rule.setCustomShortStop(i);
  }
}

void DataBase::setTest (TesterRule &rule)
{
  QString name, parms, trades, summary, el, xl, es, xs, ls, ss;
  rule.getName(name);
  rule.getParms(parms);
  rule.getTrades(trades);
  rule.getSummary(summary);

  Indicator i;
  rule.getEnterLong(i);
  i.getCommand(el);

  rule.getExitLong(i);
  i.getCommand(xl);

  rule.getEnterShort(i);
  i.getCommand(es);

  rule.getExitShort(i);
  i.getCommand(xs);

  rule.getCustomLongStop(i);
  i.getCommand(ls);

  rule.getCustomShortStop(i);
  i.getCommand(ss);

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "INSERT OR REPLACE INTO testers VALUES ('" + name + "','" + parms + "','" + trades + "','" + summary + "','" + el + "','" + xl + "','" + es + "','" + xs + "','" + ls + "','" + ss + "')";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::setTest: " << q.lastError().text();
}

/********************************************************************************/
/********** chart object functions **********************************************/
/********************************************************************************/

void DataBase::deleteChartObjects (QString &symbol)
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "DELETE FROM chartObjects WHERE symbol='" + symbol + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::deleteChartObjects: " << q.lastError().text();
    return;
  }
}

void DataBase::deleteChartObjectsIndicator (QString &indicator)
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "DELETE FROM chartObjects WHERE indicator='" + indicator + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::deleteChartObjectsIndicator: " << q.lastError().text();
    return;
  }
}

void DataBase::deleteChartObject (QString &id)
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "DELETE FROM chartObjects WHERE id=" + id;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::deleteChartObject: " << q.lastError().text();
    return;
  }
}

void DataBase::getChartObjects (QString &symbol, QString &indicator, QList<COSettings> &list)
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT id,symbol,indicator,type,settings FROM chartObjects WHERE symbol='" + symbol + "' AND indicator='" + indicator + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getChartObjects: " << q.lastError().text();
    return;
  }

  while (q.next())
  {
    COSettings co(q.value(0).toString(), q.value(1).toString(), q.value(2).toString(), q.value(3).toString());
    QString ts = q.value(4).toString();
    co.parse(ts);
    list.append(co);
  }
}

void DataBase::setChartObject (COSettings &co)
{
  QString id;
  co.getID(id);

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT id FROM chartObjects WHERE id=" + id;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::setChartObject: " << q.lastError().text();
    return;
  }

  if (q.next())
  {
    QString settings;
    co.getSettings(settings);

    s = "UPDATE chartObjects SET settings='" + settings + "' WHERE id=" + id;
    q.exec(s);
    if (q.lastError().isValid())
      qDebug() << "DataBase::setChartObject: " << q.lastError().text();
  }
  else
  {
    QString symbol, indicator, type, settings;
    co.getSymbol(symbol);
    co.getIndicator(indicator);
    co.getType(type);
    co.getSettings(settings);

    s = "INSERT OR REPLACE INTO chartObjects VALUES (" + id + ",'" + symbol + "','" + indicator + "','" + type + "','" + settings + "')";
    q.exec(s);
    if (q.lastError().isValid())
      qDebug() << "DataBase::setChartObject: " << q.lastError().text();
  }
}


