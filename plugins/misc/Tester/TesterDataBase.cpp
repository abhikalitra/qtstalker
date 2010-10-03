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

#include "TesterDataBase.h"

#include <QtDebug>

TesterDataBase::TesterDataBase ()
{
  _dbName = "data";
  
  // create the Tester table
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "CREATE TABLE IF NOT EXISTS TesterPlugin (";
  s.append("name TEXT PRIMARY KEY UNIQUE");
//  s.append(", exchange TEXT");
//  s.append(", fileSymbol INT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "TesterDataBase::TesterDataBase: " << q.lastError().text();
}

void TesterDataBase::getRules (QStringList &l)
{
  l.clear();
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT name FROM TesterPlugin ORDER BY name ASC";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "TesterDataBase::getRules: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());
}

int TesterDataBase::getRule (Setting &rule)
{
  QString name;
  rule.getData(_Name, name);
  if (name.isEmpty())
    return 1;
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString k = "SELECT blah FROM TesterPlugin";
  k.append(" WHERE name='" + name + "'");
  q.exec(k);
  if (q.lastError().isValid())
  {
    qDebug() << "TesterDataBase::getRule" << q.lastError().text();
    return 1;
  }

  if (! q.next())
    return 1;
  
  int pos = 0;
//  rule.setType(q.value(pos++).toString());
//  rule.setRemoveSuffix(q.value(pos++).toInt());

  return 0;
}

void TesterDataBase::setRule (Setting &rule)
{
  QString name;
  rule.getData(_Name, name);
  if (name.isEmpty())
  {
    qDebug() << "TesterDataBase::setRule: no name";
    return;
  }
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "INSERT OR REPLACE INTO TesterPlugin (name,blah) VALUES (";
  s.append("'" + name + "'"); // name
//  s.append(",'" + rule.type() + "'");
//  s.append("," + QString::number(rule.removeSuffix()));
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "TesterDataBase::setRule" << q.lastError().text();
}

void TesterDataBase::deleteRule (QString &name)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "DELETE FROM TesterPlugin WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "TesterDataBase::deleteRule" << q.lastError().text();
}

