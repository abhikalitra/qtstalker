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
#include "Globals.h"

#include <QtDebug>

ChartObjectDataBase::ChartObjectDataBase ()
{
  init();
}

void ChartObjectDataBase::init ()
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
      qDebug() << "ChartObjectDataBase::init:" << _db.lastError().text();
  }

  _table = "chartObjectIndex" + g_session;

  QSqlQuery q(_db);
  QString s = "CREATE TABLE IF NOT EXISTS " + _table + " (";
  s.append("id TEXT PRIMARY KEY");
  s.append(", indicator TEXT");
  s.append(", exchange TEXT");
  s.append(", symbol TEXT");
  s.append(", settings TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ChartObjectDataBase::init: " << q.lastError().text();
}

void ChartObjectDataBase::ids (BarData *bd, QString indicator, QStringList &l)
{
  QSqlQuery q(_db);

  l.clear();
  
  QString s = "SELECT id FROM " + _table;
  s.append(" WHERE exchange='" + bd->exchange() + "'");
  s.append(" AND symbol='" + bd->symbol() + "'");
  s.append(" AND indicator='" + indicator + "'");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ChartObjectDataBase::ids: " << q.lastError().text();

  while (q.next())
    l << q.value(0).toString();
}

void ChartObjectDataBase::deleteChartObjects (BarData *bd)
{
  QSqlQuery q(_db);

  _db.transaction();

  // delete index records
  QString s = "DELETE FROM " + _table;
  s.append(" WHERE exchange='" + bd->exchange() + "'");
  s.append(" AND symbol='" + bd->symbol() + "'");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ChartObjectDataBase::deleteChartObjects: " << q.lastError().text();

  _db.commit();
}

void ChartObjectDataBase::deleteChartObjectsIndicator (QString indicator)
{
  QSqlQuery q(_db);

  _db.transaction();

  QString s = "DELETE FROM " + _table;
  s.append(" WHERE indicator='" + indicator + "'");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ChartObjectDataBase::deleteChartObjectsIndicator: " << q.lastError().text();

  _db.commit();
}

void ChartObjectDataBase::deleteChartObject (QString id)
{
  QSqlQuery q(_db);

  QString s = "DELETE FROM " + _table + " WHERE id='" + id + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ChartObjectDataBase::deleteChartObject: " << q.lastError().text();
}

void ChartObjectDataBase::load (Setting *co)
{
  QSqlQuery q(_db);

  QString s = "SELECT settings FROM " + _table + " WHERE id='" + co->data("ID") + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ChartObjectDataBase::load: " << q.lastError().text();
    return;
  }

  if (q.next())
    co->parse(q.value(0).toString());
}

void ChartObjectDataBase::load (QString indicator, BarData *bd, QList<Setting> &l)
{
  QSqlQuery q(_db);

  QString s = "SELECT settings FROM " + _table;
  s.append(" WHERE exchange='" + bd->exchange() + "'");
  s.append(" AND symbol='" + bd->symbol() + "'");
  s.append(" AND indicator='" + indicator + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ChartObjectDataBase::load: " << q.lastError().text();
    return;
  }

  while (q.next())
  {
    Setting co;
    co.parse(q.value(0).toString()); // order is critical (settings first)
    l.append(co);
  }
}

void ChartObjectDataBase::save (Setting *co)
{
  QSqlQuery q(_db);

  _db.transaction();

  QString s = "INSERT OR REPLACE INTO " + _table + " (id,settings,indicator,exchange,symbol)";
  s.append(" VALUES (");
  s.append("'" + co->data("ID") + "'");
  QString ts;
  co->string(ts);
  s.append(",'" + ts + "'");
  s.append(",'" + co->data("Indicator") + "'");
  s.append(",'" + co->data("Exchange") + "'");
  s.append(",'" + co->data("Symbol") + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ChartObjectDataBase::setChartObject: " << q.lastError().text();

  _db.commit();
}

int ChartObjectDataBase::renameSymbol (BarData *obd, BarData *nbd)
{
  // update any chart objects with new symbol name
  QSqlQuery q(_db);

  _db.transaction();

  QString s = "UPDATE " + _table;
  s.append(" SET symbol='" + nbd->symbol() + "'");
  s.append(", exchange='" + nbd->exchange() + "'");
  s.append(" WHERE symbol='" + obd->symbol() + "'");
  s.append(" AND exchange='" + obd->exchange() + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ChartObjectDataBase::renameSymbol: " << q.lastError().text();
    return 1;
  }

  _db.commit();
  
  return 0;
}

void ChartObjectDataBase::transaction ()
{
  _db.transaction();
}

void ChartObjectDataBase::commit ()
{
  _db.commit();
}
