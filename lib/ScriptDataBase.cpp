/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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
#include <QtSql>

ScriptDataBase::ScriptDataBase ()
{
}

void ScriptDataBase::init ()
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "CREATE TABLE IF NOT EXISTS script (";
  s.append("name TEXT PRIMARY KEY UNIQUE"); // 0
  s.append(", command TEXT"); // 1
  s.append(", comment TEXT"); // 2
  s.append(", lastRun TEXT"); // 3
  
  s.append(", t1 TEXT"); // 4
  s.append(", t2 TEXT"); // 5
  s.append(", t3 TEXT"); // 6
  s.append(", t4 TEXT"); // 7
  s.append(", t5 TEXT"); // 8
  
  s.append(", i1 INT"); // 9
  s.append(", i2 INT"); // 10
  s.append(", i3 INT"); // 11
  s.append(", i4 INT"); // 12
  s.append(", i5 INT"); // 13

  s.append(", d1 REAL"); // 14
  s.append(", d2 REAL"); // 15
  s.append(", d3 REAL"); // 16
  s.append(", d4 REAL"); // 17
  s.append(", d5 REAL"); // 18

  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ScriptDataBase::createScriptTable: " << q.lastError().text();
}

void ScriptDataBase::transaction ()
{
  QSqlDatabase db = QSqlDatabase::database("data");
  db.transaction();
}

void ScriptDataBase::commit ()
{
  QSqlDatabase db = QSqlDatabase::database("data");
  db.commit();
}

void ScriptDataBase::getScripts (QStringList &l)
{
  l.clear();
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT name FROM script";
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

void ScriptDataBase::getScript (Script &script)
{
  QString name = script.getName();

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT command,comment,lastRun FROM script WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ScriptDataBase::getScript: " << q.lastError().text();
    return;
  }

  if (q.next())
  {
    s = q.value(0).toString();
    script.setCommand(s);

    s = q.value(1).toString();
    script.setComment(s);

    QDateTime dt = q.value(2).toDateTime();
    script.setLastRun(dt);
  }
}

void ScriptDataBase::setScript (Script &script)
{
  QDateTime dt = script.getLastRun();

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "INSERT OR REPLACE INTO script (name,command,comment,lastRun) VALUES (";
  s.append("'" + script.getName() + "'");
  s.append(",'" + script.getCommand() + "'");
  s.append(",'" + script.getComment() + "'");
  s.append(",'" + dt.toString(Qt::ISODate) + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ScriptDataBase::setScript: " << q.lastError().text();
}

void ScriptDataBase::deleteScript (Script &script)
{
  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "DELETE FROM script WHERE name='" + script.getName() + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ScriptDataBase::deleteScript: " << q.lastError().text();
}

void ScriptDataBase::getScriptSearch (QString &pattern, QStringList &list)
{
  list.clear();

  QSqlQuery q(QSqlDatabase::database("data"));
  QString s = "SELECT name FROM script WHERE name LIKE '" + pattern + "'";
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

