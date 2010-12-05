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

#include "QUOTE_DATABASE.h"
#include "Strip.h"
#include "QuoteBar.h"
#include "DateRange.h"
#include "Bar.h"

#include <QtDebug>
#include <QDateTime>

QUOTE_DATABASE::QUOTE_DATABASE ()
{
  _method << "GET" << "SET" << "DELETE" << "EXCHANGE" << "RENAME" << "SEARCH" << "TRANSACTION" << "COMMIT";

  init();
}

void QUOTE_DATABASE::init ()
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
      qDebug() << "QUOTE_DATABASE::init:" << _db.lastError().text();
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
    qDebug() << "QUOTE_DATABASE::init:" << _db.lastError().text();
}

int QUOTE_DATABASE::command (Command &command)
{
  if (command.count() < 2)
  {
    qDebug() << "QUOTE_DATABASE::command: invalid parm count" << command.count();
    return 1;
  }

  switch ((Method) _method.indexOf(command.parm(1)))
  {
    case _GET:
      return getBars(command);
      break;
    case _SET:
      return setBars(command);
      break;
    case _DELETE:
      return deleteSymbol(command);
      break;
    case _EXCHANGE:
      return getExchange(command);
      break;
    case _RENAME:
      return rename(command);
      break;
    case _SEARCH:
      return search(command);
      break;
    case _TRANSACTION:
      return transaction(command);
      break;
    case _COMMIT:
      return commit(command);
      break;
    default:
      break;
  }

  return 0;
}

int QUOTE_DATABASE::getBars (Command &command)
{
  // QUOTE_DATABASE,<METHOD>,<EXCHANGE>,<SYMBOL>,<LENGTH>,<START DATE>,<END DATE>,<DATE RANGE>
  //   0      1         2         3         4          5          6           7

  if (command.count() != 8)
  {
    qDebug() << "QUOTE_DATABASE::getBars: invalid parm count" << command.count();
    return 1;
  }

  BarData *bd = command.barData();
  if (! bd)
  {
    qDebug() << "QUOTE_DATABASE::getBars: missing bar data";
    return 1;
  }

  int pos = 2;
  QString exchange = command.parm(pos);
  if (exchange.isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::getBars: invalid exchange" << command.parm(pos);
    return 1;
  }

  pos++;
  QString tsymbol = command.parm(pos);
  if (tsymbol.isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::getBars: invalid symbol" << command.parm(pos);
    return 1;
  }

  pos++;
  QString s = command.parm(pos);
  QStringList l;
  Bar tbar;
  tbar.lengthList(l);
  int length = l.indexOf(s);
  if (length == -1)
  {
    qDebug() << "QUOTE_DATABASE::getBars: invalid length" << command.parm(pos);
    return 1;
  }

  pos++;
  QDateTime startDate = QDateTime::fromString(command.parm(pos), Qt::ISODate);

  pos++;
  QDateTime endDate = QDateTime::fromString(command.parm(pos), Qt::ISODate);

  pos++;
  QString dateRange = command.parm(pos);
  if (dateRange.isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::getBars: invalid date range" << command.parm(pos);
    return 1;
  }

  Symbol symbol;
  symbol.setExchange(exchange);
  symbol.setSymbol(tsymbol);
  if (getSymbol(&symbol))
    return 1;
  
  QSqlQuery q(_db);

  QHash<QString, Bar *> bars;
  QList<Bar *> dateList;

  // check if we want to return most recent bars
  if (! startDate.isValid() && ! endDate.isValid())
  {
    s = "SELECT max(date) FROM " + symbol.table();
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "QUOTE_DATABASE::getBars:" + q.lastError().text();
      return 1;
    }

    if (q.next())
    {
      endDate = QDateTime::fromString(q.value(0).toString(), "yyyyMMddHHmmss");
      DateRange dr;
      if (dr.dateRange((DateRange::Range) dateRange.toInt(), endDate, startDate))
      {
        qDebug() << "QUOTE_DATABASE::getBars: date range error" << dateRange;
        return 1;
      }
    }
    else
      return 1;
  }

  s = "SELECT date,open,high,low,close,volume,oi";
  s.append(" FROM " + symbol.table());
  s.append(" WHERE date >=" + startDate.toString("yyyyMMddHHmmss"));
  s.append(" AND date <=" + endDate.toString("yyyyMMddHHmmss"));
  s.append(" ORDER BY date DESC");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QUOTE_DATABASE::getBars:" + q.lastError().text();
    return 1;
  }

  while (q.next())
  {
    QDateTime lastDate = QDateTime::fromString(q.value(0).toString(), "yyyyMMddHHmmss");

    Bar tbar;
    tbar.setDateRange(lastDate, (Bar::BarLength) length);
    tbar.rangeKey(s);

    Bar *bar = bars.value(s);
    if (! bar)
    {
      bar = new Bar;
      bar->setDateRange(lastDate, (Bar::BarLength) length);
      bar->setOpen(q.value(1).toDouble());
      bar->setHigh(q.value(2).toDouble());
      bar->setLow(q.value(3).toDouble());
      bar->setClose(q.value(4).toDouble());
      bar->setVolume(q.value(5).toDouble());
      bar->setOI(q.value(6).toDouble());

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
  l.clear();
  for (loop = dateList.count() - 1; loop > -1; loop--)
  {
    Bar *bar = dateList.at(loop);
    if (! bar)
      continue;

    bd->append(bar);    
  }

  command.setReturnData("0");

  emit signalDone();

  return 0;
}

int QUOTE_DATABASE::setBars (Command &command)
{
  // QUOTE_DATABASE,<METHOD>,<FORMAT>,<DATA>
  //   0      1         2      3
  // format delimited by a colon
  // data delimted by a colon between fields

  if (command.count() != 4)
  {
    qDebug() << "QUOTE_DATABASE::setBars: invalid parm count" << command.count();
    return 1;
  }

  QStringList formatList;
  formatList << "D" << "O" << "H" << "L" << "C" << "V" << "I" << "X" << "S" << "N" << "T";

  // validate format parms
  QStringList format = command.parm(2).split(":", QString::SkipEmptyParts);

  if (! format.contains("D") || ! format.contains("X") || ! format.contains("S"))
  {
    qDebug() << "QUOTE_DATABASE::setBars: format must contain at least D,X,S fields" << command.parm(2);
    return 1;
  }

  int tloop = 0;
  int flag = 0;
  for (; tloop < format.count(); tloop++)
  {
    if (formatList.indexOf(format.at(tloop)) == -1)
    {
      flag = 1;
      break;
    }
  }
    
  if (flag)
  {
    qDebug() << "QUOTE_DATABASE::setBars: invalid format parm" << command.parm(2);
    return 1;
  }

  // validate data fields
  QStringList data = command.parm(3).split(":", QString::SkipEmptyParts);

  if (data.count() != format.count())
  {
    qDebug() << "QUOTE_DATABASE::setBars: # of format fields not equal to # data fields" << command.parm(3);
    return 1;
  }

  QuoteBar *bar = new QuoteBar;
  flag = 0;
  for (tloop = 0; tloop < format.count(); tloop++)
  {
    switch (formatList.indexOf(format.at(tloop)))
    {
      case 0: // D
        flag = bar->setDate(data.at(tloop));
        break;
      case 1: // O
        flag = bar->setOpen(data.at(tloop));
        break;
      case 2: // H
        flag = bar->setHigh(data.at(tloop));
        break;
      case 3: // L
        flag = bar->setLow(data.at(tloop));
        break;
      case 4: // C
        flag = bar->setClose(data.at(tloop));
        break;
      case 5: // V
        flag = bar->setVolume(data.at(tloop));
        break;
      case 6: // I
        flag = bar->setOI(data.at(tloop));
        break;
      case 7: // X
        bar->setExchange(data.at(tloop));
        break;
      case 8: // S
        bar->setSymbol(data.at(tloop));
        break;
      case 9: // N
        bar->setName(data.at(tloop));
        break;
      case 10: // T
        bar->setType(data.at(tloop));
        break;
      default:
        break;
    }
  }

  if (flag)
  {
    delete bar;
    qDebug() << "QUOTE_DATABASE::setBars: data error" << command.parm(3);
    return 1;
  }

  Symbol *symbol = new Symbol;
  symbol->setExchange(bar->exchange());
  symbol->setSymbol(bar->symbol());
  symbol->setName(bar->name());
  symbol->setType(bar->type());

  // save bars
  QSqlQuery q(_db);

  if (getSymbol(symbol))
  {
    if (newSymbol(symbol))
    {
      delete symbol;
      delete bar;
      return 1;
    }
  }
  
  QDateTime dt = bar->startDate();
  QString date = dt.toString("yyyyMMddHHmmss");

  // first check if record exists so we know to do an update or insert
  QString s = "SELECT date FROM " + symbol->table() + " WHERE date =" + date;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QUOTE_DATABASE::setBars:" << q.lastError().text();
    qDebug() << s;
    delete symbol;
    delete bar;
    return 1;
  }

  if (q.next()) // record exists, use update
  {
    s = "UPDATE " + symbol->table();

    QStringList tl;
    if (! bar->open().isEmpty())
      tl << " open=" + bar->open();
        
    if (! bar->high().isEmpty())
      tl << " high=" + bar->high();
        
    if (! bar->low().isEmpty())
      tl << " low=" + bar->low();
        
    if (! bar->close().isEmpty())
      tl << " close=" + bar->close();
        
    if (! bar->volume().isEmpty())
      tl << " volume=" + bar->volume();
        
    if (! bar->oi().isEmpty())
      tl << " oi=" + bar->oi();

    if (tl.count())
    {
      s.append(" SET");
      s.append(tl.join(","));
      s.append(" WHERE date=" + date);
    }
  }
  else // new record, use insert
  {
    s = "INSERT INTO " + symbol->table() + " (";

    QStringList tl;
    tl << "date";

    QString s2 = "VALUES(";

    QStringList tl2;
    tl2 << date;
        
    if (! bar->open().isEmpty())
    {
      tl << "open";
      tl2 << bar->open();
    }
        
    if (! bar->high().isEmpty())
    {
      tl << "high";
      tl2 << bar->high();
    }
        
    if (! bar->low().isEmpty())
    {
      tl << "low";
      tl2 << bar->low();
    }
        
    if (! bar->close().isEmpty())
    {
      tl << "close";
      tl2 << bar->close();
    }
        
    if (! bar->volume().isEmpty())
    {
      tl << "volume";
      tl2 << bar->volume();
    }
        
    if (! bar->oi().isEmpty())
    {
      tl << "oi";
      tl2 << bar->oi();
    }

    if (tl.count() > 1)
    {
      s.append(tl.join(","));
      s.append(")");

      s2.append(tl2.join(","));
      s2.append(")");
      s.append(s2);
    }
  }

  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QUOTE_DATABASE::setBars:" << q.lastError().text();
    qDebug() << s;
    delete bar;
    delete symbol;
    return 1;    
  }

  delete bar;
  delete symbol;
  
  command.setReturnData("0");

  emit signalDone();

  return 0;
}

//***************
//***************
//***************

int QUOTE_DATABASE::newSymbol (Symbol *symbol)
{
  if (symbol->exchange().isEmpty() || symbol->symbol().isEmpty() || symbol->type().isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::newSymbol: exchange/symbol/type missing";
    return 1;
  }

  if (! getSymbol(symbol))
  {
    qDebug() << "QUOTE_DATABASE::newSymbol: duplicate symbol";
    return 1;
  }

  QSqlQuery q(_db);

  // create the symbol
  // we use the maximum record column and add 1 to it to generate a new table name
  QString s = "SELECT max(record) FROM symbolIndex";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QUOTE_DATABASE::newSymbol: get record max " + q.lastError().text();
    return 1;
  }

  symbol->setTable("Q1");
  if (q.next())
    symbol->setTable("Q" + QString::number(q.value(0).toInt() + 1));

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
    qDebug() << "QUOTE_DATABASE::newSymbol:" + q.lastError().text();
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
    qDebug() << "QUOTE_DATABASE::newSymbol:" + q.lastError().text();
    return 1;
  }

  return 0;
}

int QUOTE_DATABASE::getSymbol (Symbol *symbol)
{
  if (symbol->exchange().isEmpty() || symbol->symbol().isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::symbol: exchange and/or symbol missing";
    return 1;
  }
  
  QSqlQuery q(_db);
  QString s = "SELECT name,type,tableName FROM symbolIndex";
  s.append(" WHERE symbol='" + symbol->symbol() + "'");
  s.append(" AND exchange='" + symbol->exchange() + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QUOTE_DATABASE::symbol:" + q.lastError().text();
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

int QUOTE_DATABASE::deleteSymbol (Command &command)
{
  // QUOTE_DATABASE,DELETE,<EXCHANGE>,<SYMBOL>
  //         0         1        2        3

  if (command.count() != 4)
  {
    qDebug() << "QUOTE_DATABASE::deleteSymbol: invalid parm count" << command.count();
    return 1;
  }

  int pos = 2;
  QString exchange = command.parm(pos);
  if (exchange.isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::deleteSymbol: invalid exchange" << command.parm(pos);
    return 1;
  }

  pos++;
  QString symbol = command.parm(pos);
  if (symbol.isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::deleteSymbol: invalid symbol" << command.parm(pos);
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
    qDebug() << "QUOTE_DATABASE::deleteSymbol:" + q.lastError().text();
    return 1;
  }

  QString table;
  if (! q.next())
  {
    qDebug() << "QUOTE_DATABASE::deleteSymbol: symbol not found";
    return 1;
  }
  else
    table = q.value(0).toString();

  _db.transaction();
  
  // drop quote table
  s = "DROP TABLE " + table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QUOTE_DATABASE::deleteSymbol:" + q.lastError().text();
    return 1;
  }

  // remove index record
  s = "DELETE FROM symbolIndex";
  s.append(" WHERE symbol='" + symbol + "'");
  s.append(" AND exchange='" + exchange + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QUOTE_DATABASE::deleteSymbol:" + q.lastError().text();
    return 1;
  }

  _db.commit();
  
  command.setReturnData("0");

  emit signalDone();

  return 0;
}

int QUOTE_DATABASE::getExchange (Command &command)
{
  // QUOTE_DATABASE,EXCHANGE
  //       0           1

  QSqlQuery q(_db);
  QString s = "SELECT DISTINCT exchange FROM symbolIndex ORDER BY exchange ASC";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QUOTE_DATABASE::getExchange:" << q.lastError().text();
    return 1;
  }

  QStringList l;
  while (q.next())
    l << q.value(0).toString();

  command.setReturnData(l.join(","));

  emit signalDone(l.join(","));

  return 0;
}

int QUOTE_DATABASE::rename (Command &command)
{
  // QUOTE_DATABASE,RENAME,<OLD EXCHANGE>,<OLD SYMBOL>,<NEW EXCHANGE>,<NEW SYMBOL>
  //          0       1           2             3             4            5

  if (command.count() != 6)
  {
    qDebug() << "QUOTE_DATABASE::rename: invalid parm count" << command.count();
    return 1;
  }

  int pos = 2;
  QString oexchange = command.parm(pos);
  if (oexchange.isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::rename: invalid old exchange" << command.parm(pos);
    return 1;
  }

  pos++;
  QString osymbol = command.parm(pos);
  if (osymbol.isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::rename: invalid old symbol" << command.parm(pos);
    return 1;
  }

  pos++;
  QString nexchange = command.parm(pos);
  if (nexchange.isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::rename: invalid new exchange" << command.parm(pos);
    return 1;
  }

  pos++;
  QString nsymbol = command.parm(pos);
  if (nsymbol.isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::rename: invalid new symbol" << command.parm(pos);
    return 1;
  }

  QSqlQuery q(_db);

  // make sure old symbol exists
  QString s = "SELECT tableName FROM symbolIndex";
  s.append(" WHERE symbol='" + osymbol + "'");
  s.append(" AND exchange='" + oexchange + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QUOTE_DATABASE::rename:" + q.lastError().text();
    return 1;
  }

  if (! q.next())
  {
    qDebug() << "QUOTE_DATABASE::rename: old exchange/symbol not found";
    return 1;
  }

  // make sure new symbol does not exist
  s = "SELECT tableName FROM symbolIndex";
  s.append(" WHERE symbol='" + nsymbol + "'");
  s.append(" AND exchange='" + nexchange + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QUOTE_DATABASE::rename:" + q.lastError().text();
    return 1;
  }

  if (q.next())
  {
    qDebug() << "QUOTE_DATABASE::rename: new exchange/symbol exists";
    return 1;
  }

  _db.transaction();

  s = "UPDATE symbolIndex";
  s.append(" SET symbol='" + nsymbol + "'");
  s.append(", exchange='" + nexchange + "'");
  s.append(" WHERE symbol='" + osymbol + "'");
  s.append(" AND exchange='" + oexchange + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QUOTE_DATABASE::rename:" + q.lastError().text();
    return 1;
  }

  _db.commit();

  command.setReturnData("0");

  emit signalDone();

  return 0;
}

int QUOTE_DATABASE::search (Command &command)
{
  // QUOTE_DATABASE,SEARCH,<EXCHANGE>,<SYMBOL>
  //       0          1         2         3

  if (command.count() != 4)
  {
    qDebug() << "QUOTE_DATABASE::search: invalid parm count" << command.count();
    return 1;
  }

  int pos = 2;
  QString exchange = command.parm(pos);
  if (exchange.isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::search: invalid exchange" << command.parm(pos);
    return 1;
  }

  pos++;
  QString symbol = command.parm(pos);
  if (symbol.isEmpty())
  {
    qDebug() << "QUOTE_DATABASE::search: invalid symbol" << command.parm(pos);
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
    qDebug() << "QUOTE_DATABASE::search:" + q.lastError().text();
    return 1;
  }

  QStringList l;

  // we create exchange,symbol,name tuples comma delimiter
  while (q.next())
  {
    QStringList tl;
    int loop = 0;
    for (; loop < 3; loop++)
      tl << q.value(loop).toString();
    l.append(tl.join(","));
  }

  // now delimit each tuple with a semicolon
  command.setReturnData(l.join(";"));

  emit signalDone(l.join(";"));

  return 0;
}

int QUOTE_DATABASE::transaction (Command &command)
{
  // QUOTE_DATABASE,TRANSACTION
  //        0            1

  _db.transaction();

  command.setReturnData("0");

  emit signalDone();

  return 0;
}

int QUOTE_DATABASE::commit (Command &command)
{
  // QUOTE_DATABASE,COMMIT
  //        0         1

  _db.commit();

  command.setReturnData("0");

  emit signalDone();

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  QUOTE_DATABASE *o = new QUOTE_DATABASE;
  return ((ScriptPlugin *) o);
}
