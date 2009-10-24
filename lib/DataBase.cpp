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

DataBase::DataBase (QString session)
{
  QString s = QDir::homePath() + "/.qtstalker/data.sqlite" + session;
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "data");
  db.setHostName("me");
  db.setDatabaseName(s);
  db.setUserName("qtstalker");
  db.setPassword("qtstalker");
  if (! db.open())
  {
    qDebug() << "DataBase::DataBase:" << db.lastError().text();
    return;
  }

  // create the config table
  QSqlQuery q(db);
  s = "CREATE TABLE IF NOT EXISTS config (key INT PRIMARY KEY, setting TEXT)";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::createConfigTable: " << q.lastError().text();

  // create the group index table
  s = "CREATE TABLE IF NOT EXISTS groupIndex (name TEXT PRIMARY KEY, parms TEXT)";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::createGroupIndexTable: " << q.lastError().text();

  // create the indicator index table
  s = "CREATE TABLE IF NOT EXISTS indicatorIndex (name TEXT PRIMARY KEY, enable INT, tabRow INT, date INT, log INT, command TEXT, path TEXT)";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::createIndicatorIndexTable: " << q.lastError().text();

  // create the chart object table
  s = "CREATE TABLE IF NOT EXISTS chartObjects (id INT PRIMARY KEY, symbol TEXT, indicator TEXT, type TEXT, settings TEXT)";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::createChartObjectsTable: " << q.lastError().text();

  // create the indicator settings table
  s = "CREATE TABLE IF NOT EXISTS indicatorSettings (indicator TEXT PRIMARY KEY UNIQUE, name TEXT, settings TEXT)";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::createIndicatorSettingsTable: " << q.lastError().text();
}

void DataBase::transaction ()
{
  QSqlDatabase db = QSqlDatabase::database("data");
  db.transaction();
}

void DataBase::commit ()
{
  QSqlDatabase db = QSqlDatabase::database("data");
  db.commit();
}

/********************************************************************************/
/********************* chart functions *******************************************/
/********************************************************************************/

void DataBase::getAllChartsList (QStringList &l)
{
  l.clear();
  QString s;
  Config config;
  config.getData(Config::DbAllSymbols, s);

  QSqlQuery q(QSqlDatabase::database("quotes"));
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
  l.clear();
  QString s;
  Config config;
  config.getData(Config::DbSearchSymbols, s);

  QSqlQuery q(QSqlDatabase::database("quotes"));
  s.append(" '" + pat + "'"); // add the pattern to the end of the sql command
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

  QString symbol, ts, ts2, s, sql;
  data->getSymbol(symbol);
  
  Config config;
  config.getData(Config::DbGetSymbol, sql);

  QDateTime firstDate;
  getFirstDate(firstDate, symbol);

  QDateTime lastDate;
  getLastDate(lastDate, symbol);

  QHash<QString, Bar *> bars;
  QStringList dateList;

  while (1)
  {
    QString sd = firstDate.toString("yyyy-MM-dd HH:mm:ss");
    sd.prepend("'"); // we need to surround date strings in ' chars here because of TEXT storage conflicts.
    sd.append("'");

    QString ed = lastDate.toString("yyyy-MM-dd HH:mm:ss");
    ed.prepend("'"); // we need to surround date strings in ' chars here because of TEXT storage conflicts.
    ed.append("'");

    s = sql;
    s.replace("$symbol", symbol, Qt::CaseSensitive);
    s.replace("$sd", sd, Qt::CaseSensitive);
    s.replace("$ed", ed, Qt::CaseSensitive);
    s.replace("$records", QString::number(data->getBarsRequested()), Qt::CaseSensitive);
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "DataBase::getChart:" << q.lastError().text();
      break;
    }
    
    while (q.next())
    {
      QDateTime dt = q.value(0).toDateTime();
      lastDate = dt;
      QDateTime sd, ed;
      setStartEndDates(dt, sd, ed, data->getBarLength());
      s = sd.toString("yyyyMMddHHmmss") + ed.toString("yyyyMMddHHmmss");
      Bar *bar = bars[s];
      if (! bar)
      {
	if (bars.count() == data->getBarsRequested())
	  break;
	
	dateList.append(s); // save new dateList entry
	bar = new Bar;
        bar->setDate(dt); // save actual date

	bars.insert(s, bar);
	
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
	
	continue;
      }

      QVariant v = q.value(1);
      if (v.isValid())
        bar->setOpen(v.toDouble());
	
      v = q.value(2);
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

      v = q.value(5);
      if (v.isValid())
      {
        double v1 = v.toDouble();
        double v2 = bar->getVolume();
        bar->setVolume(v1 + v2);
      }
    }
    
    if (bars.count() == data->getBarsRequested())
      break;

    if (lastDate == firstDate)
      break;
  }

  // order the bars from most recent to first
  int loop;
  for (loop = 0; loop < dateList.count(); loop++)
    data->prepend(bars[dateList[loop]]);

  data->createDateList();
}

void DataBase::getFirstDate (QDateTime &date, QString &symbol)
{
  QString s;
  Config config;
  config.getData(Config::DbFirstDate, s);

  QSqlQuery q(QSqlDatabase::database("quotes"));
  s.replace("$symbol", symbol, Qt::CaseSensitive);
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
  QString s;
  Config config;
  config.getData(Config::DbLastDate, s);

  QSqlQuery q(QSqlDatabase::database("quotes"));
  s.replace("$symbol", symbol, Qt::CaseSensitive);
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getLastDate:" << q.lastError().text();
    return;
  }

  if (q.next())
    date = q.value(0).toDateTime();
}

void DataBase::setStartEndDates (QDateTime &date, QDateTime &startDate, QDateTime &endDate, BarData::BarLength barLength)
{
  QString s, s2;
  int tint = 0;
  startDate = date;

  switch (barLength)
  {
    case BarData::Minute1:
      startDate.setTime(QTime(startDate.time().hour(), startDate.time().minute(), 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(60);
      break;
    case BarData::Minute5:
      tint = startDate.time().minute() / 5;
      startDate.setTime(QTime(startDate.time().hour(), tint * 5, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(300);
      break;
    case BarData::Minute10:
      tint = startDate.time().minute() / 10;
      startDate.setTime(QTime(startDate.time().hour(), tint * 10, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(600);
      break;
    case BarData::Minute15:
      tint = startDate.time().minute() / 15;
      startDate.setTime(QTime(startDate.time().hour(), tint * 15, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(900);
      break;
    case BarData::Minute30:
      tint = startDate.time().minute() / 30;
      startDate.setTime(QTime(startDate.time().hour(), tint * 30, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(1800);
      break;
    case BarData::Minute60:
      startDate.setTime(QTime(startDate.time().hour(), 0, 0, 0));
      endDate = startDate;
      endDate = endDate.addSecs(3600);
      break;
    case BarData::DailyBar:
      startDate.setTime(QTime(0, 0, 0, 0));
      endDate = startDate;
      endDate = endDate.addDays(1);
      break;
    case BarData::WeeklyBar:
      startDate.setTime(QTime(0, 0, 0, 0));
      startDate = startDate.addDays(- startDate.date().dayOfWeek());
      endDate = startDate;
      endDate = endDate.addDays(7);
      break;
    case BarData::MonthlyBar:
      startDate.setTime(QTime(0, 0, 0, 0));
      startDate = startDate.addDays(- (startDate.date().day() - 1));
      endDate = startDate;
      endDate = endDate.addDays(endDate.date().daysInMonth());
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
  parms.getData(Indicator::IndicatorParmName, name);

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

  parms.setData(Indicator::IndicatorParmEnable, q.value(1).toInt());
  parms.setData(Indicator::IndicatorParmTabRow, q.value(2).toInt());
  parms.setData(Indicator::IndicatorParmDate, q.value(3).toInt());
  parms.setData(Indicator::IndicatorParmLog, q.value(4).toInt());

  s = q.value(5).toString();
  parms.setData(Indicator::IndicatorParmCommand, s);

  s = q.value(6).toString();
  parms.setData(Indicator::IndicatorParmPath, s);
}

void DataBase::setIndicator (Indicator &i)
{
  QString name, enable, tabRow, date, log, command, path;
  i.getData(Indicator::IndicatorParmName, name);
  i.getData(Indicator::IndicatorParmEnable, enable);
  i.getData(Indicator::IndicatorParmTabRow, tabRow);
  i.getData(Indicator::IndicatorParmDate, date);
  i.getData(Indicator::IndicatorParmLog, log);
  i.getData(Indicator::IndicatorParmCommand, command);
  i.getData(Indicator::IndicatorParmPath, path);

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "INSERT OR REPLACE INTO indicatorIndex VALUES (";
  s.append("'" + name + "'");
  s.append("," + enable);
  s.append("," + tabRow);
  s.append("," + date);
  s.append("," + log);
  s.append(",'" + command + "'");
  s.append(",'" + path + "'");
  s.append(")");
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

void DataBase::getChartObjects (QString &symbol, QString &indicator, QHash<QString, COSettings *> &list)
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
    COSettings *co = new COSettings(q.value(0).toString(), q.value(1).toString(), q.value(2).toString(), q.value(3).toString());
    QString ts = q.value(4).toString();
    co->parse(ts);
    list.insert(q.value(0).toString(), co);
  }
}

void DataBase::setChartObject (COSettings *co)
{
  QString id;
  co->getData(COSettings::COID, id);

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
    co->getSettings(settings);

    s = "UPDATE chartObjects SET settings='" + settings + "' WHERE id=" + id;
    q.exec(s);
    if (q.lastError().isValid())
      qDebug() << "DataBase::setChartObject: " << q.lastError().text();
  }
  else
  {
    QString symbol, indicator, type, settings;
    co->getData(COSettings::COSymbol, symbol);
    co->getData(COSettings::COIndicator, indicator);
    co->getData(COSettings::COCOType, type);
    co->getSettings(settings);

    s = "INSERT OR REPLACE INTO chartObjects VALUES (" + id + ",'" + symbol + "','" + indicator + "','" + type + "','" + settings + "')";
    q.exec(s);
    if (q.lastError().isValid())
      qDebug() << "DataBase::setChartObject: " << q.lastError().text();
  }
}

//***************************************************************************************
//**************** INDICATOR SETTINGS ***************************************************
//***************************************************************************************

void DataBase::getIndicatorSettings (IndicatorSettings &set)
{
  set.clear();

  QString indicator;
  set.getIndicator(indicator);

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT * FROM indicatorSettings WHERE indicator='" + indicator + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getIndicatorSettings:" << q.lastError().text();
    return;
  }

  if (q.next())
  {
    QString ts = q.value(1).toString(); 
    set.setName(ts);

    ts = q.value(2).toString(); 
    set.parse(ts);
  }
}

void DataBase::setIndicatorSettings (IndicatorSettings &set)
{
  QString indicator, ts, name;
  set.getIndicator(indicator);
  set.getString(ts);
  set.getName(name);
  
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "INSERT OR REPLACE INTO indicatorSettings VALUES (";
  s.append("'" + indicator + "'");
  s.append(",'" + name + "'");
  s.append(",'" + ts + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::setIndicatorSettings:" << q.lastError().text() << s;
}

