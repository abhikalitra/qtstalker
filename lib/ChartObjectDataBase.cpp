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

#include "ChartObjectDataBase.h"
#include "ChartObjectData.h"

#include <QtDebug>

ChartObjectDataBase::ChartObjectDataBase ()
{
  _table = "chartObjects";
  open();
  init();
}

void ChartObjectDataBase::init ()
{
  QSqlQuery q(_db);
  QString s = "CREATE TABLE IF NOT EXISTS " + _table + " (";
  s.append("id INT PRIMARY KEY");
  s.append(", symbol TEXT");
  s.append(", chart TEXT");
  s.append(", data TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ChartObjectDataBase::init: " << q.lastError().text();
}

int ChartObjectDataBase::load (QString chart, QString symbol, QHash<QString, Data *> &l)
{
  qDeleteAll(l);
  l.clear();

  if (chart.isEmpty())
    return 1;

  if (symbol.isEmpty())
    return 1;

  QSqlQuery q(_db);
  QString s = "SELECT id,data FROM " + _table + " WHERE chart='" + chart + "' AND symbol='" + symbol + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ChartObjectDataBase::load: " << q.lastError().text();
    return 1;
  }

  while (q.next())
  {
    Data *co = new ChartObjectData;
    if (co->fromString(q.value(1).toString()))
    {
      delete co;
      qDebug() << "ChartObjectDataBase::load: Data::fromString error";
      continue;
    }

    l.insert(q.value(0).toString(), co);
  }

  return 0;
}

int ChartObjectDataBase::load (Data *co)
{
  QString id = co->get(ChartObjectData::_ID);
  if (id.isEmpty())
    return 1;

  QSqlQuery q(_db);

  QString s = "SELECT data FROM " + _table + " WHERE id='" + id + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ChartObjectDataBase::load: " << q.lastError().text();
    return 1;
  }

  while (q.next())
  {
    co->clear();
    if (co->fromString(q.value(0).toString()))
    {
      qDebug() << "ChartObjectDataBase::load: Data::fromString error";
      return 1;
    }
  }

  return 0;
}

int ChartObjectDataBase::save (Data *co)
{
  QStringList l;
  l << co->get(ChartObjectData::_ID);
  if (remove(l))
    return 1;

  QSqlQuery q(_db);

  QString s = "INSERT OR REPLACE INTO " + _table + " VALUES (";
  s.append(co->get(ChartObjectData::_ID));
  s.append(",'" + co->get(ChartObjectData::_SYMBOL) + "'");
  s.append(",'" + co->get(ChartObjectData::_CHART) + "'");
  s.append(",'" + co->toString() + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ChartObjectDataBase::save: " << q.lastError().text();
    qDebug() << s;
    return 1;
  }

  return 0;
}

int ChartObjectDataBase::names (QStringList &l)
{
  l.clear();

  QSqlQuery q(_db);
  QString s = "SELECT DISTINCT id FROM " + _table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ChartObjectDataBase::names: " << q.lastError().text();
    return 1;
  }

  while (q.next())
    l << q.value(0).toString();

  l.sort();

  return 0;
}

int ChartObjectDataBase::remove (QStringList &names)
{
  QSqlQuery q(_db);
  int loop = 0;
  for (; loop < names.count(); loop++)
  {
    QString s = "DELETE FROM " + _table + " WHERE id=" + names.at(loop);
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "ChartObjectDataBase::remove: " << q.lastError().text();
      return 1;
    }
  }

  return 0;
}

int ChartObjectDataBase::lastId ()
{
  int id = -1;
  QSqlQuery q(_db);
  QString s = "SELECT max(id) FROM " + _table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ChartObjectDataBase::names: " << q.lastError().text();
    return id;
  }

  if (q.next())
    id = q.value(0).toInt();

  return id;
}
