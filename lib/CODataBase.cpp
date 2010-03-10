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

#include "CODataBase.h"
#include "PluginFactory.h"
#include "COPlugin.h"
#include "Config.h"

#include <QtDebug>
#include <QtSql>

CODataBase::CODataBase ()
{
}

void CODataBase::init ()
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "CREATE TABLE IF NOT EXISTS chartObjects (";
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
    qDebug() << "CODataBase::createChartObjectsTable: " << q.lastError().text();
}

void CODataBase::transaction ()
{
  QSqlDatabase db = QSqlDatabase::database("data");
  db.transaction();
}

void CODataBase::commit ()
{
  QSqlDatabase db = QSqlDatabase::database("data");
  db.commit();
}

void CODataBase::deleteChartObjects (QString &symbol)
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "DELETE FROM chartObjects WHERE symbol='" + symbol + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "CODataBase::deleteChartObjects: " << q.lastError().text();
    return;
  }
}

void CODataBase::deleteChartObjectsIndicator (QString &indicator)
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "DELETE FROM chartObjects WHERE indicator='" + indicator + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "CODataBase::deleteChartObjectsIndicator: " << q.lastError().text();
    return;
  }
}

void CODataBase::deleteChartObject (QString &id)
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "DELETE FROM chartObjects WHERE id=" + id;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "CODataBase::deleteChartObject: " << q.lastError().text();
    return;
  }
}

void CODataBase::getChartObjects (QString &symbol, QString &indicator, Indicator &i)
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT * FROM chartObjects WHERE symbol='" + symbol + "' AND indicator='" + indicator + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "CODataBase::getChartObjects: " << q.lastError().text();
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
      qDebug() << "CODataBase::getChartObjects: error loading plugin" << s;
      continue;
    }
    
    ChartObject *co = new ChartObject;
    plug->setSettings(co, q);
    i.addChartObject(co);
  }
}

void CODataBase::setChartObject (ChartObject *co)
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
    qDebug() << "CODataBase::setChartObject: " << q.lastError().text();
    return;
  }
}

