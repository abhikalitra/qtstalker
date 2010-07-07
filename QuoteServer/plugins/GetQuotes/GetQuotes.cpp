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

#include "GetQuotes.h"
#include "QuoteDataBase.h"
#include "Bar.h"
#include "QSLog.h"

#include <QtDebug>

GetQuotes::GetQuotes ()
{
}

int GetQuotes::command (QStringList &input, QString &dbPath, QString &output)
{
  // input format = GetQuotes,<EXCHANGE>,<SYMBOL>,<LENGTH>,<START_DATE>,<END_DATE>
  //                    0         1         2        3          4            5
  // each parm is delimited by a comma
  // the end of data is delimited by a new line

  // output format depends on security type Stock,Futures
  //           
  // each bar is delimited by a semicolon, and each bar value is delimited by a comma
  // the end of data is delimited by a new line

  output = "ERROR";

  QSLog log;

  if (input.count() != 6)
  {
    log.message(QSLog::Error, QString("GetQuotes: invalid parm count " + QString::number(input.count())));
    return 1;
  }

  Symbol symbol;
  symbol.exchange = input.at(1);
  symbol.symbol = input.at(2);

  Bar bar;
  QStringList l;
  bar.lengthList(l);
  symbol.length = l.indexOf(input.at(3));
  if (symbol.length == -1)
  {
    log.message(QSLog::Error, QString("GetQuotes: invalid length " + input.at(3)));
    return 1;
  }

  symbol.startDate = QDateTime::fromString(input.at(4), QString("yyyyMMddHHmmss"));
  if (! symbol.startDate.isValid())
  {
    log.message(QSLog::Error, QString("GetQuotes: invalid start date " + input.at(4)));
    return 1;
  }
  
  symbol.endDate = QDateTime::fromString(input.at(5), QString("yyyyMMddHHmmss"));
  if (! symbol.endDate.isValid())
  {
    log.message(QSLog::Error, QString("GetQuotes: invalid end date " + input.at(5)));
    return 1;
  }

  QuoteDataBase db(dbPath);
  if (db.symbol(symbol))
   return 1;

  if (db.getBars(symbol))
    return 1;

  output = symbol.data;
  
  return 0;
}

//**********************************************************
//**********************************************************
//**********************************************************

Plugin * createPlugin ()
{
  GetQuotes *o = new GetQuotes;
  return ((Plugin *) o);
}

