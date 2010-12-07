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

#include "GROUP_DATABASE.h"

#include <QtDebug>
#include <QDir>

GROUP_DATABASE::GROUP_DATABASE ()
{
  _method << "LOAD" << "SAVE" << "SAVE_ALL" << "DELETE" << "GROUPS";

  init();
}

void GROUP_DATABASE::init ()
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
      qDebug() << "GROUP_DATABASE::init: open" << _db.lastError().text();
  }

  QSqlQuery q(_db);
  QString s = "CREATE TABLE IF NOT EXISTS groupIndex (";
  s.append("name TEXT PRIMARY KEY UNIQUE");
  s.append(", tableName TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GROUP_DATABASE::init: create table" << _db.lastError().text();
    qDebug() << s;
  }
}

int GROUP_DATABASE::command (Command *command)
{
  // GROUP_DATABASE,<METHOD>
  //         0          1
  
  if (command->count() < 2)
  {
    qDebug() << "GROUP_DATABASE::command: invalid parm count" << command->count();
    return 1;
  }

  switch ((Method) _method.indexOf(command->parm(1)))
  {
    case _LOAD:
      return load(command);
      break;
    case _SAVE:
      return save(command);
      break;
    case _SAVE_ALL:
      return saveAll(command);
      break;
    case _DELETE:
      return deleteGroup(command);
      break;
    case _GROUPS:
      return groups(command);
      break;
    default:
      break;
  }

  return 0;
}

int GROUP_DATABASE::load (Command *command)
{
  // GROUP_DATABASE,<LOAD>,<NAME>
  //       0          1       2

  if (command->count() != 3)
  {
    qDebug() << "GROUP_DATABASE::load: invalid parm count" << command->count();
    return 1;
  }

  QString name = command->parm(2);
  QString table;
  if (getTable(name, table))
    return 1;

  if (table.isEmpty())
  {
    qDebug() << "GROUP_DATABASE::load: group not found" << name;
    return 1;
  }
  
  QSqlQuery q(_db);
  QString s = "SELECT symbol FROM " + table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GROUP_DATABASE::load:" << q.lastError().text();
    return 1;
  }

  QStringList l;
  while (q.next())
    l << q.value(0).toString();

  command->setReturnData(l.join(","));

  return 0;
}

int GROUP_DATABASE::save (Command *command)
{
  // GROUP_DATABASE,<SAVE>,<NAME>,*
  //        0          1       2    *

  if (command->count() < 3)
  {
    qDebug() << "GROUP_DATABASE::save: invalid parm count" << command->count();
    return 1;
  }

  QString name = command->parm(2);
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

  int loop = 3;
  for (; loop < command->count(); loop++)
  {
    QString s = "INSERT OR REPLACE INTO " + table + " (symbol) VALUES (";
    s.append("'" + command->parm(loop) + "'");
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "GROUP_DATABASE::save:" << q.lastError().text();
      return 1;
    }
  }

  _db.commit();

  command->setReturnData("0");

  return 0;
}

int GROUP_DATABASE::saveAll (Command *command)
{
  // GROUP_DATABASE,SAVE_ALL,<NAME>,*
  //        0          1       2    *

  if (command->count() < 3)
  {
    qDebug() << "GROUP_DATABASE::saveAll: invalid parm count" << command->count();
    return 1;
  }

  QString name = command->parm(2);
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
      qDebug() << "GROUP_DATABASE::saveAll:" << q.lastError().text();
      return 1;
    }
    
    s = "CREATE TABLE IF NOT EXISTS " + table + " (";
    s.append("symbol TEXT PRIMARY KEY UNIQUE");
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "GROUP_DATABASE::newGroup:" << q.lastError().text();
      return 1;
    }

    _db.commit();
  }

  QSqlQuery q(_db);
  _db.transaction();

  int loop = 3;
  for (; loop < command->count(); loop++)
  {
    QString s = "INSERT OR REPLACE INTO " + table + " (symbol) VALUES (";
    s.append("'" + command->parm(loop) + "'");
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "GROUP_DATABASE::save:" << q.lastError().text();
      return 1;
    }
  }

  _db.commit();

  command->setReturnData("0");

  return 0;
}

int GROUP_DATABASE::deleteGroup (Command *command)
{
  // GROUP_DATABASE,<DELETE>,<NAME>,*
  //        0          1       2    *

  if (command->count() < 3)
  {
    qDebug() << "GROUP_DATABASE::deleteGroup: invalid parm count" << command->count();
    return 1;
  }

  QSqlQuery q(_db);
  _db.transaction();

  int loop = 2;
  for (; loop < command->count(); loop++)
  {
    QString table;
    QString name = command->parm(loop);
    if (getTable(name, table))
    {
      qDebug() << "GROUP_DATABASE::deleteGroup: group not found" << name;
      continue;
    }
    
    QString s = "DELETE FROM groupIndex WHERE name='" + name + "'";
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "GROUP_DATABASE::deleteGroup:" << q.lastError().text();
      continue;
    }

    s = "DROP TABLE " + table;
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "GROUP_DATABASE::deleteGroup:" << q.lastError().text();
      continue;
    }
  }

  _db.commit();

  command->setReturnData("0");
  
  return 0;
}

int GROUP_DATABASE::groups (Command *command)
{
  // GROUP_DATABASE,<GROUPS>
  //        0          1

  QSqlQuery q(_db);
  QString s = "SELECT name FROM groupIndex ORDER BY name ASC";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GROUP_DATABASE::groups: " << q.lastError().text();
    return 1;
  }

  QStringList l;
  while (q.next())
    l.append(q.value(0).toString());

  command->setReturnData(l.join(","));

  return 0;
}

int GROUP_DATABASE::newGroup (QString &name, QString &table)
{
  QSqlQuery q(_db);

  // create the symbol
  // we use the maximum record column and add 1 to it to generate a new table name
  QString s = "SELECT max(rowid) FROM groupIndex";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GROUP_DATABASE::newGroup: get record max " << q.lastError().text();
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
    qDebug() << "GROUP_DATABASE::newGroup:" << q.lastError().text();
    return 1;
  }

  s = "CREATE TABLE IF NOT EXISTS " + table + " (";
  s.append("symbol TEXT PRIMARY KEY UNIQUE");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GROUP_DATABASE::newGroup:" << q.lastError().text();
    return 1;
  }

  _db.commit();
  
  return 0;
}

int GROUP_DATABASE::getTable (QString &name, QString &table)
{
  table.clear();
  
  QSqlQuery q(_db);
  QString s = "SELECT tableName FROM groupIndex WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GROUP_DATABASE::getTable:" << q.lastError().text();
    return 1;
  }

  if (q.next())
    table = q.value(0).toString();

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  GROUP_DATABASE *o = new GROUP_DATABASE;
  return ((ScriptPlugin *) o);
}
