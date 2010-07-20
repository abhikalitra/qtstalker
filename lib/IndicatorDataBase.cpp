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

#include "IndicatorDataBase.h"
#include "CODataBase.h"

IndicatorDataBase::IndicatorDataBase ()
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "CREATE TABLE IF NOT EXISTS indicatorIndex (";
  s.append("name TEXT PRIMARY KEY"); // 0
  s.append(", enable INT"); // 1
  s.append(", tabRow INT"); // 2
  s.append(", column INT"); // 3
  s.append(", date INT"); // 4
  s.append(", log INT"); // 5
  s.append(", cus INT"); // 6
  s.append(", indicator TEXT"); // 7
  s.append(", settings TEXT"); // 8

  s.append(", t1 TEXT"); // 9
  s.append(", t2 TEXT"); // 10
  s.append(", t3 TEXT"); // 11
  s.append(", t4 TEXT"); // 12
  s.append(", t5 TEXT"); // 13

  s.append(", i1 INT"); // 14
  s.append(", i2 INT"); // 15
  s.append(", i3 INT"); // 16
  s.append(", i4 INT"); // 17
  s.append(", i5 INT"); // 18

  s.append(", d1 REAL"); // 19
  s.append(", d2 REAL"); // 20
  s.append(", d3 REAL"); // 21
  s.append(", d4 REAL"); // 22
  s.append(", d5 REAL"); // 23

  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "IndicatorDataBase::IndicatorDataBase: " << q.lastError().text();
}

void IndicatorDataBase::getIndicator (Indicator &i)
{
  QString name = i.name();

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT enable,tabRow,column,date,log,cus,indicator,settings FROM indicatorIndex WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "IndicatorDataBase::getIndicator: " << q.lastError().text();
    return;
  }

  if (! q.next())
    return;

  int pos = 0;
  i.setEnable(q.value(pos++).toInt());
  i.setTabRow(q.value(pos++).toInt());
  i.setColumn(q.value(pos++).toInt());
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

void IndicatorDataBase::setIndicator (Indicator &i)
{
  QString name = i.name();
  QString enable = QString::number(i.enable());
  QString tabRow = QString::number(i.tabRow());
  QString column = QString::number(i.column());
  QString date = QString::number(i.date());
  QString log = QString::number(i.getLog());
  QString cus = QString::number(i.CUS());
  QString indicator = i.indicator();

  Setting set = i.settings();
  QString settings;
  set.getString(settings);

  transaction();

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "INSERT OR REPLACE INTO indicatorIndex (name,enable,tabRow,column,date,log,cus,indicator,settings) VALUES (";
  s.append("'" + name + "'");
  s.append("," + enable);
  s.append("," + tabRow);
  s.append("," + column);
  s.append("," + date);
  s.append("," + log);
  s.append("," + cus);
  s.append(",'" + indicator + "'");
  s.append(",'" + settings + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "IndicatorDataBase::setIndicator: save index" << q.lastError().text();

  commit();
}

void IndicatorDataBase::deleteIndicator (QString &name)
{
  // delete any chart objects tied to this indicator before we delete it
  CODataBase codb;
  codb.deleteChartObjectsIndicator(name);

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "DELETE FROM indicatorIndex WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "IndicatorDataBase::deleteIndicatorIndex: " << q.lastError().text();
}

void IndicatorDataBase::getIndicatorList (QStringList &l)
{
  l.clear();
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT name FROM indicatorIndex";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "IndicatorDataBase::getIndicatorList: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());

  l.sort();
}

void IndicatorDataBase::getActiveIndicatorList (QStringList &l)
{
  l.clear();

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT name FROM indicatorIndex WHERE enable=1";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "IndicatorDataBase::getActiveIndicatorList: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());

  l.sort();
}

void IndicatorDataBase::getSearchIndicatorList (QString &pattern, QStringList &list)
{
  list.clear();

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT name FROM indicatorIndex WHERE name LIKE '" + pattern + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "IndicatorDataBase::getSearchIndicatorList: " << q.lastError().text();
    return;
  }

  while (q.next())
    list.append(q.value(0).toString());

  list.sort();
}

void IndicatorDataBase::setIndicatorEnable (Indicator &i)
{
  QString name = i.name();
  QString enable = QString::number(i.enable());

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "UPDATE indicatorIndex SET enable=" + enable + " WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "IndicatorDataBase::setIndicatorEnable:" << q.lastError().text();
}

