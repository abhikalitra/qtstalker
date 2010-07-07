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
#include "QSLog.h"

#include <QtDebug>

Search::Search ()
{
}

int Search::command (QStringList &input, QString &dbPath, QString &output)
{
  // input format = Search,<EXCHANGE>,<SYMBOL>
  //                  0        1         2
  
  // output format = EXCHANGE,SYMBOL,*

  output = "ERROR";

  QSLog log;

  if (input.count() != 3)
  {
    log.message(QSLog::Error, QString("Search: invalid parm count " + QString::number(input.count())));
    return 1;
  }

  if (input.at(1).isEmpty())
  {
    log.message(QSLog::Error, QString("Search: invalid exchange " + input.at(1)));
    return 1;
  }
  
  if (input.at(2).isEmpty())
  {
    log.message(QSLog::Error, QString("Search: invalid symbol " + input.at(2)));
    return 1;
  }

  QSQuoteDataBase db(dbPath);
  if (db.search(input[1], input[2], output))
   return 1;

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

