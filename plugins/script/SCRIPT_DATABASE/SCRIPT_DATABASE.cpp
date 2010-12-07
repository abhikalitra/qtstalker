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

#include "SCRIPT_DATABASE.h"

#include <QtDebug>

SCRIPT_DATABASE::SCRIPT_DATABASE ()
{
  _method << "LOAD" << "SAVE" << "DELETE" << "SCRIPTS";

  init();
}

void SCRIPT_DATABASE::init ()
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
      qDebug() << "SCRIPT_DATABASE::init:" << _db.lastError().text();
  }

  QSqlQuery q(_db);
  QString s = "CREATE TABLE IF NOT EXISTS script (";
  s.append("name TEXT PRIMARY KEY UNIQUE");
  s.append(", command TEXT");
  s.append(", script TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "SCRIPT_DATABASE::init:" << q.lastError().text();
}

int SCRIPT_DATABASE::command (Command *command)
{
  // SCRIPT_DATABASE,<METHOD>
  //         0          1
  
  if (command->count() < 2)
  {
    qDebug() << "SCRIPT_DATABASE::command: invalid parm count" << command->count();
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
    case _DELETE:
      return deleteScript(command);
      break;
    case _SCRIPTS:
      return scripts(command);
      break;
    default:
      break;
  }

  return 0;
}

int SCRIPT_DATABASE::load (Command *command)
{
  // SCRIPT_DATABASE,LOAD,<NAME>
  //       0          1      2

  if (command->count() != 3)
  {
    qDebug() << "SCRIPT_DATABASE::load: invalid parm count" << command->count();
    return 1;
  }

  QString name = command->parm(2);

  QSqlQuery q(_db);
  QString s = "SELECT command,script FROM script WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "SCRIPT_DATABASE::load:" << q.lastError().text();
    return 1;
  }

  QStringList l;

  if (q.next())
  {
    int pos = 0;
    l << q.value(pos++).toString();
    l << q.value(pos++).toString();
  }

  command->setReturnData(l.join(","));

  return 0;
}

int SCRIPT_DATABASE::save (Command *command)
{
  // SCRIPT_DATABASE,SAVE,<NAME>,<FILE>,<COMMAND>
  //        0         1      2      3       4

  if (command->count() != 5)
  {
    qDebug() << "SCRIPT_DATABASE::save: invalid parm count" << command->count();
    return 1;
  }

  int pos = 2;
  QString name = command->parm(pos++);

  QSqlQuery q(_db);
  _db.transaction();
  
  QString s = "INSERT OR REPLACE INTO script (name,command,script) VALUES (";
  s.append("'" + name + "'");
  s.append(",'" + command->parm(pos++) + "'");
  s.append(",'" + command->parm(pos++) + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "SCRIPT_DATABASE::save:" << q.lastError().text();
    return 1;
  }

  _db.commit();
  
  command->setReturnData("0");

  return 0;
}

int SCRIPT_DATABASE::deleteScript (Command *command)
{
  // SCRIPT_DATABASE,DELETE,<NAME>,*
  //        0           1      2    *

  if (command->count() < 3)
  {
    qDebug() << "SCRIPT_DATABASE::deleteScript: invalid parm count" << command->count();
    return 1;
  }

  QSqlQuery q(_db);
  _db.transaction();

  int loop = 2;
  for (; loop < command->count(); loop++)
  {
    QString s = "DELETE FROM script WHERE name='" + command->parm(loop) + "'";
    q.exec(s);
    if (q.lastError().isValid())
      qDebug() << "SCRIPT_DATABASE::deleteScript:" << q.lastError().text();
  }

  _db.commit();

  command->setReturnData("0");
  
  return 0;
}

int SCRIPT_DATABASE::scripts (Command *command)
{
  // SCRIPT_DATABASE,SCRIPTS
  //        0          1

  QSqlQuery q(_db);
  QString s = "SELECT name FROM script";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "SCRIPT_DATABASE::scripts:" << q.lastError().text();
    return 1;
  }

  QStringList l;
  while (q.next())
    l << q.value(0).toString();

  command->setReturnData(l.join(","));

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  SCRIPT_DATABASE *o = new SCRIPT_DATABASE;
  return ((ScriptPlugin *) o);
}
