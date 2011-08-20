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
#include "SettingGroup.h"
#include "ChartObjectFactory.h"

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
  s.append("a INT PRIMARY KEY");
  s.append(", id INT");
  s.append(", type TEXT");
  s.append(", symbol TEXT");
  s.append(", chart TEXT");
  s.append(", key TEXT");
  s.append(", data TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ChartObjectDataBase::init: " << q.lastError().text();
}

int ChartObjectDataBase::load (QString chart, QString symbol, QHash<QString, ChartObject *> &l)
{
  qDeleteAll(l);
  l.clear();

  if (chart.isEmpty())
    return 1;

  if (symbol.isEmpty())
    return 1;

  QSqlQuery q(_db);
  QString s = "SELECT id,type,key,data FROM " + _table + " WHERE chart='" + chart + "' AND symbol='" + symbol + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ChartObjectDataBase::load: " << q.lastError().text();
    return 1;
  }

  ChartObjectFactory fac;

  while (q.next())
  {
    ChartObject *co = l.value(q.value(0).toString());
    if (! co)
    {
      co = fac.chartObject(q.value(1).toString());
      if (! co)
        continue;

      co->setID(q.value(0).toString());
      co->setPlotName(chart);
      co->setSymbol(symbol);

      l.insert(q.value(0).toString(), co);
    }

    SettingGroup *settings = co->settings();

    Setting *set = settings->get(q.value(2).toString());
    if (! set)
      continue;

    set->fromString(q.value(3).toString());
  }

  return 0;
}

int ChartObjectDataBase::load (ChartObject *co)
{
  if (co->ID().isEmpty())
    return 1;

  QSqlQuery q(_db);

  QString s = "SELECT key,data FROM " + _table + " WHERE id='" + co->ID() + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ChartObjectDataBase::load: " << q.lastError().text();
    return 1;
  }

  SettingGroup *settings = co->settings();

  while (q.next())
  {
    Setting *set = settings->get(q.value(0).toString());
    if (! set)
      continue;

    set->fromString(q.value(1).toString());
  }

  return 0;
}

int ChartObjectDataBase::save (ChartObject *co)
{
  QStringList l;
  l << co->ID();
  if (remove(l))
    return 1;

  QSqlQuery q(_db);
  SettingGroup *settings = co->settings();
  QList<QString> keys = settings->keys();

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Setting *set = settings->get(keys.at(loop));

    QString s = "INSERT OR REPLACE INTO " + _table + " VALUES (";
    s.append("NULL"); // auto increment
    s.append("," + co->ID());
    s.append(",'" + co->type() + "'");
    s.append(",'" + co->symbol() + "'");
    s.append(",'" + co->plotName() + "'");
    s.append(",'" + keys.at(loop) + "'");
    s.append(",'" + set->toString() + "'");
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "ChartObjectDataBase::save: " << q.lastError().text();
      qDebug() << s;
    }
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

