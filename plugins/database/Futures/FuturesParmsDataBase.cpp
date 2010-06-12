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

#include "FuturesParmsDataBase.h"
#include "FuturesDataBase.h"

#include <QtSql>
#include <QtDebug>
#include <QString>

FuturesParmsDataBase::FuturesParmsDataBase ()
{
}

int FuturesParmsDataBase::add (BarData *symbol)
{
  transaction();
  
  // create a parm table if needed
  QString s = "CREATE TABLE IF NOT EXISTS futuresParms (";
  s.append(" record INTEGER PRIMARY KEY AUTOINCREMENT");
  s.append(", symbol TEXT");
  s.append(", exchange TEXT");
  s.append(", code TEXT");
  s.append(", month TEXT");
  s.append(", year INT");
  s.append(")");
  int rc = command(s, QString("FuturesParmsDataBase::add: create new parm table"));
  
  // add new record
  s = "INSERT OR REPLACE INTO futuresParms (symbol,exchange) VALUES(";
  s.append("'" + symbol->getSymbol() + "'");
  s.append(",'" + symbol->getExchange() + "'");
  s.append(")");
  rc = command(s, QString("FuturesParmsDataBase::add: add new record: "));

  commit();
  
  return rc;
}

int FuturesParmsDataBase::remove (BarData *symbol)
{
  transaction();

  // remove parms record
  QString s = "DELETE FROM futuresParms";
  s.append(" WHERE symbol='" + symbol->getSymbol() + "'");
  s.append(" AND exchange='" + symbol->getExchange() + "'");
  if (command(s, QString("FuturesParmsDataBase::remove")))
    return 1;

  commit();

  return 0;
}

int FuturesParmsDataBase::rename (BarData *osymbol, BarData *nsymbol)
{
  transaction();

  // update futures parms with new symbol name
  QString s = "UPDATE futuresParms";
  s.append(" SET symbol='" + nsymbol->getSymbol() + "'");
  s.append(", exchange='" + nsymbol->getExchange() + "'");
  s.append(" WHERE symbol='" + osymbol->getSymbol() + "'");
  s.append(" AND exchange='" + osymbol->getExchange() + "'");
  if (command(s, QString("FuturesParmsDataBase::rename")))
    return 1;

  commit();

  return 0;
}

int FuturesParmsDataBase::setYear (BarData *symbol, int year)
{
  transaction();

  QString s = "UPDATE futuresParms SET year=" + QString::number(year);
  s.append(" WHERE symbol='" + symbol->getSymbol() + "'");
  s.append(" AND exchange='" + symbol->getExchange() + "'");
  if (command(s, QString("FuturesParmsDataBase::setYear")))
    return 1;

  commit();

  return 0;
}

int FuturesParmsDataBase::setMonth (BarData *symbol, QString &month)
{
  transaction();

  QString s = "UPDATE futuresParms SET month='" + month + "'";
  s.append(" WHERE symbol='" + symbol->getSymbol() + "'");
  s.append(" AND exchange='" + symbol->getExchange() + "'");
  if (command(s, QString("FuturesParmsDataBase::setMonth")))
    return 1;

  commit();

  return 0;
}

int FuturesParmsDataBase::setCode (BarData *symbol, QString &code)
{
  transaction();

  QString s = "UPDATE futuresParms SET code='" + code + "'";
  s.append(" WHERE symbol='" + symbol->getSymbol() + "'");
  s.append(" AND exchange='" + symbol->getExchange() + "'");
  if (command(s, QString("FuturesParmsDataBase::setCode")))
    return 1;

  commit();

  return 0;
}


