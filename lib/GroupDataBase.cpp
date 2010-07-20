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

#include "GroupDataBase.h"

#include <QtDebug>
#include <QtSql>

GroupDataBase::GroupDataBase ()
{
  // create the group index table
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "CREATE TABLE IF NOT EXISTS groupIndex (";
  s.append("name TEXT PRIMARY KEY UNIQUE");
  s.append(", symbols TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "GroupDataBase::GroupDataBase: " << q.lastError().text();
}

void GroupDataBase::getAllGroupsList (QStringList &l)
{
  l.clear();
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT name FROM groupIndex ORDER BY name ASC";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GroupDataBase::getAllGroupsList: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());
}

void GroupDataBase::getGroup (Group &group)
{
  if (! group.getName().length())
    return;
  
  // get the table from the group index
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT symbols FROM groupIndex WHERE name='" + group.getName() + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GroupDataBase::getGroup: get tableNames" << q.lastError().text();
    return;
  }

  group.clear();
  
  if (q.next())
  {
    QStringList l = q.value(0).toString().split(",");

    int loop = 0;
    for (; loop < l.count(); loop++)
    {
      QStringList l2 = l.at(loop).split(":");
      if (l2.count() != 2)
        continue;
      
      BarData bd;
      bd.setExchange(l2[0]);
      bd.setSymbol(l2[1]);
      
      group.append(bd);
    }
  }
}

void GroupDataBase::setGroup (Group &group)
{
  if (! group.getName().length())
  {
    qDebug() << "GroupDataBase::setGroup: no group name";
    return;
  }
  
  transaction();
  
  // get the table from the group index
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT symbols FROM groupIndex WHERE name='" + group.getName() + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GroupDataBase::setGroup: tableNames" << q.lastError().text();
    return;
  }

  QStringList tl;
  int loop = 0;
  for (; loop < group.count(); loop++)
  {
    BarData bd;
    group.getItem(loop, bd);
    tl.append(bd.getExchange() + ":" + bd.getSymbol());
  }

  if (q.next())
  {
    // record exists, use UPDATE 
    s = "UPDATE groupIndex";
    s.append(" SET symbols='" + tl.join(",") + "'");
    s.append(" WHERE name='" + group.getName() + "'");
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "GroupDataBase::setGroup:" << q.lastError().text();
      return;
    }
  }
  else
  {
    s = "INSERT OR REPLACE INTO groupIndex (name,symbols) VALUES (";
    s.append("'" + group.getName() + "'");
    s.append(",'" + tl.join(",") + "'");
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "GroupDataBase::setGroup:" << q.lastError().text();
      return;
    }
  }

  commit();
}

void GroupDataBase::deleteGroup (QString &n)
{
  // get the table name for the group
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "DELETE FROM groupIndex WHERE name='" + n + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "GroupDataBase::deleteGroup: delete groupIndex" << q.lastError().text();
}

