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

#include "DataDataBase.h"
#include "Globals.h"

#include <QtDebug>
#include <QFile>
#include <QTextStream>

DataDataBase::DataDataBase (QString table)
{
  _table = table;
  init();
}

void DataDataBase::init ()
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
      qDebug() << "DataDataBase::init:" << _db.lastError().text();
  }

  QSqlQuery q(_db);
  QString s = "CREATE TABLE IF NOT EXISTS " + _table + " (";
  s.append("a INT PRIMARY KEY");
  s.append(", name TEXT");
  s.append(", key TEXT");
  s.append(", data TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "DataDataBase::init: " << q.lastError().text();
}

int DataDataBase::load (QString name, Setting *set)
{
  set->clear();
  
  QSqlQuery q(_db);
  QString s = "SELECT key,data FROM " + _table + " WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataDataBase::load: " << q.lastError().text();
    return 1;
  }

  while (q.next())
    set->setData(q.value(0).toString(), q.value(1).toString());

  return 0;
}

int DataDataBase::load (QString name, QString key, QString &data)
{
  data.clear();

  QSqlQuery q(_db);
  QString s = "SELECT data FROM " + _table + " WHERE name='" + name + "' AND key='" + key + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataDataBase::load: " << q.lastError().text();
    return 1;
  }

  while (q.next())
    data = q.value(0).toString();

  return 0;
}

int DataDataBase::load (QString name, QStringList &data)
{
  data.clear();

  QSqlQuery q(_db);
  QString s = "SELECT data FROM " + _table + " WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataDataBase::load: " << q.lastError().text();
    return 1;
  }

  while (q.next())
    data << q.value(0).toString();

  return 0;
}

int DataDataBase::save (QString name, Setting *set)
{
  QSqlQuery q(_db);

  int loop = 0;
  QStringList keys;
  set->keyList(keys);
  for (; loop < keys.count(); loop++)
  {
    QString s = "INSERT OR REPLACE INTO " + _table + " VALUES (";
    s.append("NULL"); // auto increment 
    s.append(",'" + name + "'");
    s.append(",'" + keys.at(loop) + "'");
    s.append(",'" + set->data(keys.at(loop)) + "'");
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "DataDataBase::save: " << q.lastError().text();
      qDebug() << s;
    }
  }

  return 0;
}

int DataDataBase::save (QString name, QString key, QString data)
{
  QSqlQuery q(_db);

  QString s = "INSERT OR REPLACE INTO " + _table + " VALUES (";
  s.append("NULL"); // auto increment
  s.append(",'" + name + "'");
  s.append(",'" + key + "'");
  s.append(",'" + data + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataDataBase::save: " << q.lastError().text();
    qDebug() << s;
    return 1;
  }

  return 0;
}

int DataDataBase::save (QString name, QStringList &data)
{
  QSqlQuery q(_db);

  int loop = 0;
  for (; loop < data.count(); loop++)
  {
    QString s = "INSERT OR REPLACE INTO " + _table + " VALUES (";
    s.append("NULL"); // auto increment
    s.append(",'" + name + "'");
    s.append(",'" + QString::number(loop) + "'");
    s.append(",'" + data.at(loop) + "'");
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "DataDataBase::save: " << q.lastError().text();
      qDebug() << s;
    }
  }

  return 0;
}

int DataDataBase::names (QStringList &l)
{
  QSqlQuery q(_db);

  l.clear();

  QString s = "SELECT DISTINCT name FROM " + _table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataDataBase::names: " << q.lastError().text();
    return 1;
  }

  while (q.next())
    l << q.value(0).toString();

  l.sort();

  return 0;
}

int DataDataBase::removeName (QString name)
{
  QSqlQuery q(_db);
  QString s = "DELETE FROM " + _table + " WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataDataBase::removeName: " << q.lastError().text();
    return 1;
  }

  return 0;
}

int DataDataBase::removeKey (QString name, QString key)
{
  QSqlQuery q(_db);
  QString s = "DELETE FROM " + _table + " WHERE name='" + name + "' AND key='" + key + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataDataBase::removeKey: " << q.lastError().text();
    return 1;
  }

  return 0;
}

int DataDataBase::search (QString key, QString data, QStringList &names)
{
  names.clear();

  if (! key.count() || ! data.count())
    return 1;
  
  QSqlQuery q(_db);
  QString s = "SELECT DISTINCT name FROM " + _table + " WHERE key='" + key + "' AND data='" + data + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataDataBase::search: " << q.lastError().text();
    return 1;
  }

  while (q.next())
    names << q.value(0).toString();

  return 0;
}

int DataDataBase::dump (QString file)
{
  QFile f(file);
  if (! f.open(QIODevice::WriteOnly | QIODevice::Text))
    return 1;
  QTextStream out(&f);
  
  QSqlQuery q(_db);
  QString s = "SELECT name,key,data FROM " + _table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataDataBase::dump: " << q.lastError().text();
    return 1;
  }

  while (q.next())
    out << q.value(0).toString() << ";" << q.value(1).toString() << ";" << q.value(2).toString() << "\n";

  return 0;
}

int DataDataBase::dumpName (QString name, QString file)
{
  QFile f(file);
  if (! f.open(QIODevice::WriteOnly | QIODevice::Text))
    return 1;
  QTextStream out(&f);

  QSqlQuery q(_db);
  QString s = "SELECT name,key,data FROM " + _table + " WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataDataBase::dump: " << q.lastError().text();
    return 1;
  }

  while (q.next())
    out << q.value(0).toString() << ";" << q.value(1).toString() << ";" << q.value(2).toString() << "\n";

  return 0;
}

int DataDataBase::import (QString file)
{
  QFile f(file);
  if (! f.open(QIODevice::ReadOnly | QIODevice::Text))
    return 1;
  QTextStream in(&f);

  QSqlQuery q(_db);

  QString s = "DROP TABLE " + _table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataDataBase::import:" << q.lastError().text();
    return 1;
  }

  // create the new table
  s = "CREATE TABLE IF NOT EXISTS " + _table + " (";
  s.append("a INT PRIMARY KEY");
  s.append(", name TEXT");
  s.append(", key TEXT");
  s.append(", data TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "DataDataBase::import:" << q.lastError().text();
    return 1;
  }

  // add records to the table
  while (! in.atEnd())
  {
    s = in.readLine();
    QStringList l = s.split(";", QString::SkipEmptyParts);
    if (l.count() != 3)
      continue;
    
    int pos = 0;
    s = "INSERT OR REPLACE INTO " + _table + " VALUES (";
    s.append("NULL"); // auto increment
    s.append(",'" + l.at(pos++) + "'");
    s.append(",'" + l.at(pos++) + "'");
    s.append(",'" + l.at(pos++) + "'");
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
      qDebug() << "DataDataBase::import:" << q.lastError().text();
  }

  return 0;
}

int DataDataBase::importName (QString name, QString file)
{
  QFile f(file);
  if (! f.open(QIODevice::ReadOnly | QIODevice::Text))
    return 1;
  QTextStream in(&f);

  removeName(name);

  QSqlQuery q(_db);

  // add records to the table
  while (! in.atEnd())
  {
    QString s = in.readLine();
    s = s.trimmed();
    QStringList l = s.split(";", QString::SkipEmptyParts);
    if (l.count() != 3)
      continue;

    int pos = 0;
    s = "INSERT OR REPLACE INTO " + _table + " VALUES (";
    s.append("NULL"); // auto increment
    s.append(",'" + l.at(pos++) + "'");
    s.append(",'" + l.at(pos++) + "'");
    s.append(",'" + l.at(pos++) + "'");
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
      qDebug() << "DataDataBase::import:" << q.lastError().text();
  }

  return 0;
}

void DataDataBase::transaction ()
{
  _db.transaction();
}

void DataDataBase::commit ()
{
  _db.commit();
}
