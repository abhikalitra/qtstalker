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
  s.append(", exchange TEXT");
  s.append(", symbol TEXT");
  s.append(", settings TEXT");
  s.append(", indicator TEXT");
  s.append(", mail INT");
  s.append(", sound INT");
  s.append(", popup INT");
  s.append(", enable TEXT");
  s.append(", op TEXT");
  s.append(", value TEXT");
  s.append(", status INT");
  s.append(", barLength INT");
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
  s.append("id,exchange,symbol,settings,indicator,mail,sound,popup,enable,op,value,status,barLength");
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
    item.setExchange(q.value(pos++).toString());
    item.setSymbol(q.value(pos++).toString());
    item.setSettings(q.value(pos++).toString());
    item.setIndicator(q.value(pos++).toString());
    item.setMail(q.value(pos++).toInt());
    item.setSound(q.value(pos++).toInt());
    item.setPopup(q.value(pos++).toInt());
    item.setEnableString(q.value(pos++).toString());
    item.setOpString(q.value(pos++).toString());
    item.setValueString(q.value(pos++).toString());
    item.setStatus((AlertItem::Status) q.value(pos++).toInt());
    item.setBarLength(q.value(pos++).toInt());

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
  k.append(" id,exchange,symbol,settings,indicator,mail,sound,popup,enable,op,value,status,barLength");
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
  item.setExchange(q.value(pos++).toString());
  item.setSymbol(q.value(pos++).toString());
  item.setSettings(q.value(pos++).toString());
  item.setIndicator(q.value(pos++).toString());
  item.setMail(q.value(pos++).toInt());
  item.setSound(q.value(pos++).toInt());
  item.setPopup(q.value(pos++).toInt());
  item.setEnableString(q.value(pos++).toString());
  item.setOpString(q.value(pos++).toString());
  item.setValueString(q.value(pos++).toString());
  item.setStatus((AlertItem::Status) q.value(pos++).toInt());
  item.setBarLength(q.value(pos++).toInt());

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
  s.append("id,exchange,symbol,settings,indicator,mail,sound,popup,enable,op,value,status,barLength");
  s.append(") VALUES (");
  s.append(QString::number(item.id()));
  s.append(",'" + item.exchange() + "'");
  s.append(",'" + item.symbol() + "'");
  s.append(",'" + item.settings() + "'");
  s.append(",'" + item.indicator() + "'");
  s.append("," + QString::number(item.mail()));
  s.append("," + QString::number(item.sound()));
  s.append("," + QString::number(item.popup()));
  s.append(",'" + item.enableString() + "'");
  s.append(",'" + item.opString() + "'");
  s.append(",'" + item.valueString() + "'");
  s.append("," + QString::number(item.status()));
  s.append("," + QString::number(item.barLength()));
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

