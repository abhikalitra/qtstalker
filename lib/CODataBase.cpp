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

#include "CODataBase.h"
#include "COPlugin.h"
#include "COFactory.h"

#include <QtDebug>
#include <QtSql>

CODataBase::CODataBase ()
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "CREATE TABLE IF NOT EXISTS chartObjects (";
  s.append("id INT PRIMARY KEY"); // 0
  s.append(", exchange TEXT"); // 1
  s.append(", symbol TEXT"); // 2
  s.append(", indicator TEXT"); // 3
  s.append(", plugin TEXT"); // 4

  s.append(", t1 TEXT"); // 5
  s.append(", t2 TEXT"); // 6
  s.append(", t3 TEXT"); // 7
  s.append(", t4 TEXT"); // 8
  s.append(", t5 TEXT"); // 9
  s.append(", t6 TEXT"); // 10
  s.append(", t7 TEXT"); // 11
  s.append(", t8 TEXT"); // 12
  s.append(", t9 TEXT"); // 13
  s.append(", t10 TEXT"); // 14

  s.append(", i1 INT"); // 15
  s.append(", i2 INT"); // 16
  s.append(", i3 INT"); // 17
  s.append(", i4 INT"); // 18
  s.append(", i5 INT"); // 19
  s.append(", i6 INT"); // 20
  s.append(", i7 INT"); // 21
  s.append(", i8 INT"); // 22
  s.append(", i9 INT"); // 23
  s.append(", i10 INT"); // 24

  s.append(", d1 REAL"); // 25
  s.append(", d2 REAL"); // 26
  s.append(", d3 REAL"); // 27
  s.append(", d4 REAL"); // 28
  s.append(", d5 REAL"); // 29
  s.append(", d6 REAL"); // 30
  s.append(", d7 REAL"); // 31
  s.append(", d8 REAL"); // 32
  s.append(", d9 REAL"); // 33
  s.append(", d10 REAL"); // 34

  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "CODataBase::createChartObjectsTable: " << q.lastError().text();
}

void CODataBase::deleteChartObjects (BarData *bd)
{
  transaction();
  
  QString s = "DELETE FROM chartObjects";
  s.append(" WHERE exchange='" + bd->getExchange() + "'");
  s.append(" AND symbol='" + bd->getSymbol() + "'");
  if (command(s, QString("CODataBase::deleteChartObjects:")))
    return;
  
  commit();
}

void CODataBase::deleteChartObjectsIndicator (QString &indicator)
{
  transaction();
  
  QString s = "DELETE FROM chartObjects WHERE indicator='" + indicator + "'";
  if (command(s, QString("CODataBase::deleteChartObjectsIndicator:")))
    return;
  
  commit();
}

void CODataBase::deleteChartObject (int id)
{
  transaction();
  
  QString s = "DELETE FROM chartObjects WHERE id=" + QString::number(id);
  if (command(s, QString("CODataBase::deleteChartObject:")))
    return;
  
  commit();
}

void CODataBase::getChartObjects (BarData *bd, QString &indicator, Indicator &i)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT * FROM chartObjects";
  s.append(" WHERE exchange='" + bd->getExchange() + "'");
  s.append(" AND symbol='" + bd->getSymbol() + "'");
  s.append(" AND indicator='" + indicator + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "CODataBase::getChartObjects: " << q.lastError().text();
    return;
  }

  COFactory fac;
  while (q.next())
  {
    s = q.value(4).toString();
    COPlugin *co = fac.getCO(s);
    if (! co)
    {
      qDebug() << "CODataBase::getChartObjects: error loading plugin" << s;
      continue;
    }
    
    co->load(q);
    i.addChartObject(co);
  }
}

void CODataBase::setChartObject (QString &co)
{
  transaction();
  
  if (command(co, QString("CODataBase::setChartObject:")))
    return;
  
  commit();
}

int CODataBase::renameSymbol (BarData *obd, BarData *nbd)
{
  transaction();

  // update any chart objects with new symbol name
  QString s = "UPDATE chartObjects";
  s.append(" SET symbol='" + nbd->getSymbol() + "'");
  s.append(", exchange='" + nbd->getExchange() + "'");
  s.append(" WHERE symbol='" + obd->getSymbol() + "'");
  s.append(" AND exchange='" + obd->getExchange() + "'");
  if (command(s, QString("CODataBase::renameSymbol")))
    return 1;
  
  commit();
  
  return 0;
}

