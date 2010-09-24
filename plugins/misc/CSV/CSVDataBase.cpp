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

#include "CSVDataBase.h"

#include <QtDebug>

CSVDataBase::CSVDataBase ()
{
  _dbName = "data";
  
  // create the CSV table
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "CREATE TABLE IF NOT EXISTS CSVPlugin (";
  s.append("name TEXT PRIMARY KEY UNIQUE");
  s.append(", plugin TEXT");
  s.append(", delimeter TEXT");
  s.append(", rule TEXT");
  s.append(", file TEXT");
  s.append(", exchange TEXT");
  s.append(", fileSymbol INT");
  s.append(", removeSuffix INT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "CSVDataBase::CSVDataBase: " << q.lastError().text();
}

void CSVDataBase::getRules (QStringList &l)
{
  l.clear();
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT name FROM CSVPlugin ORDER BY name ASC";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "CSVDataBase::rules: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());
}

int CSVDataBase::getRule (CSVRule &rule)
{
  if (rule.name().isEmpty())
    return 1;
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString k = "SELECT plugin,delimeter,rule,file,exchange,fileSymbol,removeSuffix FROM CSVPlugin";
  k.append(" WHERE name='" + rule.name() + "'");
  q.exec(k);
  if (q.lastError().isValid())
  {
    qDebug() << "CSVDataBase::getRule" << q.lastError().text();
    return 1;
  }

  if (! q.next())
    return 1;
  
  int pos = 0;
  rule.setType(q.value(pos++).toString());
  rule.setDelimiter(q.value(pos++).toString());
  rule.setRule(q.value(pos++).toString());
  rule.setFile(q.value(pos++).toString());
  rule.setExchange(q.value(pos++).toString());
  rule.setFileSymbol(q.value(pos++).toInt());
  rule.setRemoveSuffix(q.value(pos++).toInt());

  return 0;
}

void CSVDataBase::setRule (CSVRule &rule)
{
  if (rule.name().isEmpty())
  {
    qDebug() << "CSVDataBase::setRule: no name";
    return;
  }
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "INSERT OR REPLACE INTO CSVPlugin (name,plugin,delimeter,rule,file,exchange,fileSymbol,removeSuffix) VALUES (";
  s.append("'" + rule.name() + "'"); // name
  s.append(",'" + rule.type() + "'");
  s.append(",'" + rule.delimiter() + "'");
  s.append(",'" + rule.rule() + "'");
  s.append(",'" + rule.file() + "'");
  s.append(",'" + rule.exchange() + "'");
  s.append("," + QString::number(rule.fileSymbol()));
  s.append("," + QString::number(rule.removeSuffix()));
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "CSVDataBase::setRule" << q.lastError().text();
}

void CSVDataBase::deleteRule (QString &name)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "DELETE FROM CSVPlugin WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "CSVDataBase::deleteRule" << q.lastError().text();
}

