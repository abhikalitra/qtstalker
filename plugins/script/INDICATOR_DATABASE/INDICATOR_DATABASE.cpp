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

#include "INDICATOR_DATABASE.h"
#include "Globals.h"

#include <QtDebug>

INDICATOR_DATABASE::INDICATOR_DATABASE ()
{
  _method << "LOAD" << "SAVE" << "DELETE" << "INDICATORS";

  init();
}

void INDICATOR_DATABASE::init ()
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
      qDebug() << "INDICATOR_DATABASE::init:" << _db.lastError().text();
  }

  QSqlQuery q(_db);
  QString s = "CREATE TABLE IF NOT EXISTS indicator (";
  s.append("record INTEGER PRIMARY KEY AUTOINCREMENT");
  s.append(", session TEXT");
  s.append(", name TEXT");
  s.append(", row INT");
  s.append(", date INT");
  s.append(", log INT");
  s.append(", command TEXT");
  s.append(", script TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "INDICATOR_DATABASE::init:" << q.lastError().text();
}

int INDICATOR_DATABASE::command (Command &command)
{
  // INDICATOR_DATABASE,<METHOD>
  //         0          1
  
  if (command.count() < 2)
  {
    qDebug() << "INDICATOR_DATABASE::command: invalid parm count" << command.count();
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
      return deleteIndicator(command);
      break;
    case _INDICATORS:
      return indicators(command);
      break;
    default:
      break;
  }

  return 0;
}

int INDICATOR_DATABASE::load (Command &command)
{
  // INDICATOR_DATABASE,<LOAD>,<NAME>
  //          0            1       2

  if (command.count() != 3)
  {
    qDebug() << "INDICATOR_DATABASE::load: invalid parm count" << command.count();
    return 1;
  }

  QString name = command.parm(2);

  QSqlQuery q(_db);
  QString s = "SELECT script,command,row,log,date FROM indicator WHERE name='" + name + "'";
  s.append(" AND session='" + g_session + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "INDICATOR_DATABASE::load:" << q.lastError().text();
    return 1;
  }
  
  if (! q.next())
    return 1;

  QStringList l;
  int pos = 0;
  l << q.value(pos++).toString();
  l << q.value(pos++).toString();
  l << q.value(pos++).toString();
  l << q.value(pos++).toString();
  l << q.value(pos++).toString();

  command.setReturnData(l.join(","));

  emit signalDone(l.join(","));

  return 0;
}

int INDICATOR_DATABASE::save (Command &command)
{
  // INDICATOR_DATABASE,<METHOD>,<NAME>,<FILE>,<COMMAND>,<ROW>,<LOG>,<DATE>
  //           0           1       2       3       4       5     6     7

  if (command.count() != 8)
  {
    qDebug() << "INDICATOR_DATABASE::save: invalid parm count" << command.count();
    return 1;
  }

  int pos = 2;
  QString name = command.parm(pos++);

  QSqlQuery q(_db);
  _db.transaction();
  
  QString s = "INSERT OR REPLACE INTO indicator (name,session,script,command,row,log,date) VALUES (";
  s.append("'" + name + "'");
  s.append(",'" + g_session + "'");
  s.append(",'" + command.parm(pos++) + "'");
  s.append(",'" + command.parm(pos++) + "'");
  s.append("," + command.parm(pos++));
  s.append("," + command.parm(pos++));
  s.append("," + command.parm(pos++));
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "INDICATOR_DATABASE::save:" << q.lastError().text();
    return 1;
  }

  _db.commit();
  
  command.setReturnData("0");

  emit signalDone();

  return 0;
}

int INDICATOR_DATABASE::deleteIndicator (Command &command)
{
  // INDICATOR_DATABASE,<DELETE>,<NAME>
  //           0           1       2

  if (command.count() != 3)
  {
    qDebug() << "INDICATOR_DATABASE::deleteIndicator: invalid parm count" << command.count();
    return 1;
  }

  QString name = command.parm(2);
    
  QSqlQuery q(_db);
  _db.transaction();
  
  QString s = "DELETE FROM indicator WHERE name='" + name + "' AND session='" + g_session + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "INDICATOR_DATABASE::deleteIndicator:" << q.lastError().text();
    return 1;
  }

  _db.commit();

  command.setReturnData("0");
  
  emit signalDone();
  
  return 0;
}

int INDICATOR_DATABASE::indicators (Command &command)
{
  // INDICATOR_DATABASE,INDICATORS
  //           0           1

  QSqlQuery q(_db);
  QString s = "SELECT name FROM indicator where session='" + g_session + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "INDICATOR_DATABASE::indicators:" << q.lastError().text();
    return 1;
  }

  QStringList l;
  while (q.next())
    l << q.value(0).toString();

  l.sort();

  command.setReturnData(l.join(","));

  emit signalDone(l.join(","));

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  INDICATOR_DATABASE *o = new INDICATOR_DATABASE;
  return ((ScriptPlugin *) o);
}
