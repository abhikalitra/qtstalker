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

#include "QuoteDataBase.h"
#include "Strip.h"
#include "Bar.h"
#include "DateRange.h"
#include "BarLength.h"

#include <QDateTime>
#include <QtDebug>

QuoteDataBase::QuoteDataBase ()
{
  init();
}

void QuoteDataBase::init ()
{
  _db = QSqlDatabase::database("quote");
  if (! _db.isOpen())
  {
    QString s = QDir::homePath() + "/.qtstalker/quote.sqlite";
    _db = QSqlDatabase::addDatabase("QSQLITE", "quote");
    _db.setHostName("QtStalker");
    _db.setDatabaseName(s);
    _db.setUserName("QtStalker");
    _db.setPassword("QtStalker");
    if (! _db.open())
      qDebug() << "QuoteDataBase::init:" << _db.lastError().text();
  }

  QSqlQuery q(_db);
  QString s = "CREATE TABLE IF NOT EXISTS symbolIndex (";
  s.append(" record INTEGER PRIMARY KEY AUTOINCREMENT");
  s.append(", symbol TEXT");
  s.append(", exchange TEXT");
  s.append(", name TEXT");
  s.append(", tableName TEXT");
  s.append(", type TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "QuoteDataBase::init:" << _db.lastError().text();
}

int QuoteDataBase::getBars (BarData *bd)
{
  QString exchange = bd->exchange();
  if (exchange.isEmpty())
  {
    qDebug() << "QuoteDataBase::getBars: invalid exchange";
    return 1;
  }

  QString tsymbol = bd->symbol();
  if (tsymbol.isEmpty())
  {
    qDebug() << "QuoteDataBase::getBars: invalid symbol";
    return 1;
  }

  int length = bd->barLength();
  if (length == -1)
  {
    qDebug() << "QuoteDataBase::getBars: invalid length";
    return 1;
  }

  QDateTime startDate = bd->startDate();
  QDateTime endDate = bd->endDate();
  
  int dateRange = bd->range();

  if (getSymbol(bd))
    return 1;
  
  QSqlQuery q(_db);

  // check if we want to return most recent bars
  if (! startDate.isValid() && ! endDate.isValid())
  {
    QString s = "SELECT max(date) FROM " + bd->table();
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "QuoteDataBase::getBars:" + q.lastError().text();
      return 1;
    }

    if (q.next())
    {
      endDate = QDateTime::fromString(q.value(0).toString(), "yyyyMMddHHmmss");
      DateRange dr;
      if (dr.dateRange((DateRange::Range) dateRange, endDate, startDate))
      {
        qDebug() << "QuoteDataBase::getBars: date range error" << dateRange;
        return 1;
      }
    }
    else
      return 1;
  }

  QString s = "SELECT date,open,high,low,close,volume,oi";
  s.append(" FROM " + bd->table());
  s.append(" WHERE date >=" + startDate.toString("yyyyMMddHHmmss"));
  s.append(" AND date <=" + endDate.toString("yyyyMMddHHmmss"));
  s.append(" ORDER BY date DESC");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::getBars:" + q.lastError().text();
    return 1;
  }

  QHash<QString, Bar *> bars;
  QList<Bar *> dateList;

  while (q.next())
  {
    QDateTime lastDate = QDateTime::fromString(q.value(0).toString(), "yyyyMMddHHmmss");

    Bar tbar;
    tbar.setDateRange(lastDate, (BarLength::Length) length);
    tbar.rangeKey(s);

    Bar *bar = bars.value(s);
    if (! bar)
    {
      bar = new Bar;
      bar->setDateRange(lastDate, (BarLength::Length) length);
      bar->setOpen(q.value(1).toDouble());
      bar->setHigh(q.value(2).toDouble());
      bar->setLow(q.value(3).toDouble());
      bar->setClose(q.value(4).toDouble());
      bar->setVolume(q.value(5).toDouble());
      bar->setOI(q.value(6).toDouble());
      bar->setLastDate(lastDate);

      bars.insert(s, bar);
      dateList.append(bar);
    }
    else
    {
      bar->setOpen(q.value(1).toDouble());

      double v = q.value(2).toDouble();
      double t = bar->high();
      if (v > t)
        bar->setHigh(q.value(2).toDouble());

      v = q.value(3).toDouble();
      t = bar->low();
      if (v < t)
        bar->setLow(q.value(3).toDouble());

      v = q.value(5).toDouble();
      v += bar->volume();
      bar->setVolume(v);

      bar->setOI(q.value(6).toDouble());
    }
  }

  // append bars in order
  int loop;
  for (loop = dateList.count() - 1; loop > -1; loop--)
  {
    Bar *bar = dateList.at(loop);
    if (! bar)
      continue;

    bd->append(bar);    
  }

  return 0;
}

int QuoteDataBase::setBars (BarData *symbol)
{
  // save bars
  QSqlQuery q(_db);

  if (getSymbol(symbol))
  {
    if (newSymbol(symbol))
      return 1;
  }
  else
  {
    // check if we need to save the name
    if (! symbol->name().isEmpty())
      setName(symbol);
  }

  int loop = 0;
  for (; loop < symbol->count(); loop++)
  {
    Bar *bar = symbol->bar(loop);
    if (! bar)
      continue;
    
    QDateTime dt = bar->startDate();
    QString date = dt.toString("yyyyMMddHHmmss");

    // first check if record exists so we know to do an update or insert
    QString s = "SELECT date FROM " + symbol->table() + " WHERE date =" + date;
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "QuoteDataBase::setBars:" << q.lastError().text();
      qDebug() << s;
      continue;
    }

    if (q.next()) // record exists, use update
    {
      s = "UPDATE " + symbol->table() + " SET ";
      s.append("open=" + QString::number(bar->open()));
      s.append(", high=" + QString::number(bar->high()));
      s.append(", low=" + QString::number(bar->low()));
      s.append(", close=" + QString::number(bar->close()));
      s.append(", volume=" + QString::number(bar->volume()));
      s.append(", oi=" + QString::number(bar->oi()));
      s.append(" WHERE date=" + date);
    }
    else // new record, use insert
    {
      s = "INSERT INTO " + symbol->table() + " (date,open,high,low,close,volume,oi) VALUES(";
      s.append(date);
      s.append("," + QString::number(bar->open()));
      s.append("," + QString::number(bar->high()));
      s.append("," + QString::number(bar->low()));
      s.append("," + QString::number(bar->close()));
      s.append("," + QString::number(bar->volume()));
      s.append("," + QString::number(bar->oi()));
      s.append(")");
    }

    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "QuoteDataBase::setBars:" << q.lastError().text();
      qDebug() << s;
      continue;    
    }
  }

  return 0;
}

//***************
//***************
//***************

int QuoteDataBase::newSymbol (BarData *symbol)
{
  if (symbol->exchange().isEmpty() || symbol->symbol().isEmpty() || symbol->type().isEmpty())
  {
    qDebug() << "QuoteDataBase::newSymbol: exchange/symbol/type missing";
    return 1;
  }

  if (! getSymbol(symbol))
  {
    qDebug() << "QuoteDataBase::newSymbol: duplicate symbol";
    return 1;
  }

  QSqlQuery q(_db);

  // create the symbol
  // we use the maximum record column and add 1 to it to generate a new table name
  QString s = "SELECT max(record) FROM symbolIndex";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::newSymbol: get record max " + q.lastError().text();
    return 1;
  }

  symbol->setTable("Q1");
  if (q.next())
    symbol->setTable("Q" + QString::number(q.value(0).toInt() + 1));

  // set default name to symbol
  if (symbol->name().isEmpty())
    symbol->setName(symbol->symbol());

  // add new symbol entry into the symbolIndex table
  s = "INSERT OR REPLACE INTO symbolIndex (symbol,tableName,exchange,name,type) VALUES(";
  s.append("'" + symbol->symbol() + "'");
  s.append(",'" + symbol->table() + "'");
  s.append(",'" + symbol->exchange() + "'");
  s.append(",'" + symbol->name() + "'");
  s.append(",'" + symbol->type() + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::newSymbol:" + q.lastError().text();
    return 1;
  }

  s = "CREATE TABLE IF NOT EXISTS " + symbol->table() + " (";
  s.append("date INT PRIMARY KEY UNIQUE");
  s.append(", open REAL");
  s.append(", high REAL");
  s.append(", low REAL");
  s.append(", close REAL");
  s.append(", volume INT");
  s.append(", oi INT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::newSymbol:" + q.lastError().text();
    return 1;
  }

  return 0;
}

int QuoteDataBase::getSymbol (BarData *symbol)
{
  if (symbol->exchange().isEmpty() || symbol->symbol().isEmpty())
  {
    qDebug() << "QuoteDataBase::symbol: exchange and/or symbol missing";
    return 1;
  }
  
  QSqlQuery q(_db);
  QString s = "SELECT name,type,tableName FROM symbolIndex";
  s.append(" WHERE symbol='" + symbol->symbol() + "'");
  s.append(" AND exchange='" + symbol->exchange() + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::symbol:" + q.lastError().text();
    return 1;
  }

  if (q.next())
  {
    int pos = 0;
    symbol->setName(q.value(pos++).toString());
    symbol->setType(q.value(pos++).toString());
    symbol->setTable(q.value(pos++).toString());
    return 0;
  }

  return 1;
}

int QuoteDataBase::deleteSymbol (BarData *bd)
{
  QString exchange = bd->exchange();
  if (exchange.isEmpty())
  {
    qDebug() << "QuoteDataBase::deleteSymbol: invalid exchange";
    return 1;
  }

  QString symbol = bd->symbol();
  if (symbol.isEmpty())
  {
    qDebug() << "QuoteDataBase::deleteSymbol: invalid symbol";
    return 1;
  }

  QSqlQuery q(_db);

  // make sure old symbol exists
  QString s = "SELECT tableName FROM symbolIndex";
  s.append(" WHERE symbol='" + symbol + "'");
  s.append(" AND exchange='" + exchange + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::deleteSymbol:" + q.lastError().text();
    return 1;
  }

  QString table;
  if (! q.next())
  {
    qDebug() << "QuoteDataBase::deleteSymbol: symbol not found";
    return 1;
  }
  else
    table = q.value(0).toString();

  // drop quote table
  s = "DROP TABLE " + table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::deleteSymbol:" + q.lastError().text();
    return 1;
  }

  // remove index record
  s = "DELETE FROM symbolIndex";
  s.append(" WHERE symbol='" + symbol + "'");
  s.append(" AND exchange='" + exchange + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::deleteSymbol:" + q.lastError().text();
    return 1;
  }

  return 0;
}

int QuoteDataBase::getExchange (QStringList &l)
{
  l.clear();
  
  QSqlQuery q(_db);
  QString s = "SELECT DISTINCT exchange FROM symbolIndex ORDER BY exchange ASC";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::getExchange:" << q.lastError().text();
    return 1;
  }

  while (q.next())
    l << q.value(0).toString();

  return 0;
}

int QuoteDataBase::rename (BarData *osymbol, BarData *nsymbol)
{
  QString s = osymbol->exchange();
  if (s.isEmpty())
  {
    qDebug() << "QuoteDataBase::rename: invalid old exchange";
    return 1;
  }

  s = osymbol->symbol();
  if (s.isEmpty())
  {
    qDebug() << "QuoteDataBase::rename: invalid old symbol";
    return 1;
  }

  s = nsymbol->exchange();
  if (s.isEmpty())
  {
    qDebug() << "QuoteDataBase::rename: invalid new exchange";
    return 1;
  }

  s = nsymbol->symbol();
  if (s.isEmpty())
  {
    qDebug() << "QuoteDataBase::rename: invalid new symbol";
    return 1;
  }

  QSqlQuery q(_db);

  // make sure old symbol exists
  s = "SELECT tableName FROM symbolIndex";
  s.append(" WHERE symbol='" + osymbol->symbol() + "'");
  s.append(" AND exchange='" + osymbol->exchange() + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::rename:" + q.lastError().text();
    return 1;
  }

  if (! q.next())
  {
    qDebug() << "QuoteDataBase::rename: old exchange/symbol not found";
    return 1;
  }

  // make sure new symbol does not exist
  s = "SELECT tableName FROM symbolIndex";
  s.append(" WHERE symbol='" + nsymbol->symbol() + "'");
  s.append(" AND exchange='" + nsymbol->exchange() + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::rename:" + q.lastError().text();
    return 1;
  }

  if (q.next())
  {
    qDebug() << "QuoteDataBase::rename: new exchange/symbol exists";
    return 1;
  }

  s = "UPDATE symbolIndex";
  s.append(" SET symbol='" + nsymbol->symbol() + "'");
  s.append(", exchange='" + nsymbol->exchange() + "'");
  s.append(" WHERE symbol='" + osymbol->symbol() + "'");
  s.append(" AND exchange='" + osymbol->exchange() + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::rename:" + q.lastError().text();
    return 1;
  }

  return 0;
}

int QuoteDataBase::search (BarData *bd, QList<BarData> &l)
{
  l.clear();
  
  QString exchange = bd->exchange();
  if (exchange.isEmpty())
  {
    qDebug() << "QuoteDataBase::search: invalid exchange";
    return 1;
  }

  QString symbol = bd->symbol();
  if (symbol.isEmpty())
  {
    qDebug() << "QuoteDataBase::search: invalid symbol";
    return 1;
  }

  QSqlQuery q(_db);

  QString s = "SELECT exchange,symbol,name FROM symbolIndex";

  int whereFlag = 0;
  if (exchange != "*")
  {
    whereFlag = 1;

    if (exchange.contains("%"))
      s.append(" WHERE exchange LIKE '" + exchange + "'");
    else
      s.append(" WHERE exchange='" + exchange + "'");
  }

  if (symbol != "*")
  {
    if (symbol.contains("%"))
    {
      if (whereFlag)
        s.append(" AND symbol LIKE '" + symbol + "'");
      else
        s.append(" WHERE symbol LIKE '" + symbol + "'");
    }
    else
    {
      if (whereFlag)
        s.append(" AND symbol='" + symbol + "'");
      else
        s.append(" WHERE symbol='" + symbol + "'");
    }
  }

  s.append(" ORDER BY exchange,symbol ASC");

  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::search:" + q.lastError().text();
    return 1;
  }

  while (q.next())
  {
    int pos = 0;
    BarData bd;
    bd.setExchange(q.value(pos++).toString());
    bd.setSymbol(q.value(pos++).toString());
    bd.setName(q.value(pos++).toString());
    l.append(bd);
  }

  return 0;
}

int QuoteDataBase::setName (BarData *symbol)
{
  if (symbol->exchange().isEmpty())
  {
    qDebug() << "QuoteDataBase::setName: invalid exchange";
    return 1;
  }

  if (symbol->symbol().isEmpty())
  {
    qDebug() << "QuoteDataBase::setName: invalid symbol";
    return 1;
  }

  if (symbol->name().isEmpty())
  {
    qDebug() << "QuoteDataBase::setName: invalid name";
    return 1;
  }

  QSqlQuery q(_db);

  QString s = "UPDATE symbolIndex";
  s.append(" SET name='" + symbol->name() + "'");
  s.append(" WHERE symbol='" + symbol->symbol() + "'");
  s.append(" AND exchange='" + symbol->exchange() + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::setName:" + q.lastError().text();
    return 1;
  }

  return 0;
}

int QuoteDataBase::transaction ()
{
  _db.transaction();
  return 0;
}

int QuoteDataBase::commit ()
{
  _db.commit();
  return 0;
}
