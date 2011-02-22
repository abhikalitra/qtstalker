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

#include <QtDebug>

ScriptDataBase::ScriptDataBase ()
{
  init();
}

void ScriptDataBase::init ()
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
      qDebug() << "ScriptDataBase::init:" << _db.lastError().text();
  }

  QSqlQuery q(_db);
  QString s = "CREATE TABLE IF NOT EXISTS script (";
  s.append("name TEXT PRIMARY KEY UNIQUE");
  s.append(", command TEXT");
  s.append(", script TEXT");
  s.append(", minutes INT");
  s.append(", lastRun TEXT");
  s.append(", type TEXT");
  s.append(", comment TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ScriptDataBase::init:" << q.lastError().text();
}

int ScriptDataBase::load (Script *script)
{
  QString name = script->name();
  if (name.isEmpty())
  {
    qDebug() << "ScriptDataBase::load: invalid name";
    return 1;
  }

  QSqlQuery q(_db);
  QString s = "SELECT command,script,minutes,lastRun,type,comment FROM script WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ScriptDataBase::load:" << q.lastError().text();
    return 1;
  }

  if (q.next())
  {
    int pos = 0;
    script->setCommand(q.value(pos++).toString());
    script->setFile(q.value(pos++).toString());
    script->setMinutes(q.value(pos++).toInt());
    script->setLastRun(q.value(pos++).toString());
    script->setType(q.value(pos++).toString());
    script->setComment(q.value(pos++).toString());
  }

  return 0;
}

int ScriptDataBase::save (Script *script)
{
  QString name = script->name();
  if (name.isEmpty())
  {
    qDebug() << "ScriptDataBase::save: invalid name";
    return 1;
  }

  QSqlQuery q(_db);
  _db.transaction();
  
  QString s = "INSERT OR REPLACE INTO script (name,command,script,minutes,lastRun,type,comment) VALUES (";
  s.append("'" + name + "'");
  s.append(",'" + script->command() + "'");
  s.append(",'" + script->file() + "'");
  s.append("," + QString::number(script->minutes()));
  s.append(",'" + script->lastRun() + "'");
  s.append(",'" + script->type() + "'");
  s.append(",'" + script->comment() + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ScriptDataBase::save:" << q.lastError().text();
    return 1;
  }

  _db.commit();
  
  return 0;
}

int ScriptDataBase::deleteScript (QStringList &l)
{
  QSqlQuery q(_db);
  _db.transaction();

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QString s = "DELETE FROM script WHERE name='" + l.at(loop) + "'";
    q.exec(s);
    if (q.lastError().isValid())
      qDebug() << "ScriptDataBase::deleteScript:" << q.lastError().text();
  }

  _db.commit();

  return 0;
}

int ScriptDataBase::scripts (QString type, QStringList &l)
{
  l.clear();
  
  QSqlQuery q(_db);

  QString s = "SELECT name FROM script";
  if (! type.isEmpty())
    s.append(" WHERE type='" + type + "'");
  
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ScriptDataBase::scripts:" << q.lastError().text();
    return 1;
  }

  while (q.next())
    l << q.value(0).toString();

  l.sort();

  return 0;
}

int ScriptDataBase::types (QStringList &l)
{
  l.clear();

  QSqlQuery q(_db);

  QString s = "SELECT DISTINCT type FROM script";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ScriptDataBase::types:" << q.lastError().text();
    return 1;
  }

  while (q.next())
    l << q.value(0).toString();

  l.sort();

  return 0;
}

int ScriptDataBase::timerScripts (QStringList &l)
{
  l.clear();

  QSqlQuery q(_db);
  QString s = "SELECT name FROM script WHERE minutes > 0";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ScriptDataBase::timerScripts:" << q.lastError().text();
    return 1;
  }

  while (q.next())
    l << q.value(0).toString();

  return 0;
}

void ScriptDataBase::transaction ()
{
  _db.transaction();
}

void ScriptDataBase::commit ()
{
  _db.commit();
}
