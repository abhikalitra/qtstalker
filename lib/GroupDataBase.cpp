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
#include <QDir>

GroupDataBase::GroupDataBase ()
{
  init();
}

void GroupDataBase::init ()
{
  _db = QSqlDatabase::database("data");
  if (! _db.isOpen())
  {
    QString s = QDir::homePath() + "/.qtstalker/data.sqlite";
    _db = QSqlDatabase::addDatabase("QSQLITE", "data");
    _db.setHostName("QtStalker");
    _db.setDatabaseName(s);
    _db.setUserName("QtStalker");
    _db.setPassword("QtStalker");
    if (! _db.open())
      qDebug() << "GroupDataBase::init: open" << _db.lastError().text();
  }

  QSqlQuery q(_db);
  QString s = "CREATE TABLE IF NOT EXISTS groupIndex (";
  s.append("name TEXT PRIMARY KEY UNIQUE");
  s.append(", tableName TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GroupDataBase::init: create table" << _db.lastError().text();
    qDebug() << s;
  }
}

int GroupDataBase::load (QString name, QStringList &l)
{
  l.clear();
  
  QString table;
  if (getTable(name, table))
    return 1;

  if (table.isEmpty())
  {
    qDebug() << "GroupDataBase::load: group not found" << name;
    return 1;
  }
  
  QSqlQuery q(_db);
  QString s = "SELECT symbol FROM " + table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GroupDataBase::load:" << q.lastError().text();
    return 1;
  }

  while (q.next())
    l << q.value(0).toString();

  return 0;
}

int GroupDataBase::save (QString name, QStringList &l)
{
  QString table;
  if (getTable(name, table))
    return 1;

  if (table.isEmpty())
  {
    if (newGroup(name, table))
      return 1;
  }

  QSqlQuery q(_db);
  _db.transaction();

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QString s = "INSERT OR REPLACE INTO " + table + " (symbol) VALUES (";
    s.append("'" + l.at(loop) + "'");
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "GroupDataBase::save:" << q.lastError().text();
      return 1;
    }
  }

  _db.commit();

  return 0;
}

int GroupDataBase::saveAll (QString name, QStringList &l)
{
  QString table;
  if (getTable(name, table))
    return 1;

  if (table.isEmpty())
  {
    if (newGroup(name, table))
      return 1;
  }
  else
  {
    QSqlQuery q(_db);
    _db.transaction();

    // drop old table
    QString s = "DROP TABLE " + table;
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "GroupDataBase::saveAll:" << q.lastError().text();
      return 1;
    }
    
    s = "CREATE TABLE IF NOT EXISTS " + table + " (";
    s.append("symbol TEXT PRIMARY KEY UNIQUE");
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "GroupDataBase::newGroup:" << q.lastError().text();
      return 1;
    }

    _db.commit();
  }

  QSqlQuery q(_db);
  _db.transaction();

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QString s = "INSERT OR REPLACE INTO " + table + " (symbol) VALUES (";
    s.append("'" + l.at(loop) + "'");
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "GroupDataBase::save:" << q.lastError().text();
      return 1;
    }
  }

  _db.commit();

  return 0;
}

int GroupDataBase::deleteGroup (QStringList &l)
{
  QSqlQuery q(_db);
  _db.transaction();

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QString table;
    QString name = l.at(loop);
    if (getTable(name, table))
    {
      qDebug() << "GroupDataBase::deleteGroup: group not found" << name;
      continue;
    }
    
    QString s = "DELETE FROM groupIndex WHERE name='" + name + "'";
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "GroupDataBase::deleteGroup:" << q.lastError().text();
      continue;
    }

    s = "DROP TABLE " + table;
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "GroupDataBase::deleteGroup:" << q.lastError().text();
      continue;
    }
  }

  _db.commit();

  return 0;
}

int GroupDataBase::groups (QStringList &l)
{
  l.clear();

  QSqlQuery q(_db);
  QString s = "SELECT name FROM groupIndex ORDER BY name ASC";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GroupDataBase::groups: " << q.lastError().text();
    return 1;
  }

  while (q.next())
    l << q.value(0).toString();

  return 0;
}

int GroupDataBase::newGroup (QString &name, QString &table)
{
  QSqlQuery q(_db);

  // create the symbol
  // we use the maximum record column and add 1 to it to generate a new table name
  QString s = "SELECT max(rowid) FROM groupIndex";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GroupDataBase::newGroup: get record max " << q.lastError().text();
    return 1;
  }

  table = "G1";
  if (q.next())
    table = "G" + QString::number(q.value(0).toInt() + 1);

  _db.transaction();
  
  // add new symbol entry into the symbolIndex table
  s = "INSERT OR REPLACE INTO groupIndex (name,tableName) VALUES(";
  s.append("'" + name + "'");
  s.append(",'" + table + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GroupDataBase::newGroup:" << q.lastError().text();
    return 1;
  }

  s = "CREATE TABLE IF NOT EXISTS " + table + " (";
  s.append("symbol TEXT PRIMARY KEY UNIQUE");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GroupDataBase::newGroup:" << q.lastError().text();
    return 1;
  }

  _db.commit();
  
  return 0;
}

int GroupDataBase::getTable (QString &name, QString &table)
{
  table.clear();
  
  QSqlQuery q(_db);
  QString s = "SELECT tableName FROM groupIndex WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GroupDataBase::getTable:" << q.lastError().text();
    return 1;
  }

  if (q.next())
    table = q.value(0).toString();

  return 0;
}
