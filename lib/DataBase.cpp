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
  QDateTime firstDate, lastDate;
  data->getSymbol(symbol);

  // get the index details
  QString s = "SELECT name,firstDate,lastDate,format FROM symbolIndex WHERE symbol='" + symbol + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getChart: " << q.lastError().text();
    return;
  }

  if (q.next())
  {
    ts = q.value(0).toString();
    data->setName(ts);

    ts = q.value(1).toString();
    firstDate = QDateTime::fromString(ts, "yyyyMMddHHmmsszzz");

    ts = q.value(2).toString();
    lastDate = QDateTime::fromString(ts, "yyyyMMddHHmmsszzz");

    format = q.value(3).toString();
  }

  QDateTime sdate = lastDate;
  QDateTime edate = sdate;

  do
  {
    edate = sdate;
    data->getDateOffset(sdate);
    if (edate <= firstDate)
      break;

    ts = sdate.toString("yyyyMMddHHmmsszzz");
    ts2 = edate.toString("yyyyMMddHHmmsszzz");

    // get some quotes
    s = "SELECT date,data FROM " + symbol + " WHERE date >= " + ts + " AND date <= " + ts2;
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "DataBase::getChart: " << q.lastError().text();
      break;
    }

    if (! q.last())
    {
      qDebug() << "DataBase::getChart: no records found" << q.lastError().text();
      break;
    }

    do
    {
      Bar bar;
      s = q.value(0).toString();
      QDateTime dt = QDateTime::fromString(s, "yyyyMMddHHmmsszzz");
      bar.setDate(dt);

      s = q.value(1).toString();
      bar.parse(format, s);

      data->prepend(bar);
    } while (q.previous() && data->count() < data->getBarsRequested());
  } while (data->count() < data->getBarsRequested());
}

/********************************************************************************/
/********************* group functions *******************************************/
/********************************************************************************/

int DataBase::createGroup (QString &n)
{
  int rc = TRUE;
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT name FROM groupIndex WHERE name='" + n + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::createGroup: " << q.lastError().text();
    return rc;
  }

  if (! q.next())
  {
    s = "INSERT OR REPLACE INTO groupIndex (name) VALUES('" + n + "')";
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "DataBase::createGroup: " << q.lastError().text();
      return rc;
    }

    s = "CREATE TABLE group_" + n + " (chart VARCHAR(25) PRIMARY KEY)";
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "DataBase::createGroup: " << q.lastError().text();
      return rc;
    }

    rc = FALSE;
  }

  return rc;
}

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
  QString s = "SELECT chart FROM group_" + n;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getGroupList: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());

  l.sort();
}

void DataBase::setGroupList (QString &n, QStringList &l, bool flag)
{
  QSqlQuery q(QSqlDatabase::database("data"));

  if (flag)
  {
    QString s = "DELETE FROM group_" + n;
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "DataBase::setGroupList: " << q.lastError().text();
      return;
    }
  }

  QString s = "BEGIN TRANSACTION";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::setGroupList:begin transaction: " << q.lastError().text();
    return;
  }

  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    s = "INSERT OR REPLACE INTO group_" + n + " (chart) VALUES ('" + l[loop] + "')";
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "DataBase::setGroupList: " << q.lastError().text();
      continue;
    }
  }

  s = "COMMIT";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::setGroupList:commit: " << q.lastError().text();
}

void DataBase::deleteGroup (QString &n)
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "DELETE FROM groupIndex WHERE name='" + n + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::deleteGroup: " << q.lastError().text();
    return;
  }

  s = "DROP TABLE group_" + n;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::deleteGroup: " << q.lastError().text();
    return;
  }
}

/********************************************************************************/
/********************* indicator functions *******************************************/
/********************************************************************************/

int DataBase::createIndicator (QString &n)
{
  int rc = TRUE;
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "INSERT INTO indicatorIndex (name) VALUES('" + n + "')";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::createIndicator: " << q.lastError().text();
    return rc;
  }

  s = "CREATE TABLE indicator_" + n + " (id INT AUTO INCREMENT PRIMARY KEY, parms VARCHAR(500))";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::createIndicator: " << q.lastError().text();
    return rc;
  }

  rc = FALSE;
  return rc;
}

void DataBase::getIndicator (QString &name, QList<IndicatorParms> &parms)
{
  parms.clear();

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

  s = "SELECT parms FROM indicator_" + name;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getIndicator: " << q.lastError().text();
    return;
  }

  while (q.next())
  {
    IndicatorParms p;
    QString s2 = q.value(0).toString();
    p.parse(s2);
    parms.append(p);
  }
}

void DataBase::setIndicator (QString &name, QList<IndicatorParms> &parms)
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "DELETE FROM indicator_" + name;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::setIndicator:delete records: " << q.lastError().text();
    return;
  }

  s = "BEGIN TRANSACTION";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::setIndicator:begin transaction: " << q.lastError().text();
    return;
  }

  int loop;
  for (loop = 0; loop < parms.count(); loop++)
  {
    IndicatorParms p = parms.at(loop);
    QString s2;
    p.getString(s2);
    s = "INSERT INTO indicator_" + name + " (parms) VALUES ('" + s2 + "')";
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "DataBase::setIndicator: " << q.lastError().text();
      return;
    }
  }

  s = "COMMIT";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::setIndicator:commit: " << q.lastError().text();
}

void DataBase::deleteIndicator (QString &name)
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "DELETE FROM indicatorIndex WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::deleteIndicatorIndex: " << q.lastError().text();
    return;
  }

  s = "DROP TABLE indicator_" + name;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::deleteIndicatorTable: " << q.lastError().text();
    return;
  }
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

void DataBase::getIndicatorIndex (QString &name, IndicatorIndex &parm)
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT * FROM indicatorIndex WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getIndicatorIndex: " << q.lastError().text();
    return;
  }

  if (q.next())
  {
    QString s = q.value(0).toString();
    parm.setName(s);
    parm.setEnable(q.value(1).toInt());
    parm.setTabRow(q.value(2).toInt());
    parm.setDate(q.value(3).toInt());
    parm.setLog(q.value(4).toInt());
  }
}

void DataBase::setIndicatorIndex (QString &name, IndicatorIndex &parm)
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "UPDATE indicatorIndex SET enable=" + QString::number(parm.getEnable()) + ", tabRow=" + QString::number(parm.getTabRow());
  s.append(", date="  + QString::number(parm.getDate()) + ", log=" + QString::number(parm.getLog()) + " WHERE name='" + name + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::setIndicatorIndex: " << q.lastError().text();
    return;
  }
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


