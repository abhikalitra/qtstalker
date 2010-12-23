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

#include "IndicatorDataBase.h"
#include "Globals.h"

#include <QtDebug>

IndicatorDataBase::IndicatorDataBase ()
{
  init();
}

void IndicatorDataBase::init ()
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
      qDebug() << "IndicatorDataBase::init:" << _db.lastError().text();
  }

  _table = "indicator" + g_session;
  
  QSqlQuery q(_db);
  QString s = "CREATE TABLE IF NOT EXISTS " + _table + " (";
  s.append("name TEXT PRIMARY KEY UNIQUE");
  s.append(", row INT");
  s.append(", date INT");
  s.append(", log INT");
  s.append(", command TEXT");
  s.append(", script TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "IndicatorDataBase::init:" << q.lastError().text();
}

int IndicatorDataBase::load (Indicator *i)
{
  QString name = i->name();
  if (name.isEmpty())
  {
    qDebug() << "IndicatorDataBase::load: invalid name";
    return 1;
  }

  QSqlQuery q(_db);
  QString s = "SELECT command,script,row,log,date FROM " + _table + " WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "IndicatorDataBase::load:" << q.lastError().text();
    return 1;
  }
  
  if (! q.next())
    return 1;

  int pos = 0;
  i->setCommand(q.value(pos++).toString());
  i->setScript(q.value(pos++).toString());
  i->setTabRow(q.value(pos++).toInt());
  i->setLog(q.value(pos++).toInt());
  i->setDate(q.value(pos++).toInt());

  return 0;
}

int IndicatorDataBase::save (Indicator *i)
{
  QString name = i->name();
  if (name.isEmpty())
  {
    qDebug() << "IndicatorDataBase::save: invalid name";
    return 1;
  }

  QSqlQuery q(_db);
  _db.transaction();
  
  QString s = "INSERT OR REPLACE INTO " + _table + " (name,command,script,row,log,date) VALUES (";
  s.append("'" + name + "'");
  s.append(",'" + i->command() + "'");
  s.append(",'" + i->script() + "'");
  s.append("," + QString::number(i->tabRow()));
  s.append("," + QString::number(i->getLog()));
  s.append("," + QString::number(i->date()));
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "IndicatorDataBase::save:" << q.lastError().text();
    return 1;
  }

  _db.commit();
  
  return 0;
}

int IndicatorDataBase::deleteIndicator (QStringList &l)
{
  QSqlQuery q(_db);
  _db.transaction();

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QString s = "DELETE FROM " + _table + " WHERE name='" + l.at(loop) + "'";
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "IndicatorDataBase::deleteIndicator:" << q.lastError().text();
      continue;
    }
  }

  _db.commit();

  return 0;
}

int IndicatorDataBase::indicators (QStringList &l)
{
  l.clear();

  QSqlQuery q(_db);
  QString s = "SELECT name FROM " + _table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "IndicatorDataBase::indicators:" << q.lastError().text();
    return 1;
  }

  while (q.next())
    l << q.value(0).toString();

  l.sort();

  return 0;
}
