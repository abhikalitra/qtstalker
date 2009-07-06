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

#include <QtDebug>
#include <QtSql>


DataBase::DataBase ()
{
}

/********************************************************************************/
/********************* chart functions *******************************************/
/********************************************************************************/

void DataBase::getAllChartsList (QStringList &l)
{
  l.clear();
  QSqlQuery q(QSqlDatabase::database("quotes"));
  QString s = "SELECT symbol FROM symbolIndex";
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

void DataBase::getChart (BarData *data)
{
  QSqlQuery q(QSqlDatabase::database("quotes"));

  QString symbol, ts, ts2, format;
  data->getSymbol(symbol);

  // get the index details
  QString s = "SELECT name,format FROM symbolIndex WHERE symbol='" + symbol + "'";
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

    format = q.value(1).toString();
  }
  else
  {
    qDebug() << "DataBase::getChart: no symbolIndex record found";
    return;
  }

  QStringList formatList = format.split(",");

  QDateTime firstDate;
  getFirstDate(firstDate, symbol);

  QDateTime lastDate;
  getLastDate(lastDate, symbol);

  QDateTime sdate = lastDate;
  QDateTime edate = lastDate;

  setStartEndDates(sdate, edate, data->getBarLength());
  if (sdate < firstDate)
    return;

  while (1)
  {
    QString sd = sdate.toString("yyyyMMddHHmmsszzz");
    QString ed = edate.toString("yyyyMMddHHmmsszzz");

    QString s = "SELECT * FROM " + symbol + " WHERE date >=" + sd + " AND date <" + ed;
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
      QDateTime dt = QDateTime::fromString(q.value(0).toString(), "yyyyMMddHHmmsszzz");
      bar->setDate(dt);

      // check if this is first data for bar construction, we need to set the open
      if (! openFlag)
      {
        int loop;
        for (loop = 0; loop < formatList.count(); loop++)
        {
          s = q.value(loop + 1).toString();
          bar->setData(formatList[loop], s);
        }
        openFlag = TRUE;
        continue;
      }

      int i = formatList.indexOf("High");
      if (i)
      {
        double h = q.value(i + 1).toDouble();
        if (h > bar->getHigh())
          bar->setHigh(h);
      }

      i = formatList.indexOf("Low");
      if (i)
      {
        double l = q.value(i + 1).toDouble();
        if (l < bar->getLow())
          bar->setLow(l);
      }

      i = formatList.indexOf("Close");
      if (i)
      {
        double c = q.value(i + 1).toDouble();
        bar->setClose(c);
      }

      i = formatList.indexOf("Volume");
      if (i)
      {
        double v = q.value(i + 1).toDouble();
        double v2 = bar->getVolume();
        bar->setVolume(v + v2);
      }

      i = formatList.indexOf("OI");
      if (i)
      {
        double c = q.value(i + 1).toDouble();
        bar->setOI(c);
      }
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
  QSqlQuery q(QSqlDatabase::database("quotes"));
  QString s = "SELECT date FROM " + symbol + " ORDER BY date ASC LIMIT 1";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getFirstDate:" << q.lastError().text();
    return;
  }

  if (q.next())
    date = QDateTime::fromString(q.value(0).toString(), "yyyyMMddHHmmsszzz");
}

void DataBase::getLastDate (QDateTime &date, QString &symbol)
{
  QSqlQuery q(QSqlDatabase::database("quotes"));
  QString s = "SELECT date FROM " + symbol + " ORDER BY date DESC LIMIT 1";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getLastDate:" << q.lastError().text();
    return;
  }

  if (q.next())
    date = QDateTime::fromString(q.value(0).toString(), "yyyyMMddHHmmsszzz");
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

  s = q.value(1).toString();
  parms.setType(s);
  parms.setEnable(q.value(2).toInt());
  parms.setTabRow(q.value(3).toInt());
  parms.setDate(q.value(4).toInt());
  parms.setLog(q.value(5).toInt());
  parms.setParms(q.value(6).toString());
}

void DataBase::setIndicator (Indicator &i)
{
  QString name, enable, tabRow, date, log, parms, type;
  i.getName(name);
  enable = QString::number(i.getEnable());
  tabRow = QString::number(i.getTabRow());
  date = QString::number(i.getDate());
  log = QString::number(i.getLog());
  i.getParms(parms);
  i.getType(type);

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "INSERT OR REPLACE INTO indicatorIndex VALUES ('" + name + "','" + type + "'," + enable + "," + tabRow + "," + date + "," + log + ",'" + parms + "')";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::setIndicator: " << q.lastError().text();
}

void DataBase::deleteIndicator (QString &name)
{
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
  QString s = "SELECT name FROM indicatorIndex WHERE type='Plot'";
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

void DataBase::dumpIndicators ()
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT * FROM indicatorIndex";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::dumpIndicators: " << q.lastError().text();
    return;
  }

  while (q.next())
  {
    qDebug() << q.value(0).toString() << q.value(1).toString() << q.value(2).toString() << q.value(3).toString()
             << q.value(4).toString() << q.value(5).toString() << q.value(6).toString();
  }
}

/********************************************************************************/
/****************************** scanner functions *********************************/
/********************************************************************************/

void DataBase::deleteScanner (QString &name)
{
  // delete indicator linked to the scanner
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT parms FROM scanners WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::deleteScanner: " << q.lastError().text();
    return;
  }

  if (q.next())
  {
    s = q.value(0).toString();
    deleteIndicator(s);
  }

  // now delete the scannner
  s = "DELETE FROM scanners WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::deleteScanner: " << q.lastError().text();
    return;
  }
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
    i.setName(s);
    getIndicator(i);
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
  setIndicator(i);
  QString parms;
  i.getName(parms);

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
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "DELETE FROM testers WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::deleteTest: " << q.lastError().text();
    return;
  }
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


