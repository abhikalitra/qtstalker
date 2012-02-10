/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2011 Stefan S. Stratigakos
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

#include "EAVDataBase.h"
#include "TypeData.h"

#include <QtDebug>

EAVDataBase::EAVDataBase ()
{
}

EAVDataBase::EAVDataBase (QString table)
{
  _table = table;
  open();
  init();
}

void EAVDataBase::init ()
{
  // EAV structure for data
  QSqlQuery q(_db);
  QString s = "CREATE TABLE IF NOT EXISTS " + _table + " (";
  s.append("a INTEGER PRIMARY KEY"); // auto increment index
  s.append(", name TEXT"); // entity name
  s.append(", key TEXT"); // attribute
  s.append(", value TEXT"); // attribute value
  s.append(", type TEXT"); // data type
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "EAVDataBase::init: " << q.lastError().text();
    return;
  }
}

int EAVDataBase::get (Entity &data)
{
  QString name = data.name();
  if (name.isEmpty())
  {
    qDebug() << "EAVDataBase::init: invalid name" << name;
    return 1;
  }

  QSqlQuery q(_db);
  QString s = "SELECT key,value,type FROM " + _table + " WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "EAVDataBase::get: " << q.lastError().text();
    return 1;
  }

  int flag = 0;
  while (q.next())
  {
    flag++;
    
    Data d((TypeData::Key) q.value(2).toInt());
    
    switch ((TypeData::Key) d.type())
    {
      case TypeData::_LIST:
      case TypeData::_FILE:
      {
	QStringList l = q.value(1).toString().split(";", QString::SkipEmptyParts);
	d.set(l);
	break;
      }
      default:
        d.set(q.value(1).toString(), q.value(2).toInt());
	break;
    }
    
    data.set(q.value(0).toString(), d);
  }
  
  if (! flag)
    return 1;

  return 0;
}

int EAVDataBase::set (Entity &data)
{
  QString name = data.name();
  if (name.isEmpty())
  {
    qDebug() << "EAVDataBase::init: invalid name" << name;
    return 1;
  }

  QStringList l;
  l << name;
  if (remove(l))
  {
    qDebug() << "EAVDataBase::set: error during remove";
    return 1;
  }

  QSqlQuery q(_db);

  QList<QString> keys;
  data.dkeys(keys);

  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Data d;
    if (data.toData(keys.at(loop), d))
      continue;

    QString s = "INSERT INTO " + _table + " VALUES (";
    s.append("NULL"); // auto increment
    s.append(",'" + name + "'");
    s.append(",'" + keys.at(loop) + "'");
    
    switch ((TypeData::Key) d.type())
    {
      case TypeData::_LIST:
      case TypeData::_FILE:
        s.append(",'" + d.toList().join(";") + "'");
	break;
      default:
        s.append(",'" + d.toString() + "'");
	break;
    }
    
    s.append(",'" + QString::number(d.type()) + "'");
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "EAVDataBase::set: " << q.lastError().text();
      qDebug() << s;
      return 1;
    }
  }

  return 0;
}

int EAVDataBase::remove (QStringList l)
{
  QSqlQuery q(_db);
  int loop = 0;
  for (; loop < l.size(); loop++)
  {
    QString s = "DELETE FROM " + _table + " WHERE name='" + l.at(loop) + "'";
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "EAVDataBase::remove: " << q.lastError().text();
      qDebug() << s;
      return 1;
    }
  }

  return 0;
}

int EAVDataBase::search (EAVSearch &req, QStringList &names)
{
  names.clear();

  QSqlQuery q(_db);

  QHash<QString, int> tnames;
  int loop = 0;
  for (; loop < req.size(); loop++)
  {
    QString key, op, value;
    req.get(loop, key, op, value);
    
    QString s = "SELECT name FROM " + _table + " WHERE";
    s.append(" key='" + key + "' AND value" + op + "'" + value + "'");
    
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "EAVDataBase::search: " << q.lastError().text();
      qDebug() << s;
      return 1;
    }
    
    while (q.next())
    {
      int count = 1;
      if (tnames.contains(q.value(0).toString()))
      {
	count = tnames.value(q.value(0).toString());
	count++;
      }
      tnames.insert(q.value(0).toString(), count);
    }
  }
  
  QHashIterator<QString, int> it(tnames);
  while (it.hasNext())
  {
    it.next();
    if (it.value() == req.size())
      names << it.key();
  }
  
  names.sort();

  return 0;
}

int EAVDataBase::names (QStringList &rl)
{
  rl.clear();

  QSqlQuery q(_db);

  QString s = "SELECT DISTINCT name FROM " + _table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "EAVDataBase::names: " << q.lastError().text();
    return 1;
  }

  while (q.next())
    rl << q.value(0).toString();

  rl.sort();

  return 0;
}

int EAVDataBase::keys (QStringList &d)
{
  d.clear();

  QSqlQuery q(_db);

  QString s = "SELECT key FROM " + _table + " GROUP BY key";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "EAVDataBase::names: " << q.lastError().text();
    return 1;
  }

  while (q.next())
    d << q.value(0).toString();
  
  return 0;
}
