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

#include "Quotes.h"
#include "QSQuoteDataBase.h"
#include "QSBar.h"
#include "QSGlobals.h" // for mutex
#include "QSDateRange.h"

#include <QtDebug>

Quotes::Quotes ()
{
  _methodList << "Set" << "Date" << "Recent" << "Delete" << "Rename";
}

int Quotes::command (QStringList &input, QString &dbPath, QString &output)
{
  output = "ERROR";

  QSLog log;

  if (input.count() < 2)
  {
    log.message(QSLog::Error, QString(" Quotes::command invalid parm count " + QString::number(input.count())));
    return 1;
  }

  int rc = 0;

  switch ((Method) _methodList.indexOf(input.at(1)))
  {
    case _Set:
      rc = set(input, dbPath, output, log);
      break;
    case _Date:
      rc = date(input, dbPath, output, log);
      break;
    case _Recent:
      rc = recent(input, dbPath, output, log);
      break;
    case _Delete:
      rc = remove(input, dbPath, output, log);
      break;
    case _Rename:
      rc = rename(input, dbPath, output, log);
      break;
    default:
      break;
  }

  return rc;
}

int Quotes::set (QStringList &input, QString &dbPath, QString &output, QSLog &log)
{
  // format = Quotes,Set,<TYPE>,<EXCHANGE>,<SYMBOL>,<DATE_MASK>,* bar fields depending on type
  //           0      1    2        3         4          5        6  
  
  if (input.count() < 6)
  {
    log.message(QSLog::Error, QString(" Quotes::set: invalid parm count " + QString::number(input.count())));
    return 1;
  }

  QSBar tbar;
  QStringList typeList;
  tbar.typeList(typeList);
  int type = typeList.indexOf(input.at(2));
  if (type == -1)
  {
    log.message(QSLog::Error, QString(" Quotes::set: invalid type " + input.at(2)));
    return 1;
  }

  QSSymbol symbol;
  symbol.type = type;
  symbol.exchange = input.at(3);
  symbol.symbol = input.at(4);

  int fields = 0;
  switch ((QSBar::QSBarType) type)
  {
    case QSBar::Futures:
      fields = 7;
      break;
    default:
      fields = 6;
      break;
  }

  QString dateMask = input.at(5);
  
  QList<QSBar> bars;
  int loop = 6;
  int record = 1;
  for (; loop < input.count(); loop += fields, record += 1)
  {
    if (loop + fields > input.count())
    {
      log.message(QSLog::Error, QString(" Quotes::set: not enough bar values starting at record " + QString::number(record)));
      break;
    }
    
    QSBar bar;
    bar.setBarType((QSBar::QSBarType) type);
    
    QDateTime dt = QDateTime::fromString(input[loop], dateMask);
    if (! dt.isValid())
    {
      log.message(QSLog::Error, QString(" Quotes::set: bar date or date mask " + input.at(loop) + " in record " + QString::number(record)));
      continue;
    }
    else
      bar.setDate(dt);
    
    if (bar.setOpen(input[loop + 1]))
    {
      log.message(QSLog::Error, QString(" Quotes::set: invalid bar open " + input.at(loop + 1) + " in record " + QString::number(record)));
      continue;
    }

    if (bar.setHigh(input[loop + 2]))
    {
      log.message(QSLog::Error, QString(" Quotes::set: invalid bar high " + input.at(loop + 2) + " in record " + QString::number(record)));
      continue;
    }

    if (bar.setLow(input[loop + 3]))
    {
      log.message(QSLog::Error, QString(" Quotes::set: invalid bar low " + input.at(loop + 3) + " in record " + QString::number(record)));
      continue;
    }

    if (bar.setClose(input[loop + 4]))
    {
      log.message(QSLog::Error, QString(" Quotes::set: invalid bar close " + input.at(loop + 4) + " in record " + QString::number(record)));
      continue;
    }

    if (bar.setVolume(input[loop + 5]))
    {
      log.message(QSLog::Error, QString(" Quotes::set: invalid bar volume " + input.at(loop + 5) + " in record " + QString::number(record)));
      continue;
    }

    if ((QSBar::QSBarType) type == QSBar::Futures)
    {
      if (bar.setOI(input[loop + 6]))
      {
        log.message(QSLog::Error, QString(" Quotes::set: invalid bar oi " + input.at(loop + 6) + " in record " + QString::number(record)));
        continue;
      }
    }

    if (bar.verify())
    {
      log.message(QSLog::Error, QString(" Quotes::set: invalid bar values in record " + QString::number(record)));
      continue;
    }

    bars.append(bar);
  }

  // lock the mutex so we stop other threads from writing to the database
  g_mutex.lock();

  QSQuoteDataBase db(dbPath);
  db.transaction();
  db.setBars(symbol, bars);
  db.commit();

  // we are done writing, unlock the mutex so other threads can write
  g_mutex.unlock();
  
  output = "OK";
  
  return 0;
}

int Quotes::date (QStringList &input, QString &dbPath, QString &output, QSLog &log)
{
  // format = Quotes,Date,<EXCHANGE>,<SYMBOL>,<LENGTH>,<START_DATE>,<END_DATE>
  //           0       1      2         3        4          5           6
  //
  // each parm is delimited by a comma
  // the end of data is delimited by a new line
  //
  // output format depends on security type Stock,Futures
  // the stable output portion is Start DateTime,End DateTime,Open,High,Low,Close,Volume,*
  //
  // each bar is delimited by a colon, and each bar value is delimited by a comma
  // the end of data is delimited by a new line

  if (input.count() != 7)
  {
    log.message(QSLog::Error, QString(" Quotes::date: invalid parm count " + QString::number(input.count())));
    return 1;
  }

  QSSymbol symbol;
  symbol.exchange = input.at(2);
  symbol.symbol = input.at(3);

  QSBar bar;
  QStringList l;
  bar.lengthList(l);
  symbol.length = l.indexOf(input.at(4));
  if (symbol.length == -1)
  {
    log.message(QSLog::Error, QString(" Quotes::date: invalid length " + input.at(4)));
    return 1;
  }

  symbol.startDate = QDateTime::fromString(input.at(5), QString("yyyyMMddHHmmss"));
  if (! symbol.startDate.isValid())
  {
    log.message(QSLog::Error, QString(" Quotes::date: invalid start date " + input.at(5)));
    return 1;
  }

  symbol.endDate = QDateTime::fromString(input.at(6), QString("yyyyMMddHHmmss"));
  if (! symbol.endDate.isValid())
  {
    log.message(QSLog::Error, QString(" Quotes::date: invalid end date " + input.at(6)));
    return 1;
  }

  QSQuoteDataBase db(dbPath);
  if (db.symbol(symbol))
   return 1;

  if (db.getBars(symbol))
    return 1;

  output = symbol.data;

  return 0;
}

int Quotes::recent (QStringList &input, QString &dbPath, QString &output, QSLog &log)
{
  // format = Quotes,Recent,<EXCHANGE>,<SYMBOL>,<BAR LENGTH>,<DATE RANGE>
  //            0      1        2         3          4            5
  // will return the most recent bars available for <DATE RANGE>

  // each parm is delimited by a comma
  // the end of data is delimited by a new line

  // output format depends on security type Stock,Futures
  // the stable output portion is Start DateTime,End DateTime,Open,High,Low,Close,Volume,*
  //
  // each bar is delimited by a colon, and each bar value is delimited by a comma
  // the end of data is delimited by a new line

  if (input.count() != 6)
  {
    log.message(QSLog::Error, QString(" Quotes::recent: invalid parm count " + QString::number(input.count())));
    return 1;
  }

  QSSymbol symbol;
  symbol.exchange = input.at(2);
  symbol.symbol = input.at(3);

  QSBar bar;
  QStringList l;
  bar.lengthList(l);
  symbol.length = l.indexOf(input.at(4));
  if (symbol.length == -1)
  {
    log.message(QSLog::Error, QString(" Quotes::recent: invalid length " + input.at(4)));
    return 1;
  }

  // check if valid date range index number
  bool ok;
  symbol.dateRange = input.at(5).toInt(&ok);
  if (! ok)
  {
    log.message(QSLog::Error, QString(" Quotes::recent: invalid date range " + input.at(5)));
    return 1;
  }

  // check if date range number is in list index bounds
  QSDateRange dr;
  l.clear();
  dr.list(l);
  if (symbol.dateRange < 0 || symbol.dateRange >= l.count())
  {
    log.message(QSLog::Error, QString(" Quotes::recent: invalid date range " + input.at(5)));
    return 1;
  }

  QSQuoteDataBase db(dbPath);
  if (db.symbol(symbol))
   return 1;

  if (db.getBars(symbol))
    return 1;

  output = symbol.data;

  return 0;
}

int Quotes::remove (QStringList &input, QString &dbPath, QString &output, QSLog &log)
{
  // format = Quotes,Delete,<EXCHANGE>,<SYMBOL>
  //             0     1        2         3

  if (input.count() != 4)
  {
    log.message(QSLog::Error, QString(" Quotes::remove: invalid parm count " + QString::number(input.count())));
    return 1;
  }

  QSSymbol symbol;
  symbol.exchange = input.at(2);
  symbol.symbol = input.at(3);

  // lock the mutex so we stop other threads from writing to the database
  g_mutex.lock();

  QSQuoteDataBase db(dbPath);
  db.transaction();
  db.deleteSymbol(symbol);
  db.commit();

  // we are done writing, unlock the mutex so other threads can write
  g_mutex.unlock();

  output = "OK";

  return 0;
}

int Quotes::rename (QStringList &input, QString &dbPath, QString &output, QSLog &log)
{
  // format = Quotes,Rename,<OLD EXCHANGE>,<OLD SYMBOL>,<NEW EXCHANGE>,<NEW SYMBOL>
  //             0      1          2            3             4             5

  if (input.count() != 6)
  {
    log.message(QSLog::Error, QString(" Quotes::rename: invalid parm count " + QString::number(input.count())));
    return 1;
  }

  QSSymbol osymbol, nsymbol;
  osymbol.exchange = input.at(2);
  osymbol.symbol = input.at(3);
  nsymbol.exchange = input.at(4);
  nsymbol.symbol = input.at(5);

  // lock the mutex so we stop other threads from writing to the database
  g_mutex.lock();

  QSQuoteDataBase db(dbPath);
  db.transaction();
  db.rename(osymbol, nsymbol);
  db.commit();

  // we are done writing, unlock the mutex so other threads can write
  g_mutex.unlock();

  output = "OK";

  return 0;
}

//**********************************************************
//**********************************************************
//**********************************************************

QSPlugin * createPlugin ()
{
  Quotes *o = new Quotes;
  return ((QSPlugin *) o);
}

