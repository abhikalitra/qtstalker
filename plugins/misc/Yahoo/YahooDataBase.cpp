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

#include "YahooDataBase.h"

#include <QtDebug>
#include <QtSql>

YahooDataBase::YahooDataBase ()
{
  // create the table
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "CREATE TABLE IF NOT EXISTS YahooPlugin (";
  s.append("ysymbol TEXT PRIMARY KEY UNIQUE"); // yahoo symbol
  s.append(", symbol TEXT"); // actual symbol
  s.append(", exchange TEXT"); // actual exchange
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "YahooDataBase::YahooDataBase: " << q.lastError().text();
}

void YahooDataBase::getSymbols (QStringList &l)
{
  l.clear();
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT ysymbol FROM YahooPlugin ORDER BY ysymbol ASC";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "YahooDataBase::getSymbols: " << q.lastError().text();
    return;
  }

  while (q.next())
    l.append(q.value(0).toString());
}

void YahooDataBase::setSymbol (QString &ysymbol, QString &symbol, QString &exchange)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "INSERT OR REPLACE INTO YahooPlugin (ysymbol,symbol,exchange) VALUES (";
  s.append("'" + ysymbol + "'");
  s.append(",'" + symbol + "'");
  s.append(",'" + exchange + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "YahooDataBase::setSymbol" << q.lastError().text();
}

void YahooDataBase::getSymbol (QString &ysymbol, QString &symbol, QString &exchange)
{
  symbol.clear();
  exchange.clear();
  
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT symbol,exchange FROM YahooPlugin WHERE ysymbol='" + ysymbol + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "YahooDataBase::getSymbol" << q.lastError().text();

  if (q.next())
  {
    symbol = q.value(0).toString();
    exchange = q.value(1).toString();
  }
}

void YahooDataBase::deleteSymbol (QString &ysymbol)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "DELETE FROM YahooPlugin WHERE ysymbol='" + ysymbol + "'";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "YahooDataBase::deleteSymbol" << q.lastError().text();
}

