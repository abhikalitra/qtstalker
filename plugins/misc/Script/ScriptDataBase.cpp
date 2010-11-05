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

#include "ScriptDataBase.h"

#include <QDebug>

ScriptDataBase::ScriptDataBase ()
{
  _dbName = "data";

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "CREATE TABLE IF NOT EXISTS ScriptPlugin (";
  s.append("name TEXT PRIMARY KEY UNIQUE"); // 0
  s.append(", type TEXT"); // 1
  s.append(", command TEXT"); // 2
  s.append(", comment TEXT"); // 3
  s.append(", lastRun TEXT"); // 4
  s.append(", file TEXT"); // 5
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ScriptDataBase::ScriptDataBase: " << q.lastError().text();
}

void ScriptDataBase::getScripts (QStringList &l)
{
  l.clear();
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT name FROM ScriptPlugin";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ScriptDataBase::getScripts: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());

  l.sort();
}

int ScriptDataBase::getScript (ScriptItem *script)
{
  int rc = 1;

  if (script->name().isEmpty())
  {
    qDebug() << "ScriptDataBase::getScript: no name";
    return rc;
  }

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT type,command,comment,lastRun,file FROM ScriptPlugin WHERE name='" + script->name() + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ScriptDataBase::getScript: " << q.lastError().text();
    return rc;
  }

  if (q.next())
  {
    rc = 0;
    int pos = 0;
    
    script->setType(q.value(pos++).toString());
    script->setCommand(q.value(pos++).toString());
    script->setComment(q.value(pos++).toString());
    script->setLastRun(q.value(pos++).toDateTime());
    script->setFile(q.value(pos++).toString());
  }
  
  return rc;
}

void ScriptDataBase::setScript (ScriptItem *script)
{
  if (script->name().isEmpty())
  {
    qDebug() << "ScriptDataBase::setScript: no name";
    return;
  }

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "INSERT OR REPLACE INTO ScriptPlugin (name,type,command,comment,lastRun,file) VALUES (";
  s.append("'" + script->name() + "'");
  s.append(",'" + script->type() + "'");
  s.append(",'" + script->command() + "'");
  s.append(",'" + script->comment() + "'");
  QDateTime dt = script->lastRun();
  s.append(",'" + dt.toString(Qt::ISODate) + "'");
  s.append(",'" + script->file() + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ScriptDataBase::setScript: " << q.lastError().text();
}

void ScriptDataBase::deleteScript (ScriptItem *script)
{
  if (script->name().isEmpty())
  {
    qDebug() << "ScriptDataBase::deleteScript: no name";
    return;
  }

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "DELETE FROM ScriptPlugin WHERE name='" + script->name() + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ScriptDataBase::deleteScript: " << q.lastError().text();
}

void ScriptDataBase::getScriptSearch (QString pattern, QStringList &list)
{
  list.clear();

  if (pattern == "*")
    pattern = "%";

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT name FROM ScriptPlugin WHERE name LIKE '" + pattern + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ScriptDataBase::getScriptSearch: " << q.lastError().text();
    return;
  }

  while (q.next())
    list.append(q.value(0).toString());

  list.sort();
}

