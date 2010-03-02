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
#include "PluginFactory.h"
#include "COPlugin.h"
#include "Config.h"

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
  s = "CREATE TABLE IF NOT EXISTS groupIndex (";
  s.append("name TEXT PRIMARY KEY");
  s.append(", parms TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::createGroupIndexTable: " << q.lastError().text();

  // create the indicator index table
  s = "CREATE TABLE IF NOT EXISTS indicatorIndex (";
  s.append("name TEXT PRIMARY KEY"); // 0
  s.append(", enable INT"); // 1
  s.append(", tabRow INT"); // 2
  s.append(", date INT"); // 3
  s.append(", log INT"); // 4
  s.append(", cus INT"); // 5
  s.append(", indicator TEXT"); // 6
  s.append(", settings TEXT"); // 7

  s.append(", t1 TEXT"); // 8
  s.append(", t2 TEXT"); // 9
  s.append(", t3 TEXT"); // 10
  s.append(", t4 TEXT"); // 11
  s.append(", t5 TEXT"); // 12

  s.append(", i1 INT"); // 13
  s.append(", i2 INT"); // 14
  s.append(", i3 INT"); // 15
  s.append(", i4 INT"); // 16
  s.append(", i5 INT"); // 17

  s.append(", d1 REAL"); // 18
  s.append(", d2 REAL"); // 19
  s.append(", d3 REAL"); // 20
  s.append(", d4 REAL"); // 21
  s.append(", d5 REAL"); // 22

  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::createIndicatorIndexTable: " << q.lastError().text();

  // create the chart object table
  s = "CREATE TABLE IF NOT EXISTS chartObjects (";
  s.append("id INT PRIMARY KEY"); // 0
  s.append(", symbol TEXT"); // 1
  s.append(", indicator TEXT"); // 2
  s.append(", plugin TEXT"); // 3
  
  s.append(", t1 TEXT"); // 4
  s.append(", t2 TEXT"); // 5
  s.append(", t3 TEXT"); // 6
  s.append(", t4 TEXT"); // 7
  s.append(", t5 TEXT"); // 8
  s.append(", t6 TEXT"); // 9
  s.append(", t7 TEXT"); // 10
  s.append(", t8 TEXT"); // 11
  s.append(", t9 TEXT"); // 12
  s.append(", t10 TEXT"); // 13
  
  s.append(", i1 INT"); // 14
  s.append(", i2 INT"); // 15
  s.append(", i3 INT"); // 16
  s.append(", i4 INT"); // 17
  s.append(", i5 INT"); // 18
  s.append(", i6 INT"); // 19
  s.append(", i7 INT"); // 20
  s.append(", i8 INT"); // 21
  s.append(", i9 INT"); // 22
  s.append(", i10 INT"); // 23
  
  s.append(", d1 REAL"); // 24
  s.append(", d2 REAL"); // 25
  s.append(", d3 REAL"); // 26
  s.append(", d4 REAL"); // 27
  s.append(", d5 REAL"); // 28
  s.append(", d6 REAL"); // 29
  s.append(", d7 REAL"); // 30
  s.append(", d8 REAL"); // 31
  s.append(", d9 REAL"); // 32
  s.append(", d10 REAL"); // 33
  
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::createChartObjectsTable: " << q.lastError().text();

  // create the script table
  s = "CREATE TABLE IF NOT EXISTS script (";
  s.append("name TEXT PRIMARY KEY UNIQUE"); // 0
  s.append(", command TEXT"); // 1
  s.append(", comment TEXT"); // 2
  s.append(", lastRun TEXT"); // 3
  
  s.append(", t1 TEXT"); // 4
  s.append(", t2 TEXT"); // 5
  s.append(", t3 TEXT"); // 6
  s.append(", t4 TEXT"); // 7
  s.append(", t5 TEXT"); // 8
  
  s.append(", i1 INT"); // 9
  s.append(", i2 INT"); // 10
  s.append(", i3 INT"); // 11
  s.append(", i4 INT"); // 12
  s.append(", i5 INT"); // 13

  s.append(", d1 REAL"); // 14
  s.append(", d2 REAL"); // 15
  s.append(", d3 REAL"); // 16
  s.append(", d4 REAL"); // 17
  s.append(", d5 REAL"); // 18

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

void DataBase::getIndicator (Indicator &i)
{
  QString name;
  i.getName(name);

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT enable,tabRow,date,log,cus,indicator,settings FROM indicatorIndex WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getIndicator: " << q.lastError().text();
    return;
  }

  if (! q.next())
    return;

  int pos = 0;
  i.setEnable(q.value(pos++).toInt());
  i.setTabRow(q.value(pos++).toInt());
  i.setDate(q.value(pos++).toInt());
  i.setLog(q.value(pos++).toInt());
  i.setCUS(q.value(pos++).toInt());
  s = q.value(pos++).toString();
  i.setIndicator(s);

  s = q.value(pos++).toString();
  Setting set;
  set.parse(s);
  i.setSettings(set);
}

void DataBase::setIndicator (Indicator &i)
{
  QString name, enable, tabRow, date, log, cus, indicator, settings;
  i.getName(name);
  enable = QString::number(i.getEnable());
  tabRow = QString::number(i.getTabRow());
  date = QString::number(i.getDate());
  log = QString::number(i.getLog());
  cus = QString::number(i.getCUS());
  i.getIndicator(indicator);

  Setting set;
  i.getSettings(set);
  set.getString(settings);

  transaction();

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "INSERT OR REPLACE INTO indicatorIndex (name,enable,tabRow,date,log,cus,indicator,settings) VALUES (";
  s.append("'" + name + "'");
  s.append("," + enable);
  s.append("," + tabRow);
  s.append("," + date);
  s.append("," + log);
  s.append("," + cus);
  s.append(",'" + indicator + "'");
  s.append(",'" + settings + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataBase::setIndicator: save index" << q.lastError().text();

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

void DataBase::getChartObjects (QString &symbol, QString &indicator, QHash<QString, ChartObject *> &list)
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT * FROM chartObjects WHERE symbol='" + symbol + "' AND indicator='" + indicator + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataBase::getChartObjects: " << q.lastError().text();
    return;
  }

  PluginFactory fac;
  Config config;
  QString path;
  config.getData(Config::COPluginPath, path);
  
  while (q.next())
  {
    s = q.value(3).toString();
    COPlugin *plug = fac.getCO(path, s);
    if (! plug)
    {
      qDebug() << "DataBase::getChartObjects: error loading plugin" << s;
      continue;
    }
    
    ChartObject *co = new ChartObject;
    plug->setSettings(co, q);
    list.insert(q.value(0).toString(), co);
  }
}

void DataBase::setChartObject (ChartObject *co)
{
  Config config;
  QString path;
  config.getData(Config::COPluginPath, path);

  QSqlQuery q(QSqlDatabase::database("data"));
  PluginFactory fac;
  QString s;
  co->getData(ChartObject::ParmPlugin, s);  
  COPlugin *plug = fac.getCO(path, s);
    
  plug->getSettings(co, s);
  
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
  QString s = "INSERT OR REPLACE INTO script (name,command,comment,lastRun) VALUES (";
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

