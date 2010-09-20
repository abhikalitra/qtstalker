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
#include "ChartObjectFactory.h"

#include <QtDebug>

ChartObjectDataBase::ChartObjectDataBase ()
{
  _dbName = "data";
  
  _fieldList << "id" << "type" << "exchange" << "symbol" << "indicator" << "color" << "date" << "date2";
  _fieldList  << "price" << "price2" << "label" << "font" << "extend" << "high" << "low";
  _fieldList  << "line1" << "line2" << "line3" << "line4" << "line5" << "line6";
}

void ChartObjectDataBase::init ()
{
  QSqlQuery q(QSqlDatabase::database(_dbName));

  // create index if needed
  QString s = "CREATE TABLE IF NOT EXISTS chartObjectIndex (";
  s.append("id INT PRIMARY KEY");
  s.append(", type INT");
  s.append(", exchange TEXT");
  s.append(", symbol TEXT");
  s.append(", indicator TEXT");
  s.append(", color TEXT");
  s.append(", date TEXT");
  s.append(", date2 TEXT");
  s.append(", price REAL");
  s.append(", price2 REAL");
  s.append(", label TEXT");
  s.append(", font TEXT");
  s.append(", extend INT");
  s.append(", high REAL");
  s.append(", low REAL");
  s.append(", line1 REAL");
  s.append(", line2 REAL");
  s.append(", line3 REAL");
  s.append(", line4 REAL");
  s.append(", line5 REAL");
  s.append(", line6 REAL");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ChartObjectDataBase::init: " << q.lastError().text();
}

void ChartObjectDataBase::deleteChartObjects (BarData *bd)
{
  transaction();

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "DELETE FROM chartObjectIndex";
  s.append(" WHERE exchange='" + bd->getExchange() + "'");
  s.append(" AND symbol='" + bd->getSymbol() + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ChartObjectDataBase::deleteChartObjects: " << q.lastError().text();
    return;
  }

  commit();
}

void ChartObjectDataBase::deleteChartObjectsIndicator (QString &indicator)
{
  transaction();

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "DELETE FROM chartObjectIndex";
  s.append(" WHERE indicator='" + indicator + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ChartObjectDataBase::deleteChartObjectsIndicator: " << q.lastError().text();
    return;
  }

  commit();
}

void ChartObjectDataBase::deleteChartObject (int id)
{
  transaction();

  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "DELETE FROM chartObjectIndex WHERE id=" + QString::number(id);
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ChartObjectDataBase::deleteChartObject: " << q.lastError().text();

  commit();
}

void ChartObjectDataBase::getChartObjects (QString &exchange, QString &symbol, QString &iname,
                                           QMap<int, ChartObject *> &cos)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT " + _fieldList.join(",") + " FROM chartObjectIndex";
  s.append(" WHERE exchange='" + exchange + "'");
  s.append(" AND symbol='" + symbol + "'");
  s.append(" AND indicator='" + iname + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ChartObjectDataBase::getChartObjects: " << q.lastError().text();
    return;
  }

  ChartObjectFactory fac;
  
  while (q.next())
  {
    ChartObject *co = fac.chartObject(q.value(1).toInt());
    if (! co)
      continue;

    int pos = 0;
    ChartObjectSettings set;
    set.id = q.value(pos++).toInt();
    set.type = q.value(pos++).toInt();
    set.exchange = q.value(pos++).toString();
    set.symbol = q.value(pos++).toString();
    set.indicator = q.value(pos++).toString();
    set.color = QColor(q.value(pos++).toString());
    set.date = q.value(pos++).toDateTime();
    set.date2 = q.value(pos++).toDateTime();
    set.price = q.value(pos++).toDouble();
    set.price2 = q.value(pos++).toDouble();
    set.text = q.value(pos++).toString();

    QStringList l = q.value(pos++).toString().split(":");
    set.font.setFamily(l[0]);
    set.font.setPointSize(l[1].toInt());
    set.font.setWeight(l[2].toInt());
    set.font.setItalic(l[3].toInt());
    set.font.setBold(l[4].toInt());
    
    set.extend = q.value(pos++).toInt();
    set.high = q.value(pos++).toDouble();
    set.low = q.value(pos++).toDouble();
    set.line1 = q.value(pos++).toDouble();
    set.line2 = q.value(pos++).toDouble();
    set.line3 = q.value(pos++).toDouble();
    set.line4 = q.value(pos++).toDouble();
    set.line5 = q.value(pos++).toDouble();
    set.line6 = q.value(pos++).toDouble();

    co->setSettings(set);
    
    cos.insert(set.id, co);
  }
}

void ChartObjectDataBase::setChartObject (ChartObjectSettings &d)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));

  QString s = "INSERT OR REPLACE INTO chartObjectIndex (";
  s.append(_fieldList.join(","));
  s.append(") VALUES (");

  QStringList l;
  l.append(QString::number(d.id));
  l.append(QString::number(d.type));
  l.append("'" + d.exchange + "'");
  l.append("'" + d.symbol + "'");
  l.append("'" + d.indicator + "'");
  l.append("'" + d.color.name() + "'");
  l.append("'" + d.date.toString(Qt::ISODate) + "'");
  l.append("'" + d.date2.toString(Qt::ISODate) + "'");
  l.append(QString::number(d.price));
  l.append(QString::number(d.price2));
  l.append("'" + d.text + "'");

  QStringList l2;
  l2.append(d.font.family());
  l2.append(QString::number(d.font.pointSize()));
  l2.append(QString::number(d.font.weight()));
  l2.append(QString::number(d.font.italic()));
  l2.append(QString::number(d.font.bold()));
  l.append("'" + l2.join(":") + "'");
  
  l.append(QString::number(d.extend));
  l.append(QString::number(d.high));
  l.append(QString::number(d.low));
  l.append(QString::number(d.line1));
  l.append(QString::number(d.line2));
  l.append(QString::number(d.line3));
  l.append(QString::number(d.line4));
  l.append(QString::number(d.line5));
  l.append(QString::number(d.line6));

  s.append(l.join(","));
  s.append(")");
  
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ChartObjectDataBase::setChartObject: " << q.lastError().text();
}

int ChartObjectDataBase::renameSymbol (BarData *obd, BarData *nbd)
{
  // update any chart objects with new symbol name
  QString s = "UPDATE chartObjects";
  s.append(" SET symbol='" + nbd->getSymbol() + "'");
  s.append(", exchange='" + nbd->getExchange() + "'");
  s.append(" WHERE symbol='" + obd->getSymbol() + "'");
  s.append(" AND exchange='" + obd->getExchange() + "'");

  transaction();

  if (command(s, QString("ChartObjectDataBase::renameSymbol")))
    return 1;
  
  commit();
  
  return 0;
}

