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
#include "QuoteIndexDataBase.h"
#include "DBPluginFactory.h"

#include <QtDebug>
#include <QtSql>

GroupDataBase::GroupDataBase ()
{
}

void GroupDataBase::init ()
{
  // create the group index table
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "CREATE TABLE IF NOT EXISTS groupIndex (";
  s.append("name TEXT PRIMARY KEY UNIQUE");
  s.append(", tableName TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "GroupDataBase::init: " << q.lastError().text();
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
  QString s = "SELECT tableName FROM groupIndex WHERE name='" + group.getName() + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GroupDataBase::getGroup: get tableName" << q.lastError().text();
    return;
  }

  QString table;
  if (q.next())
    table = q.value(0).toString();
  else
    return;
  
  // get the group contents
  s = "SELECT exchange,symbol FROM " + table + " ORDER BY symbol,exchange ASC";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GroupDataBase::getGroup: group contents" << q.lastError().text();
    qDebug() << s;
    return;
  }

  group.clear();

  QuoteIndexDataBase qidb;
  DBPluginFactory fac;
  
  while (q.next())
  {
    int pos = 0;
    BarData bd;
    
    s = q.value(pos++).toString();
    bd.setExchange(s);
    
    s = q.value(pos++).toString();
    bd.setSymbol(s);

    // get the security name
    if (! qidb.getIndexData(&bd))
    {
      DBPlugin *plug = fac.plugin(bd.getPlugin());
      if (plug)
      {
        QString name;
        s = "NAME";
        plug->detail(s, &bd, name);
        bd.setName(name);
      }
    }
    
    group.append(bd);
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
  QString s = "SELECT tableName FROM groupIndex WHERE name='" + group.getName() + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GroupDataBase::setGroup: tableName" << q.lastError().text();
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
      qDebug() << "GroupDataBase::setGroup: new index entry" << q.lastError().text();
  }

  // create a new one
  s = "CREATE TABLE IF NOT EXISTS " + table + " (";
  s.append("record INTEGER PRIMARY KEY AUTOINCREMENT");
  s.append(", exchange TEXT");
  s.append(", symbol TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "GroupDataBase::setGroup: create table" << q.lastError().text();

  int loop;
  for (loop = 0; loop < group.count(); loop++)
  {
    BarData bd;
    group.getItem(loop, bd);
    
    // populate the table
    s = "INSERT OR REPLACE INTO " + table + " (exchange,symbol) VALUES (";
    s.append("'" + bd.getExchange() + "'");
    s.append(",'" + bd.getSymbol() + "'");
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
      qDebug() << "GroupDataBase::setGroup: populate table" << q.lastError().text();
  }
  
  commit();
}

void GroupDataBase::deleteGroup (QString &n)
{
  // get the table name for the group
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT tableName FROM groupIndex WHERE name='" + n + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GroupDataBase::deleteGroup: get tableName" << q.lastError().text();
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
    qDebug() << "GroupDataBase::deleteGroup: delete groupIndex" << q.lastError().text();
  
  // drop the table
  s = "DROP TABLE " + table;
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "GroupDataBase::setGroup: drop table" << q.lastError().text();
}

