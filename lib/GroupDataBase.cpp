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

GroupDataBase::GroupDataBase ()
{
  _table = "groups";
  open();
  init();
}

void GroupDataBase::init ()
{
  QSqlQuery q(_db);
  QString s = "CREATE TABLE IF NOT EXISTS " + _table + " (";
  s.append("name TEXT PRIMARY KEY");
  s.append(", items TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "GroupDataBase::init: " << q.lastError().text();
}

int GroupDataBase::load (QString name, QStringList &group)
{
  group.clear();

  if (name.isEmpty())
    return 1;

  QString s = "SELECT items FROM " + _table + " WHERE name='" + name + "'";
  QSqlQuery q(_db);
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GroupDataBase::load: " << q.lastError().text();
    return 1;
  }

  if (q.next())
    group = q.value(0).toString().split(",");

  return 0;
}

int GroupDataBase::save (QString name, QStringList &group)
{
  if (name.isEmpty())
    return 1;

  QSqlQuery q(_db);

  QString s = "INSERT OR REPLACE INTO " + _table + " VALUES (";
  s.append("'" + name + "'");
  s.append(",'" + group.join(",") + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GroupDataBase::save: " << q.lastError().text();
    qDebug() << s;
    return 1;
  }

  return 0;
}

int GroupDataBase::names (QStringList &l)
{
  l.clear();

  QSqlQuery q(_db);

  QString s = "SELECT DISTINCT name FROM " + _table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "GroupDataBase::names: " << q.lastError().text();
    return 1;
  }

  while (q.next())
    l << q.value(0).toString();

  l.sort();

  return 0;
}

int GroupDataBase::remove (QStringList &names)
{
  int loop = 0;
  for (; loop < names.count(); loop++)
  {
    QString s = "DELETE FROM " + _table + " WHERE name='" + names.at(loop) + "'";
    QSqlQuery q(_db);
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "GroupDataBase::remove: " << q.lastError().text();
      return 1;
    }
  }

  return 0;
}
