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
}

void GroupDataBase::init ()
{
  // create the group index table
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "CREATE TABLE IF NOT EXISTS groupIndex (";
  s.append("name TEXT PRIMARY KEY UNIQUE");
  s.append(", tableName TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "GroupDataBase::init: " << q.lastError().text();
}

void GroupDataBase::transaction ()
{
  QSqlDatabase db = QSqlDatabase::database("data");
  db.transaction();
}

void GroupDataBase::commit ()
{
  QSqlDatabase db = QSqlDatabase::database("data");
  db.commit();
}

void GroupDataBase::getAllGroupsList (QStringList &l)
{
  l.clear();
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT name FROM groupIndex";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GroupDataBase::getGroupsList: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());

  l.sort();
}

void GroupDataBase::getGroup (Group &group)
{
  // get the table from the group index
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT tableName FROM groupIndex WHERE name='" + group.getName() + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GroupDataBase::getGroupList: " << q.lastError().text();
    return;
  }

  QString table;
  if (q.next())
    table = q.value(0).toString();
  
  // get the group contents
  s = "SELECT exchange,symbol,name FROM " + table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GroupDataBase::getGroupList: " << q.lastError().text();
    return;
  }

  while (q.next())
  {
    int pos = 0;
    BarData *bd = new BarData;
    
    s = q.value(pos++).toString();
    bd->setExchange(s);
    
    s = q.value(pos++).toString();
    bd->setSymbol(s);
    
    s = q.value(pos++).toString();
    bd->setName(s);
    
    group.append(bd);
  }
}

void GroupDataBase::setGroup (Group &group)
{
  transaction();
  
  // get the table from the group index
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT tableName FROM groupIndex WHERE name='" + group.getName() + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GroupDataBase::setGroupList: " << q.lastError().text();
    return;
  }

  QString table;
  if (q.next())
  {
    table = q.value(0).toString();

    s = "DROP TABLE " + table;
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "GroupDataBase::setGroup: drop table" << q.lastError().text();
      return;
    }
  }
  else
  {
    // create new index entry
    table = "G" + group.getName();
    s = "INSERT OR REPLACE INTO groupIndex (name,tableName) VALUES (";
    s.append("'" + group.getName() + "'");
    s.append(",'" + table + "'");
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
      qDebug() << "GroupDataBase::setGroup: " << q.lastError().text();
  }

  // create a new one
  s = "CREATE TABLE IF NOT EXISTS " + table + " (";
  s.append("record INTEGER PRIMARY KEY AUTOINCREMENT");
  s.append(", exchange TEXT");
  s.append(", symbol TEXT");
  s.append(", name TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "GroupDataBase::setGroup: " << q.lastError().text();

  int loop;
  for (loop = 0; loop < group.count(); loop++)
  {
    BarData *bd = group.getItem(loop);
    
    // populate the table
    s = "INSERT OR REPLACE INTO " + table + " (exchange,symbol,name) VALUES (";
    s.append("'" + bd->getExchange() + "'");
    s.append(",'" + bd->getSymbol() + "'");
    s.append(",'" + bd->getName() + "'");
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
      qDebug() << "GroupDataBase::setGroup: " << q.lastError().text();
  }
  
  commit();
}

void GroupDataBase::deleteGroup (QString &n)
{
  // get the table name for the group
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT tableName FROM groupIndex WHERE name='" + n + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GroupDataBase::deleteGroup: " << q.lastError().text();
    return;
  }

  QString table;
  if (q.next())
    table = q.value(0).toString();
  else
  {
    qDebug() << "GroupDataBase::deleteGroup: group table not found" << n;
    return;
  }

  // delete the index entry
  s = "DELETE FROM groupIndex WHERE name='" + n + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "GroupDataBase::deleteGroup: " << q.lastError().text();
  
  // drop the table
  s = "DROP TABLE " + table;
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "GroupDataBase::setGroup: drop table" << q.lastError().text();
}

