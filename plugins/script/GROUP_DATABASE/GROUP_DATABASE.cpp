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
  _method << "LOAD" << "SAVE" << "DELETE" << "GROUPS";

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
  s.append(", symbols TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GROUP_DATABASE::init: create table" << _db.lastError().text();
    qDebug() << s;
  }
}

int GROUP_DATABASE::command (Command &command)
{
  // GROUP_DATABASE,<METHOD>
  //         0          1
  
  if (command.count() < 2)
  {
    qDebug() << "GROUP_DATABASE::command: invalid parm count" << command.count();
    return 1;
  }

  switch ((Method) _method.indexOf(command.parm(1)))
  {
    case _LOAD:
      return load(command);
      break;
    case _SAVE:
      return save(command);
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

int GROUP_DATABASE::load (Command &command)
{
  // GROUP_DATABASE,<LOAD>,<NAME>
  //       0          1       2

  if (command.count() != 3)
  {
    qDebug() << "GROUP_DATABASE::load: invalid parm count" << command.count();
    return 1;
  }

  QSqlQuery q(_db);
  QString s = "SELECT symbols FROM groupIndex WHERE name='" + command.parm(2) + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GROUP_DATABASE::load:" << q.lastError().text();
    return 1;
  }

  QStringList l;
  
  if (q.next())
    l = q.value(0).toString().split(",", QString::SkipEmptyParts);

  command.setReturnData(l.join(","));

  emit signalDone(l.join(","));

  return 0;
}

int GROUP_DATABASE::save (Command &command)
{
  // GROUP_DATABASE,<SAVE>,<NAME>,*
  //        0          1       2    *

  if (command.count() < 3)
  {
    qDebug() << "GROUP_DATABASE::save: invalid parm count" << command.count();
    return 1;
  }

  QString name = command.parm(2);
  
  // get the table from the group index
  QSqlQuery q(_db);
  QString s = "SELECT symbols FROM groupIndex WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GROUP_DATABASE::save:" << q.lastError().text();
    return 1;
  }
  
  QStringList l;
  int loop = 3;
  for (; loop < command.count(); loop++)
    l << command.parm(loop);

  _db.transaction();

  if (q.next())
  {
    // record exists, use UPDATE
    s = "UPDATE groupIndex";
    s.append(" SET symbols='" + l.join(",") + "'");
    s.append(" WHERE name='" + name + "'");
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "GROUP_DATABASE::save:" << q.lastError().text();
      return 1;
    }
  }
  else
  {
    s = "INSERT OR REPLACE INTO groupIndex (name,symbols) VALUES (";
    s.append("'" + name + "'");
    s.append(",'" + l.join(",") + "'");
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "GROUP_DATABASE::save:" << q.lastError().text();
      return 1;
    }
  }

  _db.commit();

  command.setReturnData("0");

  emit signalDone();

  return 0;
}

int GROUP_DATABASE::deleteGroup (Command &command)
{
  // GROUP_DATABASE,<DELETE>,<NAME>,*
  //        0          1       2    *

  if (command.count() < 3)
  {
    qDebug() << "GROUP_DATABASE::deleteGroup: invalid parm count" << command.count();
    return 1;
  }

  QSqlQuery q(_db);
  _db.transaction();

  int loop = 2;
  for (; loop < command.count(); loop++)
  {
    QString s = "DELETE FROM groupIndex WHERE name='" + command.parm(loop) + "'";
    q.exec(s);
    if (q.lastError().isValid())
      qDebug() << "GROUP_DATABASE::deleteGroup:" << q.lastError().text();
  }

  _db.commit();

  command.setReturnData("0");
  
  emit signalDone();
  
  return 0;
}

int GROUP_DATABASE::groups (Command &command)
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

  command.setReturnData(l.join(","));

  emit signalDone(l.join(","));

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
