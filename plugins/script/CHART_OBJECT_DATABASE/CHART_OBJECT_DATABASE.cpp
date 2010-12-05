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

#include "CHART_OBJECT_DATABASE.h"
#include "Indicator.h"

#include <QtDebug>

CHART_OBJECT_DATABASE::CHART_OBJECT_DATABASE ()
{
  _method << "GET" << "LOAD" << "SAVE" << "DELETE" << "DELETE_SYMBOL" << "DELETE_INDICATOR";
  _method << "SEARCH" << "RENAME" << "TRANSACTION" << "COMMIT";

  _db = QSqlDatabase::database("data");
  QSqlQuery q(_db);

  // create index if needed
  QString s = "CREATE TABLE IF NOT EXISTS chartObjectIndex (";
  s.append("id INT PRIMARY KEY");
  s.append(", type INT");
  s.append(", exchange TEXT");
  s.append(", symbol TEXT");
  s.append(", indicator TEXT");
  s.append(", table TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "CHART_OBJECT_DATABASE::CHART_OBJECT_DATABASE: " << q.lastError().text();
}

int CHART_OBJECT_DATABASE::command (Command &command)
{
  // CHART_OBJECT_DATABASE,<METHOD>
  //         0          1
  
  if (command.count() < 2)
  {
    qDebug() << "CHART_OBJECT_DATABASE::command: invalid parm count" << command.count();
    return 1;
  }

  switch ((Method) _method.indexOf(command.parm(1)))
  {
    case _GET:
      return get(command);
      break;
    case _LOAD:
      return load(command);
      break;
    case _SAVE:
      return save(command);
      break;
    case _DELETE:
      return deleteObject(command);
      break;
    case _DELETE_SYMBOL:
      return deleteSymbol(command);
      break;
    case _DELETE_INDICATOR:
      return deleteIndicator(command);
      break;
    case _OBJECTS:
      return objects(command);
      break;
    case _RENAME:
      return rename(command);
      break;
    case _TRANSACTION:
      return transaction(command);
      break;
    case _COMMIT:
      return commit(command);
      break;
    default:
      break;
  }

  return 0;
}

int CHART_OBJECT_DATABASE::get (Command &command)
{
  // CHART_OBJECT_DATABASE,<GET>,<ID>
  //            0            1     2

  if (command.count() != 3)
  {
    qDebug() << "CHART_OBJECT_DATABASE::get: invalid parm count" << command.count();
    return 1;
  }

  int pos = 2;
  bool ok;
  QString id = command.parm();
  id.toInt(&ok);
  if (! ok)
  {
    qDebug() << "CHART_OBJECT_DATABASE::get: invalid id" << command.parm(pos);
    return 1;
  }

  QSqlQuery q(_db);
  QString s = "SELECT id,type,exchange,symbol,indicator,table FROM chartObjectIndex WHERE id=" + id;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "CHART_OBJECT_DATABASE::load: " << q.lastError().text();
    return 1;
  }

  Setting co;
  pos = 0;
  QString table;
  if (q.next())
  {
    co.setData("ID", q.value(pos++).toString());
    co.setData("Type", q.value(pos++).toString());
    co.setData("Exchange", q.value(pos++).toString());
    co.setData("Symbol", q.value(pos++).toString());
    co.setData("Indicator", q.value(pos++).toString());

    table = q.value(pos++).toString();
    co.setData("Table", table);
  }

  s = "SELECT * FROM " + table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "CHART_OBJECT_DATABASE::get: " << q.lastError().text();
    return 1;
  }

  while (q.next())
    co.setData(q.value(0).toString(), q.value(1).toString());

  command.setReturnData("0");

  emit signalDone();

  return 0;
}

int CHART_OBJECT_DATABASE::load (Command &command)
{
  // CHART_OBJECT_DATABASE,<METHOD>,<ID>
  //            0             1       2

  if (command.count() != 3)
  {
    qDebug() << "CHART_OBJECT_DATABASE::load: invalid parm count" << command.count();
    return 1;
  }

  Indicator *i = command.indicator();
  if (! i)
  {
    qDebug() << "CHART_OBJECT_DATABASE::loads: no indicator";
    return 1;
  }

  int pos = 2;
  bool ok;
  QString id = command.parm();
  id.toInt(&ok);
  if (! ok)
  {
    qDebug() << "CHART_OBJECT_DATABASE::load: invalid id" << command.parm(pos);
    return 1;
  }
  
  QSqlQuery q(_db);
  QString s = "SELECT id,type,exchange,symbol,indicator,table FROM chartObjectIndex WHERE id=" + id;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "CHART_OBJECT_DATABASE::load: " << q.lastError().text();
    return 1;
  }

  Setting co;
  pos = 0;
  QString table;
  if (q.next())
  {
    co.setData("ID", q.value(pos++).toString());
    co.setData("Type", q.value(pos++).toString());
    co.setData("Exchange", q.value(pos++).toString());
    co.setData("Symbol", q.value(pos++).toString());
    co.setData("Indicator", q.value(pos++).toString());
    
    table = q.value(pos++).toString();
    co.setData("Table", table);
  }

  s = "SELECT * FROM " + table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "CHART_OBJECT_DATABASE::load: " << q.lastError().text();
    return 1;
  }

  while (q.next())
    co.setData(q.value(0).toString(), q.value(1).toString());

  i->addChartObject(co);
  
  command.setReturnData("0");

  emit signalDone();

  return 0;
}

int CHART_OBJECT_DATABASE::save (Command &command)
{
  // CHART_OBJECT_DATABASE,<METHOD>,<DATA>
  //           0              1        2

  if (command.count() != 7)
  {
    qDebug() << "CHART_OBJECT_DATABASE::save: invalid parm count" << command.count();
    return 1;
  }

  int pos = 2;
  QString exchange = command.parm(pos++);
  QString symbol = command.parm(pos++);
  QString indicator = command.parm(pos++);
  QString data = command.parm(pos++);

  QSqlQuery q(_db);
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

  command.setReturnData("0");

  emit signalDone();

  return 0;
}

void ChartObjectDataBase::deleteChartObject (int id)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "DELETE FROM chartObjectIndex WHERE id=" + QString::number(id);
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "ChartObjectDataBase::deleteChartObject: " << q.lastError().text();
}

int CHART_OBJECT_DATABASE::deleteSymbol (Command &command)
{
  // CHART_OBJECT_DATABASE,<METHOD>,<YSYMBOL>
  //        0           1        2    

  if (command.count() != 3)
  {
    qDebug() << "CHART_OBJECT_DATABASE::deleteSymbol: invalid parm count" << command.count();
    return 1;
  }

  QSqlQuery q(_db);

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

  command.setReturnData("0");
  
  emit signalDone();
  
  return 0;
}

void ChartObjectDataBase::deleteChartObjectsIndicator (QString &indicator)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "DELETE FROM chartObjectIndex";
  s.append(" WHERE indicator='" + indicator + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ChartObjectDataBase::deleteChartObjectsIndicator: " << q.lastError().text();
    return;
  }
}


int ChartObjectDataBase::renameSymbol (BarData *obd, BarData *nbd)
{
  // update any chart objects with new symbol name
  QString s = "UPDATE chartObjects";
  s.append(" SET symbol='" + nbd->getSymbol() + "'");
  s.append(", exchange='" + nbd->getExchange() + "'");
  s.append(" WHERE symbol='" + obd->getSymbol() + "'");
  s.append(" AND exchange='" + obd->getExchange() + "'");

  if (command(s, QString("ChartObjectDataBase::renameSymbol")))
    return 1;

  return 0;
}


int CHART_OBJECT_DATABASE::transaction (Command &command)
{
  // CHART_OBJECT_DATABASE,<METHOD>
  //        0          1

  if (command.count() != 2)
  {
    qDebug() << "CHART_OBJECT_DATABASE::transaction: invalid parm count" << command.count();
    return 1;
  }

  _db.transaction();
  
  command.setReturnData("0");

  emit signalDone();

  return 0;
}

int CHART_OBJECT_DATABASE::commit (Command &command)
{
  // CHART_OBJECT_DATABASE,<METHOD>
  //        0          1

  if (command.count() != 2)
  {
    qDebug() << "CHART_OBJECT_DATABASE::commit: invalid parm count" << command.count();
    return 1;
  }

  _db.commit();

  command.setReturnData("0");

  emit signalDone();

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  CHART_OBJECT_DATABASE *o = new CHART_OBJECT_DATABASE;
  return ((ScriptPlugin *) o);
}
