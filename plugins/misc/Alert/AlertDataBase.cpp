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

#include "AlertDataBase.h"

#include <QtDebug>

AlertDataBase::AlertDataBase ()
{
  _dbName = "data";
  
  // create the CSV table
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "CREATE TABLE IF NOT EXISTS AlertPlugin (";
  s.append("id INT PRIMARY KEY UNIQUE");
  s.append(", symbol TEXT");
  s.append(", settings TEXT");
  s.append(", indicator TEXT");
  s.append(", plots TEXT");
  s.append(", mail INT");
  s.append(", sound INT");
  s.append(", popup INT");
  s.append(", status INT");
  s.append(", barLength INT");
  s.append(", bars INT");
  s.append(", lastUpdate TEXT");
  s.append(", hitDate TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "AlertDataBase::AlertDataBase: " << q.lastError().text();
}

int AlertDataBase::getAlerts (QList<AlertItem> &l)
{
  l.clear();
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT ";
  s.append("id,symbol,settings,indicator,plots,mail,sound,popup,status,barLength,bars,lastUpdate,hitDate");
  s.append(" FROM AlertPlugin");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "AlertDataBase::rules: " << q.lastError().text();
    return 1;
  }

  while (q.next())
  {
    AlertItem item;
    int pos = 0;
    
    item.setId(q.value(pos++).toInt());
    item.setSymbol(q.value(pos++).toString());

    QString d = q.value(pos++).toString();
    Setting set;
    set.parse(d);
    item.setSettings(set);
    
    item.setIndicator(q.value(pos++).toString());
    item.setPlots(q.value(pos++).toString().split(":"));
    item.setMail(q.value(pos++).toInt());
    item.setSound(q.value(pos++).toInt());
    item.setPopup(q.value(pos++).toInt());
    item.setStatus((AlertItem::Status) q.value(pos++).toInt());
    item.setBarLength(q.value(pos++).toInt());
    item.setBars(q.value(pos++).toInt());
    item.setLastUpdate(q.value(pos++).toDateTime());
    item.setHitDate(q.value(pos++).toDateTime());

    l.append(item);
  }

  return 0;
}

int AlertDataBase::getAlert (AlertItem &item)
{
  if (item.id() == -1)
    return 1;
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString k = "SELECT";
  k.append(" id,symbol,settings,indicator,plots,mail,sound,popup,status,barLength,bars,lastUpdate,hitDate");
  k.append(" FROM AlertPlugin WHERE id=" + QString::number(item.id()));
  q.exec(k);
  if (q.lastError().isValid())
  {
    qDebug() << "AlertDataBase::getAlert" << q.lastError().text();
    return 1;
  }

  if (! q.next())
    return 1;
  
  int pos = 0;
  
  item.setId(q.value(pos++).toInt());
  item.setSymbol(q.value(pos++).toString());
  
  QString d = q.value(pos++).toString();
  Setting set;
  set.parse(d);
  item.setSettings(set);

  item.setIndicator(q.value(pos++).toString());
  item.setPlots(q.value(pos++).toString().split(":"));
  item.setMail(q.value(pos++).toInt());
  item.setSound(q.value(pos++).toInt());
  item.setPopup(q.value(pos++).toInt());
  item.setStatus((AlertItem::Status) q.value(pos++).toInt());
  item.setBarLength(q.value(pos++).toInt());
  item.setBars(q.value(pos++).toInt());
  item.setLastUpdate(q.value(pos++).toDateTime());
  item.setHitDate(q.value(pos++).toDateTime());

  return 0;
}

int AlertDataBase::setAlert (AlertItem &item)
{
  if (item.id() == -1)
  {
    qDebug() << "AlertDataBase::setAlert: no id";
    return 1;
  }
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "INSERT OR REPLACE INTO AlertPlugin (";
  s.append("id,symbol,settings,indicator,plots,mail,sound,popup,status,barLength,bars,lastUpdate,hitDate");
  s.append(") VALUES (");
  s.append(QString::number(item.id()));
  s.append(",'" + item.symbol() + "'");

  Setting set = item.settings();
  QString d;
  set.getString(d);
  s.append(",'" + d + "'");

  s.append(",'" + item.indicator() + "'");
  s.append(",'" + item.plots().join(":") + "'");
  s.append("," + QString::number(item.mail()));
  s.append("," + QString::number(item.sound()));
  s.append("," + QString::number(item.popup()));
  s.append("," + QString::number(item.status()));
  s.append("," + QString::number(item.barLength()));
  s.append("," + QString::number(item.bars()));
  s.append(",'" + item.lastUpdate().toString(Qt::ISODate) + "'");
  s.append(",'" + item.hitDate().toString(Qt::ISODate) + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "AlertDataBase::setAlert" << q.lastError().text();
    return 1;
  }

  return 0;
}

int AlertDataBase::deleteAlert (AlertItem &item)
{
  if (item.id() == -1)
  {
    qDebug() << "AlertDataBase::setAlert: no id";
    return 1;
  }

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "DELETE FROM AlertPlugin WHERE id=" + QString::number(item.id());
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "AlertDataBase::deleteAlert" << q.lastError().text();
    return 1;
  }

  return 0;
}

