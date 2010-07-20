/*
 *  QuoteServer
 *
 *  Copyright (C) 2010 Stefan S. Stratigakos
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

#include "QSQuoteDataBase.h"
#include "QSLog.h"
#include "QSDateRange.h"

#include <QDebug>
#include <QtSql>
#include <QDir>
#include <QStringList>

QSQuoteDataBase::QSQuoteDataBase (QString &dbPath)
{
  QSLog log;
  
  QSqlDatabase db = QSqlDatabase::database(_dbName, FALSE);
  if (! db.isOpen())
  {
    db = QSqlDatabase::addDatabase("QSQLITE", _dbName);
    db.setHostName("localhost");
    db.setDatabaseName(dbPath);
    db.setUserName("QuoteServer");
    db.setPassword("QuoteServer");
    if (! db.open())
      log.message(QSLog::Error, QString(" QSQuoteDataBase: quotes db open failed"));

    // create the main quotes index table
    QSqlQuery q(db);
    QString s = "CREATE TABLE IF NOT EXISTS symbolIndex (";
    s.append(" record INTEGER PRIMARY KEY AUTOINCREMENT");
    s.append(", symbol TEXT");
    s.append(", exchange TEXT");
    s.append(", tableName TEXT");
    s.append(", type INT");
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
      log.message(QSLog::Error, QString(" QSQuoteDataBase: " + q.lastError().text()));

    // create the details table, we just create it here for simplicity so we only have to check once
    // when server is started instead of every time we open the db.
    // QSDetailsDataBase handles all the actual data management
    s = "CREATE TABLE IF NOT EXISTS detailsIndex (";
    s.append(" tableName TEXT PRIMARY KEY UNIQUE");
    s.append(", Name TEXT");
    s.append(", Sector TEXT");
    s.append(", FuturesCode TEXT");
    s.append(", FuturesExpMonth TEXT");
    s.append(", FuturesExpYear REAL");
    s.append(", T1 TEXT");
    s.append(", T2 TEXT");
    s.append(", T3 TEXT");
    s.append(", T4 TEXT");
    s.append(", T5 TEXT");
    s.append(", T6 TEXT");
    s.append(", T7 TEXT");
    s.append(", T8 TEXT");
    s.append(", T9 TEXT");
    s.append(", T10 TEXT");
    s.append(", T11 TEXT");
    s.append(", T12 TEXT");
    s.append(", T13 TEXT");
    s.append(", T14 TEXT");
    s.append(", T15 TEXT");
    s.append(", D1 REAL");
    s.append(", D2 REAL");
    s.append(", D3 REAL");
    s.append(", D4 REAL");
    s.append(", D5 REAL");
    s.append(", D6 REAL");
    s.append(", D7 REAL");
    s.append(", D8 REAL");
    s.append(", D9 REAL");
    s.append(", D10 REAL");
    s.append(", D11 REAL");
    s.append(", D12 REAL");
    s.append(", D13 REAL");
    s.append(", D14 REAL");
    s.append(", D15 REAL");
    s.append(")");
    q.exec(s);
    if (q.lastError().isValid())
      log.message(QSLog::Error, QString(" QSQuoteDataBase: " + q.lastError().text()));
  }
}

int QSQuoteDataBase::deleteSymbol (QSSymbol &input)
{
//  QTime time;
//  time.start();

  if (symbol(input))
    return 1;

  // drop quote table
  QSLog log;
  QSqlQuery q(QSqlDatabase::database(_dbName));

  QString s = "DROP TABLE " + input.table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    log.message(QSLog::Error, QString(" QSQuoteDataBase::deleteSymbol: drop quotes table" + q.lastError().text()));
    return 1;
  }
  
  // remove index record
  s = "DELETE FROM symbolIndex";
  s.append(" WHERE symbol='" + input.symbol + "'");
  s.append(" AND exchange='" + input.exchange + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    log.message(QSLog::Error, QString(" QSQuoteDataBase::deleteSymbol: remove symbolIndex record" + q.lastError().text()));
    return 1;
  }

//qDebug("Delete: elapsed: %d ms", time.elapsed());

  return 0;
}

int QSQuoteDataBase::search (QString &exchange, QString &symbol, QString &output)
{
//  QTime time;
//  time.start();

  // if exchange and pat is empty then get all symbols from all exchanges will be returned
  QString s = ("SELECT exchange,symbol FROM symbolIndex");

  int whereFlag = 0;
  if (exchange == "*")
    ;
  else
  {
    whereFlag = 1;
    
    if (exchange.contains("%"))
      s.append(" WHERE exchange LIKE '" + exchange + "'");
    else
      s.append(" WHERE exchange='" + exchange + "'");
  }
  
  if (symbol == "*")
    ;
  else
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

  QSLog log;
  QSqlQuery q(QSqlDatabase::database(_dbName));
  q.exec(s);
  if (q.lastError().isValid())
  {
    log.message(QSLog::Error, QString(" QSQuoteDataBase: getSearchList " + q.lastError().text()));
    return 1;
  }

  QStringList l;

  // we create exchange,symbol pairs comma delimiter
  while (q.next())
  {
    int pos = 0;
    QString s = q.value(pos++).toString();
    s.append("," + q.value(pos++).toString());
    l.append(s);
  }

  // now delimit each pair with a semicolon
  output = l.join(":");

//qDebug("Search: elapsed: %d ms", time.elapsed());

  return 0;
}

int QSQuoteDataBase::symbol (QSSymbol &input)
{
//  QTime time;
//  time.start();

  QSLog log;
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT tableName,type FROM symbolIndex";
  s.append(" WHERE symbol='" + input.symbol + "'");
  s.append(" AND exchange='" + input.exchange + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    log.message(QSLog::Error, QString(" QSQuoteDataBase:symbol " + q.lastError().text()));
    return 1;
  }

  if (q.next())
  {
    int pos = 0;
    input.table = q.value(pos++).toString();
    input.type = q.value(pos++).toInt();
  }

//qDebug("Symbol: elapsed: %d ms", time.elapsed());

  return 0;
}

int QSQuoteDataBase::addSymbol (QSSymbol &input)
{
//  QTime time;
//  time.start();

  QSLog log;
  QSqlQuery q(QSqlDatabase::database(_dbName));

  // we use the maximum record column and add 1 to it to generate a new table name
  QString ts = "SELECT max(record) FROM symbolIndex";
  q.exec(ts);
  if (q.lastError().isValid())
  {
    log.message(QSLog::Error, QString(" QSQuoteDataBase:addSymbol get record max " + q.lastError().text()));
    return 1;
  }
  
  if (q.next())
    input.table = "Q" + QString::number(q.value(0).toInt() + 1);
  else
    input.table = "Q1";
    
  // add new symbol entry into the symbolIndex table
  ts = "INSERT OR REPLACE INTO symbolIndex (symbol,tableName,exchange,type) VALUES(";
  ts.append("'" + input.symbol + "'");
  ts.append(",'" + input.table + "'");
  ts.append(",'" + input.exchange + "'");
  ts.append("," + QString::number(input.type));
  ts.append(")");
  q.exec(ts);
  if (q.lastError().isValid())
  {
    log.message(QSLog::Error, QString(" QSQuoteDataBase:addSymbol " + q.lastError().text()));
    return 1;
  }

  ts = "CREATE TABLE IF NOT EXISTS " + input.table + " (";
  ts.append("date INT PRIMARY KEY UNIQUE");
  ts.append(", open REAL");
  ts.append(", high REAL");
  ts.append(", low REAL");
  ts.append(", close REAL");
  ts.append(", volume INT");
  if ((QSBar::QSBarType) input.type == QSBar::Futures)
    ts.append(", oi INT");
  ts.append(")");
  q.exec(ts);
  if (q.lastError().isValid())
  {
    log.message(QSLog::Error, QString(" QSQuoteDataBase:addSymbol " + q.lastError().text()));
    return 1;
  }

//qDebug("AddSymbol: elapsed: %d ms", time.elapsed());

  return 0;
}

void QSQuoteDataBase::exchanges (QString &output)
{
//  QTime time;
//  time.start();

  QSLog log;
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT DISTINCT exchange FROM symbolIndex ORDER BY exchange ASC";
  q.exec(s);
  if (q.lastError().isValid())
  {
    log.message(QSLog::Error, QString(" QSQuoteDataBase:exchanges " + q.lastError().text()));
    return;
  }

  QStringList l;
  while (q.next())
    l.append(q.value(0).toString());

  output = l.join(":");

//qDebug("Exchanges: elapsed: %d ms", time.elapsed());
}

int QSQuoteDataBase::rename (QSSymbol &oldSymbol, QSSymbol &newSymbol)
{
//  QTime time;
//  time.start();

  // make sure old symbol exists
  if (symbol(oldSymbol))
    return 1;

  // make sure new symbol does not exist
  if (symbol(newSymbol))
    return 1;
  
  if (! newSymbol.table.isEmpty())
    return 1;

  QSLog log;
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "UPDATE symbolIndex";
  s.append(" SET symbol='" + newSymbol.symbol + "'");
  s.append(", exchange='" + newSymbol.exchange + "'");
  s.append(" WHERE symbol='" + oldSymbol.symbol + "'");
  s.append(" AND exchange='" + oldSymbol.exchange + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    log.message(QSLog::Error, QString(" QSQuoteDataBase:rename " + q.lastError().text()));
    return 1;
  }

//qDebug("Rename: elapsed: %d ms", time.elapsed());

  return 0;
}

int QSQuoteDataBase::getBars (QSSymbol &output)
{
//  QTime time;
//  time.start();

  QSLog log;
  QSqlQuery q(QSqlDatabase::database(_dbName));

  QHash<QString, QSBar *> bars;
  QList<QSBar *> dateList;

  // check if we want to return most recent bars
  if (! output.startDate.isValid() && ! output.endDate.isValid())
  {
    QString s = "SELECT max(date) FROM " + output.table;
    q.exec(s);
    if (q.lastError().isValid())
    {
      log.message(QSLog::Error, QString(" QSQuoteDataBase::getBars: max date: " + q.lastError().text()));
      return 1;
    }
    
    if (q.next())
    {
      output.endDate = QDateTime::fromString(q.value(0).toString(), "yyyyMMddHHmmss");
      QSDateRange dr;
      if (dr.dateRange((QSDateRange::Range) output.dateRange, output.endDate, output.startDate))
      {
        log.message(QSLog::Error, QString(" QSQuoteDataBase::getBars: date range error"));
        return 1;
      }
    }
    else
      return 1;
  }

  QString s = "SELECT date,open,high,low,close,volume";
  if ((QSBar::QSBarType) output.type == QSBar::Futures)
    s.append(",oi");
  s.append(" FROM " + output.table);
  s.append(" WHERE date >=" + output.startDate.toString("yyyyMMddHHmmss"));
  s.append(" AND date <=" + output.endDate.toString("yyyyMMddHHmmss"));
  s.append(" ORDER BY date DESC");
  q.exec(s);
  if (q.lastError().isValid())
  {
    log.message(QSLog::Error, QString(" QSQuoteDataBase:getBars " + q.lastError().text()));
    return 1;
  }

  while (q.next())
  {
    QDateTime lastDate = QDateTime::fromString(q.value(0).toString(), "yyyyMMddHHmmss");

    QSBar tbar;
    tbar.setDateRange(lastDate, (QSBar::QSBarLength) output.length);
    tbar.rangeKey(s);

    QSBar *bar = bars.value(s);
    if (! bar)
    {
      bar = new QSBar;
      bar->setBarType((QSBar::QSBarType) output.type);
      bar->setDateRange(lastDate, (QSBar::QSBarLength) output.length);
      bar->setOpen(q.value(1).toString());
      bar->setHigh(q.value(2).toString());
      bar->setLow(q.value(3).toString());
      bar->setClose(q.value(4).toString());
      bar->setVolume(q.value(5).toString());
      if ((QSBar::QSBarType) output.type == QSBar::Futures)
        bar->setOI(q.value(6).toString());

      bars.insert(s, bar);
      dateList.append(bar);
    }
    else
    {
      bar->setOpen(q.value(1).toString());

      double v = q.value(2).toDouble();
      double t = bar->high().toDouble();
      if (v > t)
        bar->setHigh(q.value(2).toString());

      v = q.value(3).toDouble();
      t = bar->low().toDouble();
      if (v < t)
        bar->setLow(q.value(3).toString());

      v = q.value(5).toDouble();
      v += bar->volume().toDouble();
      bar->setVolume(QString::number(v));

      if ((QSBar::QSBarType) output.type == QSBar::Futures)
        bar->setOI(q.value(6).toString());
    }
  }

  // append bars in order
  int loop;
  QStringList l;
  for (loop = dateList.count() - 1; loop > -1; loop--)
  {
    QString s;
    QSBar *bar = dateList.at(loop);
    if (! bar)
      continue;
    
    bar->string(s);
    l << s;
  }

  // bar fields use comma delimiter
  // bars use semicolon delimiter
  output.bars = dateList.count(); // set # of bars
  output.data = l.join(":");

  if (bars.count())
    qDeleteAll(bars);

//qDebug("GetBars: elapsed: %d ms", time.elapsed());

  return 0;
}

int QSQuoteDataBase::setBars (QSSymbol &input, QList<QSBar> &bars)
{
//  QTime time;
//  time.start();

  QSLog log;
  if (symbol(input))
  {
    log.message(QSLog::Error, QString(" QSQuoteDataBase:setBars symbol check error "));
    return 1;
  }

  if (input.table.isEmpty())
  {
    if(addSymbol(input))
    {
      log.message(QSLog::Error, QString(" QSQuoteDataBase:setBars add symbol error"));
      return 1;
    }
  }

  QSqlQuery q(QSqlDatabase::database(_dbName));

  int loop = 0;
  for (; loop < bars.count(); loop++)
  {
    QSBar bar = bars.at(loop);

    QDateTime dt = bar.startDate();
    QString date = dt.toString("yyyyMMddHHmmss");

    // first check if record exists so we know to do an update or insert
    QString s = "SELECT date FROM " + input.table + " WHERE date =" + date;
    q.exec(s);
    if (q.lastError().isValid())
    {
      log.message(QSLog::Error, QString(" QSQuoteDataBase:setBars " + q.lastError().text()));
      continue;
    }

    if (q.next()) // record exists, use update
    {
      s = "UPDATE " + input.table;
      s.append(" SET open=" + bar.open());
      s.append(", high=" + bar.high());
      s.append(", low=" + bar.low());
      s.append(", close=" + bar.close());
      s.append(", volume=" + bar.volume());
      if ((QSBar::QSBarType) input.type == QSBar::Futures)
        s.append(", oi=" + bar.oi());
      s.append(" WHERE date=" + date);
    }
    else // new record, use insert
    {
      if ((QSBar::QSBarType) input.type == QSBar::Futures)
        s = "INSERT INTO " + input.table + " (date,open,high,low,close,volume,oi) VALUES(";
      else
        s = "INSERT INTO " + input.table + " (date,open,high,low,close,volume) VALUES(";
      s.append(date);
      s.append("," + bar.open());
      s.append("," + bar.high());
      s.append("," + bar.low());
      s.append("," + bar.close());
      s.append("," + bar.volume());
      if ((QSBar::QSBarType) input.type == QSBar::Futures)
        s.append("," + bar.oi());
      s.append(")");
    }
    
    q.exec(s);
    if (q.lastError().isValid())
    {
      log.message(QSLog::Error, QString(" QSQuoteDataBase:setBars " + q.lastError().text()));
      continue;
    }
  }

//qDebug("SetBars: elapsed: %d ms", time.elapsed());

  return 0;
}

