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

YahooDataBase::YahooDataBase ()
{
  _table = "data";
  
  _db = QSqlDatabase::database(_table);
  QSqlQuery q(_db);

  // create the table
  QString s = "CREATE TABLE IF NOT EXISTS YahooSymbols (";
  s.append("ysymbol TEXT PRIMARY KEY UNIQUE"); // yahoo symbol
  s.append(", symbol TEXT"); // actual symbol
  s.append(", exchange TEXT"); // actual exchange
//  s.append(", lastDate TEXT"); // last datetime downloaded
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "YahooDataBase::YahooDataBase: " << q.lastError().text();
}

int YahooDataBase::load (Setting &set)
{
  QString ysymbol = set.data("YSYMBOL");

  QSqlQuery q(_db);

  QString s = "SELECT exchange,symbol FROM YahooSymbols WHERE ysymbol='" + ysymbol + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "YahooDataBase::load" << q.lastError().text();
    return 1;
  }

  if (q.next())
  {
    set.setData("EXCHANGE", q.value(0).toString());
    set.setData("SYMBOL", q.value(1).toString());
  }

  return 0;
}

int YahooDataBase::save (Setting &set)
{
  QSqlQuery q(_db);

  QString s = "INSERT OR REPLACE INTO YahooSymbols (ysymbol,exchange,symbol) VALUES (";
  s.append("'" + set.data("YSYMBOL") + "'");
  s.append(",'" + set.data("EXCHANGE") + "'");
  s.append(",'" + set.data("SYMBOL") + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "YahooDataBase::save" << q.lastError().text();
    return 1;
  }

  return 0;
}

int YahooDataBase::deleteSymbol (Setting &set)
{
  QSqlQuery q(_db);

  QString s = "DELETE FROM YahooSymbols WHERE ysymbol='" + set.data("YSYMBOL") + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "YahooDataBase::deleteSymbol" << q.lastError().text();
    return 1;
  }

  return 0;
}

int YahooDataBase::symbols (QStringList &l)
{
  l.clear();
  
  QSqlQuery q(_db);

  QString s = "SELECT ysymbol FROM YahooSymbols ORDER BY ysymbol ASC";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "YahooDataBase::symbols" << q.lastError().text();
    return 1;
  }

  while (q.next())
    l << q.value(0).toString();

  return 0;
}

void YahooDataBase::transaction ()
{
  _db.transaction();
}

void YahooDataBase::commit ()
{
  _db.commit();
}
