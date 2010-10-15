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

#include "ScannerDataBase.h"

#include <QtDebug>

ScannerDataBase::ScannerDataBase ()
{
  _dbName = "data";
  
  // create the CSV table
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "CREATE TABLE IF NOT EXISTS ScannerPlugin (";
  s.append("name TEXT PRIMARY KEY UNIQUE");
  s.append(", symbols TEXT");
  s.append(", settings TEXT");
  s.append(", indicator TEXT");
  s.append(", plots TEXT");
  s.append(", barLength INT");
  s.append(", dateRange INT");
  s.append(", groupName TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ScannerDataBase::ScannerDataBase: " << q.lastError().text();
}

int ScannerDataBase::getScanners (QStringList &l)
{
  l.clear();
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT name FROM ScannerPlugin";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ScannerDataBase::getScanners: " << q.lastError().text();
    return 1;
  }

  while (q.next())
    l.append(q.value(0).toString());

  return 0;
}

int ScannerDataBase::getScanner (ScannerItem &scanner)
{
  if (! scanner.name().length())
    return 1;
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString k = "SELECT";
  k.append(" symbols,settings,indicator,plots,barLength,dateRange,groupName");
  k.append(" FROM ScannerPlugin WHERE name='" + scanner.name() + "'");
  q.exec(k);
  if (q.lastError().isValid())
  {
    qDebug() << "ScannerDataBase::getScanner" << q.lastError().text();
    return 1;
  }

  if (! q.next())
    return 1;
  
  int pos = 0;
  
  QStringList l = q.value(pos++).toString().split(",");
  Group g;
  g.setStringList(l);
  scanner.setGroup(g);

  QString d = q.value(pos++).toString();
  Setting set;
  set.parse(d);
  scanner.setSettings(set);
  
  scanner.setIndicator(q.value(pos++).toString());
  scanner.setPlots(q.value(pos++).toString().split(":"));
  scanner.setBarLength(q.value(pos++).toInt());
  scanner.setDateRange(q.value(pos++).toInt());
  scanner.setGroupName(q.value(pos++).toString());

  return 0;
}

int ScannerDataBase::setScanner (ScannerItem &scanner)
{
  if (! scanner.name().length())
  {
    qDebug() << "ScannerDataBase::setScanner: no name";
    return 1;
  }
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "INSERT OR REPLACE INTO ScannerPlugin (";
  s.append("name,symbols,settings,indicator,plots,barLength,dateRange,groupName");
  s.append(") VALUES (");
  s.append("'" + scanner.name() + "'");

  // symbols
  Group g = scanner.group();
  QStringList l;
  g.getStringList(l);
  s.append(",'" + l.join(",") + "'");

  Setting set = scanner.settings();
  QString d;
  set.getString(d);
  s.append(",'" + d + "'");

  s.append(",'" + scanner.indicator() + "'");
  s.append(",'" + scanner.plots().join(":") + "'");
  s.append("," + QString::number(scanner.barLength()));
  s.append("," + QString::number(scanner.dateRange()));
  s.append(",'" + scanner.groupName() + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ScannerDataBase::setScanner" << q.lastError().text();
    return 1;
  }

  return 0;
}

int ScannerDataBase::deleteScanner (QString name)
{
  if (! name.length())
  {
    qDebug() << "ScannerDataBase::deleteScanner: no name";
    return 1;
  }

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "DELETE FROM ScannerPlugin WHERE name='" + name + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ScannerDataBase::deleteScanner" << q.lastError().text();
    return 1;
  }

  return 0;
}

