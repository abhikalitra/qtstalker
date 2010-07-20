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

#include "Details.h"
#include "QSDetailsDataBase.h"
#include "QSQuoteDataBase.h"
#include "QSDetail.h"
#include "QSSymbol.h"
#include "QSGlobals.h" // for mutex

#include <QtDebug>

Details::Details ()
{
  _methodList << "G" << "S" << "R";
}

int Details::command (QStringList &input, QString &dbPath, QString &output)
{
  output = "ERROR";

  QSLog log;

  if (input.count() < 2)
  {
    log.message(QSLog::Error, QString(" Details::command invalid parm count " + QString::number(input.count())));
    return 1;
  }

  int rc = 0;

  switch ((Method) _methodList.indexOf(input.at(1)))
  {
    case _Get:
      rc = get(input, dbPath, output, log);
      break;
    case _Set:
      rc = set(input, dbPath, output, log);
      break;
    case _Remove:
      rc = remove(input, dbPath, output, log);
      break;
    default:
      break;
  }

  return rc;
}

int Details::get (QStringList &input, QString &dbPath, QString &output, QSLog &log)
{
  // format = Details,G,<EXCHANGE>,<SYMBOL>,<DETAILS>
  //             0    1     2         3        4
  // DETAILS is a colon delimited list of detail keywords ie Name:Sector
  // a comma delimited list of details is returned is the same order as requested

  if (input.count() < 5)
  {
    log.message(QSLog::Error, QString(" Details::get: invalid parm count " + QString::number(input.count())));
    return 1;
  }

  // create symbol structure
  QSSymbol symbol;
  symbol.exchange = input.at(2);
  symbol.symbol = input.at(3);

  // verify the details
  QStringList details = input.at(4).split(":");
  QSDetail detail;
  int loop = 0;
  for (; loop < details.count(); loop++)
  {
    if (detail.addKey(details[loop]))
    {
      log.message(QSLog::Error, QString(" Details::get: invalid detail " + details.at(loop)));
      return 1;
    }
  }

  QSQuoteDataBase db(dbPath);
  if (db.symbol(symbol))
  {
    log.message(QSLog::Error, QString(" Details::get: error getting symbol " + input.at(2) + " " + input.at(3)));
    return 1;
  }

  detail.setTable(symbol.table);
  
  QSDetailsDataBase ddb;
  if (ddb.details(detail))
  {
    log.message(QSLog::Error, QString(" Details::get: error getting detail"));
    return 1;
  }

  detail.string(output);

  return 0;
}

int Details::set (QStringList &input, QString &dbPath, QString &output, QSLog &log)
{
  // format = Details,S,<EXCHANGE>,<SYMBOL>,<DETAILS>,<DETAIL TEXT>
  //             0    1      2        3        4            5
  
  if (input.count() < 6)
  {
    log.message(QSLog::Error, QString(" Details::set: invalid parm count " + QString::number(input.count())));
    return 1;
  }

  QSSymbol symbol;
  symbol.exchange = input.at(2);
  symbol.symbol = input.at(3);

  // verify the details
  QStringList details = input.at(4).split(":");
  QSDetail detail;
  int loop = 0;
  for (; loop < details.count(); loop++)
  {
    if (detail.addKey(details[loop]))
    {
      log.message(QSLog::Error, QString(" Details::set: invalid detail " + details.at(loop)));
      return 1;
    }
  }

  // check if the # of details matches the # of data items
  if (input.count() - 5 != details.count())
  {
    log.message(QSLog::Error, QString(" Details::set: details(" + QString::number(details.count()) + ") != items(" + QString::number(input.count() - 5) + ")"));
    return 1;
  }
  
  // now gather the data to save
  for (loop = 5; loop < input.count(); loop++)
    detail.addData(input[loop]);

  QSQuoteDataBase db(dbPath);
  if (db.symbol(symbol))
  {
    log.message(QSLog::Error, QString(" Details::set: error getting symbol " + input.at(2) + " " + input.at(3)));
    return 1;
  }

  detail.setTable(symbol.table);

  // lock the mutex so we stop other threads from writing to the database
  g_mutex.lock();

  QSDetailsDataBase ddb;
  ddb.transaction();
  if (ddb.setDetails(detail))
  {
    log.message(QSLog::Error, QString(" Details::set: error getting detail"));
    return 1;
  }
  ddb.commit();

  // we are done writing, unlock the mutex so other threads can write
  g_mutex.unlock();
  
  output = "OK";
  
  return 0;
}

int Details::remove (QStringList &input, QString &dbPath, QString &output, QSLog &log)
{
  // format = Details,R,<EXCHANGE>,<SYMBOL>
  //             0    1      2        3

  if (input.count() != 4)
  {
    log.message(QSLog::Error, QString(" Details::remove: invalid parm count " + QString::number(input.count())));
    return 1;
  }

  QSSymbol symbol;
  symbol.exchange = input.at(2);
  symbol.symbol = input.at(3);

  QSQuoteDataBase db(dbPath);
  if (db.symbol(symbol))
  {
    log.message(QSLog::Error, QString(" Details::remove: error getting symbol " + input.at(2) + " " + input.at(3)));
    return 1;
  }

  QSDetail detail;
  detail.setTable(symbol.table);

  // lock the mutex so we stop other threads from writing to the database
  g_mutex.lock();

  QSDetailsDataBase ddb;
  ddb.transaction();
  if (ddb.remove(detail))
  {
    log.message(QSLog::Error, QString(" Details::remove: error getting detail"));
    return 1;
  }
  ddb.commit();

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
  Details *o = new Details;
  return ((QSPlugin *) o);
}

