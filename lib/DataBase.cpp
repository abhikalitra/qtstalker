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
#include "Setting.h"
#include "COFactory.h"

#include <QtDebug>
#include <QtSql>
#include <QVariant>


DataBase::DataBase ()
{
}

void DataBase::init ()
{
  QString s = QDir::homePath() + "/.qtstalker/data.sqlite";
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

  // create the group index table
  QSqlQuery q(db);
  s = "CREATE TABLE IF NOT EXISTS groupIndex (name TEXT PRIMARY KEY, parms TEXT)";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::createGroupIndexTable: " << q.lastError().text();

  // create the indicator index table
  s = "CREATE TABLE IF NOT EXISTS indicatorIndex (";
  s.append("name TEXT PRIMARY KEY");
  s.append(", enable INT");
  s.append(", tabRow INT");
  s.append(", date INT");
  s.append(", log INT");
  s.append(", cus INT");
  s.append(", indicator TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::createIndicatorIndexTable: " << q.lastError().text();

  // create the chart object table
  s = "CREATE TABLE IF NOT EXISTS chartObjects (";
  s.append("id INT PRIMARY KEY");
  s.append(", symbol TEXT");
  s.append(", indicator TEXT");
  s.append(", type INT");
  s.append(", color TEXT");
  s.append(", date TEXT");
  s.append(", date2 TEXT");
  s.append(", price REAL");
  s.append(", price2 REAL");
  s.append(", high REAL");
  s.append(", low REAL");
  s.append(", font TEXT");
  s.append(", label TEXT");
  s.append(", extend INT");
  s.append(", line1 REAL");
  s.append(", line2 REAL");
  s.append(", line3 REAL");
  s.append(", line4 REAL");
  s.append(", line5 REAL");
  s.append(", line6 REAL");
  s.append(", barField TEXT");
  s.append(", useBar INT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::createChartObjectsTable: " << q.lastError().text();

  // create the indicator settings table
  s = "CREATE TABLE IF NOT EXISTS indicatorSettings (";
  s.append("indicator TEXT PRIMARY KEY UNIQUE");
  s.append(", name TEXT");
  s.append(", settings TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::createIndicatorSettingsTable: " << q.lastError().text();

  // create the script table
  s = "CREATE TABLE IF NOT EXISTS script (";
  s.append("name TEXT PRIMARY KEY UNIQUE");
  s.append(", command TEXT");
  s.append(", comment TEXT");
  s.append(", lastRun TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::createScriptTable: " << q.lastError().text();
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
  parms.setCUS(q.value(5).toInt());
  s = q.value(6).toString();
  parms.setIndicator(s);

  // load the parms from the indicator
  s = "SELECT * FROM INDICATOR_PARMS_" + name;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getIndicator: " << q.lastError().text();
    return;
  }

  Setting set;
  while (q.next())
  {
    QString k = q.value(0).toString();
    QString d = q.value(1).toString();
    set.setData(k, d);
  }

  parms.setSettings(set);
}

void DataBase::setIndicator (Indicator &i)
{
  QString name, enable, tabRow, date, log, cus, indicator;
  i.getName(name);
  enable = QString::number(i.getEnable());
  tabRow = QString::number(i.getTabRow());
  date = QString::number(i.getDate());
  log = QString::number(i.getLog());
  cus = QString::number(i.getCUS());
  i.getIndicator(indicator);

  transaction();

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "INSERT OR REPLACE INTO indicatorIndex VALUES (";
  s.append("'" + name + "'");
  s.append("," + enable);
  s.append("," + tabRow);
  s.append("," + date);
  s.append("," + log);
  s.append("," + cus);
  s.append(",'" + indicator + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::setIndicator: save index" << q.lastError().text();

  // remove the parms table
  QString table = "INDICATOR_PARMS_" + name;
  s = "DROP TABLE " + table;
  q.exec(s);

  // re-create the parms table
  s = "CREATE TABLE IF NOT EXISTS " + table + " (";
  s.append("key TEXT");
  s.append(", data TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::setIndicator: create parms table" << q.lastError().text();

  Setting set;
  i.getSettings(set);
  QStringList keyList;
  set.getKeyList(keyList);
  int loop;
  for (loop = 0; loop < keyList.count(); loop++)
  {
    QString d;
    set.getData(keyList[loop], d);

    s = "INSERT OR REPLACE INTO " + table + " VALUES (";
    s.append("'" + keyList[loop] + "'");
    s.append(",'" + d + "'");
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
      qDebug() << "DataBase::setIndicator: save parm" << q.lastError().text();
  }

  commit();
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

  s = "DROP TABLE INDICATOR_PARMS_" + name;
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::deleteIndicator: drop parms table" << q.lastError().text();
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

void DataBase::setIndicatorEnable (Indicator &i)
{
  QString name, enable;
  i.getName(name);
  enable = QString::number(i.getEnable());

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "UPDATE indicatorIndex SET enable=" + enable + " WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::setIndicatorEnable:" << q.lastError().text();
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

void DataBase::getChartObjects (QString &symbol, QString &indicator, QHash<QString, BaseCO *> &list)
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT * FROM chartObjects WHERE symbol='" + symbol + "' AND indicator='" + indicator + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getChartObjects: " << q.lastError().text();
    return;
  }

  while (q.next())
  {
    Setting set;
    int loop;
    for (loop = (int) BaseCO::ParmID; loop <= BaseCO::ParmUseBar; loop++)
    {
      QString k = QString::number(loop);
      QString d = q.value(loop).toString();
      set.setData(k, d);
    }

    COFactory fac;
    BaseCO *co = fac.getCO(q.value(1).toInt());
    if (! co)
    {
      qDebug() << "DataBase::getChartObjects: error creating co";
      continue;
    }

    co->setSettings(set);
    list.insert(q.value(1).toString(), co);
  }
}

void DataBase::setChartObject (BaseCO *co)
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s;
  co->getSettings(s);
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::setChartObject: " << q.lastError().text();
    return;
  }
}

/********************************************************************************/
/********************* script functions *****************************************/
/********************************************************************************/

void DataBase::getScripts (QStringList &l)
{
  l.clear();
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT name FROM script";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getScripts: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());

  l.sort();
}

void DataBase::getScript (Script &script)
{
  QString name;
  script.getName(name);

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT command,comment,lastRun FROM script WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getScript: " << q.lastError().text();
    return;
  }

  if (q.next())
  {
    s = q.value(0).toString();
    script.setCommand(s);

    s = q.value(1).toString();
    script.setComment(s);

    QDateTime dt = q.value(2).toDateTime();
    script.setLastRun(dt);
  }
}

void DataBase::setScript (Script &script)
{
  QString name, command, comment, lastRun;
  script.getName(name);
  script.getCommand(command);
  script.getComment(comment);
  QDateTime dt;
  script.getLastRun(dt);
  lastRun = dt.toString(Qt::ISODate);

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "INSERT OR REPLACE INTO script VALUES (";
  s.append("'" + name + "'");
  s.append(",'" + command + "'");
  s.append(",'" + comment + "'");
  s.append(",'" + lastRun + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::setScript: " << q.lastError().text();
}

void DataBase::deleteScript (Script &script)
{
  QString name;
  script.getName(name);

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "DELETE FROM script WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::deleteScript: " << q.lastError().text();
}

void DataBase::getScriptSearch (QString &pattern, QStringList &list)
{
  list.clear();

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT name FROM script WHERE name LIKE '" + pattern + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getScriptSearch: " << q.lastError().text();
    return;
  }

  while (q.next())
    list.append(q.value(0).toString());

  list.sort();
}

