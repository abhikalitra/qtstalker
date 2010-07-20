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

#include "Search.h"
#include "QSQuoteDataBase.h"

#include <QtDebug>

Search::Search ()
{
  _methodList << "S" << "E";
}

int Search::command (QStringList &input, QString &dbPath, QString &output)
{
  output = "ERROR";

  QSLog log;

  if (input.count() < 2)
  {
    log.message(QSLog::Error, QString(" Search::command invalid parm count " + QString::number(input.count())));
    return 1;
  }

  int rc = 0;
  
  switch ((Method) _methodList.indexOf(input.at(1)))
  {
    case _Search:
      rc = search(input, dbPath, output, log);
      break;
    case _Exchange:
      rc = exchange(dbPath, output);
      break;
    default:
      break;
  }

  return rc;
}

int Search::search (QStringList &input, QString &dbPath, QString &output, QSLog &log)
{
  // input format = Search,S,<EXCHANGE>,<SYMBOL>
  //                  0    1     2         3
  
  // output format = EXCHANGE,SYMBOL:

  if (input.count() != 4)
  {
    log.message(QSLog::Error, QString(" Search::search: invalid parm count " + QString::number(input.count())));
    return 1;
  }

  if (input.at(2).isEmpty())
  {
    log.message(QSLog::Error, QString(" Search::search: invalid exchange " + input.at(2)));
    return 1;
  }
  
  if (input.at(3).isEmpty())
  {
    log.message(QSLog::Error, QString(" Search::search: invalid symbol " + input.at(3)));
    return 1;
  }

  QSQuoteDataBase db(dbPath);
  if (db.search(input[2], input[3], output))
   return 1;

  return 0;
}

int Search::exchange (QString &dbPath, QString &output)
{
  // This function returns all exchanges listed in the database
  // input format = Search,E
  //                  0       1

  // output format = EXCHANGE,*

  QSQuoteDataBase db(dbPath);
  db.exchanges(output);

  return 0;
}

//**********************************************************
//**********************************************************
//**********************************************************

QSPlugin * createPlugin ()
{
  Search *o = new Search;
  return ((QSPlugin *) o);
}

