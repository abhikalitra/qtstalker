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
  _table = "script_index";
  open();
  init();
}

void ScriptDataBase::init ()
{
  // index table
  QSqlQuery q(_db);
  QString s = "CREATE TABLE IF NOT EXISTS " + _table + " (";
  s.append("name TEXT PRIMARY KEY");
  s.append(", file TEXT");
  s.append(", startup INT");
  s.append(", runInterval INT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ScriptDataBase::init: " << q.lastError().text();
}

int ScriptDataBase::load (QString name, QString &file, QString &startup, QString &interval)
{
  if (name.isEmpty())
    return 1;

  file.clear();
  startup.clear();
  interval.clear();

  QSqlQuery q(_db);
  QString s = "SELECT file,startup,runInterval FROM " + _table + " WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ScriptDataBase::load: " << q.lastError().text();
    return 1;
  }

  if (q.next())
  {
    int pos = 0;
    file = q.value(pos++).toString();
    startup = q.value(pos++).toString();
    interval = q.value(pos++).toString();
  }

  return 0;
}

int ScriptDataBase::save (QString name, QString file, QString startup, QString interval)
{
  if (name.isEmpty())
    return 1;

  QSqlQuery q(_db);
  QString s = "INSERT OR REPLACE INTO " + _table + " VALUES (";
  s.append("'" + name + "'");
  s.append(",'" + file + "'");
  s.append("," + startup);
  s.append("," + interval);
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ScriptDataBase::save: " << q.lastError().text();
    qDebug() << s;
    return 1;
  }

  return 0;
}

int ScriptDataBase::remove (QStringList &names)
{
  QSqlQuery q(_db);
  int loop = 0;
  for (; loop < names.count(); loop++)
  {
    QString s = "DELETE FROM " + _table + " WHERE name='" + names.at(loop) + "'";
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "ScriptDataBase::remove: " << q.lastError().text();
      return 1;
    }
  }

  return 0;
}

int ScriptDataBase::search (QString startup, QString interval, QStringList &l)
{
  l.clear();

  QSqlQuery q(_db);
  QString s = "SELECT name FROM " + _table;

  int whereFlag = 0;

  if (startup != "*")
  {
    if (whereFlag)
      s.append(" AND startup=" + startup);
    else
    {
      s.append(" WHERE startup=" + startup);
      whereFlag++;
    }
  }

  if (interval != "*")
  {
    if (whereFlag)
      s.append(" AND runInterval > " + interval);
    else
    {
      s.append(" WHERE runInterval > " + interval);
      whereFlag++;
    }
  }

  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ScriptDataBase::search: " << q.lastError().text();
    return 1;
  }

  while (q.next())
    l << q.value(0).toString();

  return 0;
}
