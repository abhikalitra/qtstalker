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


#include "QuoteDataBase.h"
#include "Strip.h"
#include "CurveBar.h"
#include "CurveData.h"
#include "DateRange.h"
#include "BarLength.h"
#include "Symbol.h"

#include <QDateTime>
#include <QtDebug>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

QuoteDataBase::QuoteDataBase ()
{
  init();
}

void QuoteDataBase::init ()
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
      qDebug() << "QuoteDataBase::init:" << _db.lastError().text();
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
    qDebug() << "QuoteDataBase::init:" << _db.lastError().text();
}

int QuoteDataBase::getBars (Data *bd)
{
  QString exchange = bd->get(Symbol::_EXCHANGE).toString();
  if (exchange.isEmpty())
  {
    qDebug() << "QuoteDataBase::getBars: invalid EXCHANGE" << exchange;
    return 1;
  }

  QString tsymbol = bd->get(Symbol::_SYMBOL).toString();
  if (tsymbol.isEmpty())
  {
    qDebug() << "QuoteDataBase::getBars: invalid SYMBOL" << tsymbol;
    return 1;
  }

  int length = bd->get(Symbol::_LENGTH).toInt();
  if (length == -1)
  {
    qDebug() << "QuoteDataBase::getBars: invalid length";
    return 1;
  }

  QDateTime startDate = bd->get(Symbol::_START_DATE).toDateTime();
  QDateTime endDate = bd->get(Symbol::_END_DATE).toDateTime();

  int dateRange = bd->get(Symbol::_RANGE).toInt();

  if (getSymbol(bd))
    return 1;

  QSqlQuery q(_db);

  // check if we want to return most recent bars
  if (! startDate.isValid() && ! endDate.isValid())
  {
    QString s = "SELECT max(date) FROM " + bd->get(Symbol::_TABLE).toString();
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "QuoteDataBase::getBars:" + q.lastError().text();
      return 1;
    }

    if (q.next())
    {
      endDate = QDateTime::fromString(q.value(0).toString(), "yyyyMMddHHmmss");
      DateRange dr;
      if (dr.dateRange((DateRange::Range) dateRange, endDate, startDate))
      {
        qDebug() << "QuoteDataBase::getBars: date range error" << dateRange;
        return 1;
      }
    }
    else
      return 1;
  }

  QString s = "SELECT date,open,high,low,close,volume,oi";
  s.append(" FROM " + bd->get(Symbol::_TABLE).toString());
  s.append(" WHERE date >=" + startDate.toString("yyyyMMddHHmmss"));
  s.append(" AND date <=" + endDate.toString("yyyyMMddHHmmss"));
  s.append(" ORDER BY date DESC");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::getBars:" + q.lastError().text();
    return 1;
  }

  QHash<QString, CurveBar *> bars;
  QList<CurveBar *> order;
  DateRange dr;

  while (q.next())
  {
    QDateTime lastDate = QDateTime::fromString(q.value(0).toString(), "yyyyMMddHHmmss");

    QDateTime tsd, ted;
    dr.dateInterval(lastDate, (BarLength::Length) length, tsd, ted);
    s = dr.rangeKey(tsd, ted);

    CurveBar *bar = bars.value(s);
    if (! bar)
    {
      bar = new CurveBar;
//      bar->setDateRange(lastDate, (BarLength::Length) length);
      bar->set(CurveBar::_OPEN, QVariant(q.value(1).toDouble()));
      bar->set(CurveBar::_HIGH, QVariant(q.value(2).toDouble()));
      bar->set(CurveBar::_LOW, QVariant(q.value(3).toDouble()));
      bar->set(CurveBar::_CLOSE, QVariant(q.value(4).toDouble()));
      bar->set(CurveBar::_VOLUME, QVariant(q.value(5).toDouble()));
      bar->set(CurveBar::_OI, QVariant(q.value(6).toDouble()));
      bar->set(CurveBar::_DATE, QVariant(lastDate));
      bars.insert(s, bar);
      order.prepend(bar);
    }
    else
    {
      bar->set(CurveBar::_OPEN, QVariant(q.value(1).toDouble()));

      double v = q.value(2).toDouble();
      double t = bar->get(CurveBar::_HIGH).toDouble();
      if (v > t)
        bar->set(CurveBar::_HIGH, QVariant(q.value(2).toDouble()));

      v = q.value(3).toDouble();
      t = bar->get(CurveBar::_LOW).toDouble();
      if (v < t)
        bar->set(CurveBar::_LOW, QVariant(q.value(3).toDouble()));

      v = q.value(5).toDouble();
      v += bar->get(CurveBar::_VOLUME).toDouble();
      bar->set(CurveBar::_VOLUME, QVariant(v));

      bar->set(CurveBar::_OI, QVariant(q.value(6).toDouble()));
    }
  }

  // insert bars into Symbol bars in proper order
  int loop = 0;
  for (; loop < order.count(); loop++)
    bd->set(loop, order.at(loop));

  return 0;
}

int QuoteDataBase::setBars (Data *symbol)
{
  // save bars
  QSqlQuery q(_db);

  if (getSymbol(symbol))
  {
    if (newSymbol(symbol))
      return 1;
  }
  else
  {
    // check if we need to save the name
    if (! symbol->get(Symbol::_NAME).toString().isEmpty())
      setName(symbol);
  }

  QList<int> keys = symbol->barKeys();
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = (CurveBar *) symbol->getData(keys.at(loop));
    if (! bar)
      continue;

    QDateTime dt = bar->get(CurveBar::_DATE).toDateTime();
    QString date = dt.toString("yyyyMMddHHmmss");

    // first check if record exists so we know to do an update or insert
    QString s = "SELECT date FROM " + symbol->get(Symbol::_TABLE).toString() + " WHERE date =" + date;
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "QuoteDataBase::setBars:" << q.lastError().text();
      qDebug() << s;
      continue;
    }

    if (q.next()) // record exists, use update
    {
      s = "UPDATE " + symbol->get(Symbol::_TABLE).toString() + " SET ";
      s.append("open=" + bar->get(CurveBar::_OPEN).toString());
      s.append(", high=" + bar->get(CurveBar::_HIGH).toString());
      s.append(", low=" + bar->get(CurveBar::_LOW).toString());
      s.append(", close=" + bar->get(CurveBar::_CLOSE).toString());
      s.append(", volume=" + bar->get(CurveBar::_VOLUME).toString());
      s.append(", oi=" + bar->get(CurveBar::_OI).toString());
      s.append(" WHERE date=" + date);
    }
    else // new record, use insert
    {
      s = "INSERT INTO " + symbol->get(Symbol::_TABLE).toString() + " (date,open,high,low,close,volume,oi) VALUES(";
      s.append(date);
      s.append("," + bar->get(CurveBar::_OPEN).toString());
      s.append("," + bar->get(CurveBar::_HIGH).toString());
      s.append("," + bar->get(CurveBar::_LOW).toString());
      s.append("," + bar->get(CurveBar::_CLOSE).toString());
      s.append("," + bar->get(CurveBar::_VOLUME).toString());
      s.append("," + bar->get(CurveBar::_OI).toString());
      s.append(")");
    }

    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "QuoteDataBase::setBars:" << q.lastError().text();
      qDebug() << s;
      continue;
    }
  }

  return 0;
}

//***************
//***************
//***************

int QuoteDataBase::newSymbol (Data *symbol)
{
  QString exchange = symbol->get(Symbol::_EXCHANGE).toString();
  if (exchange.isEmpty())
  {
    qDebug() << "QuoteDataBase::newSymbol: invalid EXCHANGE" << exchange;
    return 1;
  }

  QString tsymbol = symbol->get(Symbol::_SYMBOL).toString();
  if (tsymbol.isEmpty())
  {
    qDebug() << "QuoteDataBase::newSymbol: invalid SYMBOL" << tsymbol;
    return 1;
  }

  QString type = symbol->get(Symbol::_TYPE).toString();
  if (type.isEmpty())
  {
    qDebug() << "QuoteDataBase::newSymbol: invalid TYPE" << type;
    return 1;
  }

  if (! getSymbol(symbol))
  {
    qDebug() << "QuoteDataBase::newSymbol: duplicate symbol";
    return 1;
  }

  QSqlQuery q(_db);

  // create the symbol
  // we use the maximum record column and add 1 to it to generate a new table name
  QString s = "SELECT max(record) FROM symbolIndex";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::newSymbol: get record max " + q.lastError().text();
    return 1;
  }

  QString table = "Q1";
  if (q.next())
    table = "Q" + QString::number(q.value(0).toInt() + 1);
  symbol->set(Symbol::_TABLE, QVariant(table));

  // set default name to symbol
  QString name = symbol->get(Symbol::_NAME).toString();
  if (name.isEmpty())
  {
    name = tsymbol;
    symbol->set(Symbol::_NAME, QVariant(name));
  }

  // add new symbol entry into the symbolIndex table
  s = "INSERT OR REPLACE INTO symbolIndex (symbol,tableName,exchange,name,type) VALUES(";
  s.append("'" + tsymbol + "'");
  s.append(",'" + table + "'");
  s.append(",'" + exchange + "'");
  s.append(",'" + name + "'");
  s.append(",'" + type + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::newSymbol:" + q.lastError().text();
    return 1;
  }

  s = "CREATE TABLE IF NOT EXISTS " + table + " (";
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
    qDebug() << "QuoteDataBase::newSymbol:" + q.lastError().text();
    return 1;
  }

  return 0;
}

int QuoteDataBase::getSymbol (Data *symbol)
{
  QString exchange = symbol->get(Symbol::_EXCHANGE).toString();
  if (exchange.isEmpty())
  {
    qDebug() << "QuoteDataBase::getSymbol: invalid EXCHANGE" << exchange;
    return 1;
  }

  QString tsymbol = symbol->get(Symbol::_SYMBOL).toString();
  if (tsymbol.isEmpty())
  {
    qDebug() << "QuoteDataBase::getSymbol: invalid SYMBOL" << tsymbol;
    return 1;
  }

  QSqlQuery q(_db);
  QString s = "SELECT name,type,tableName FROM symbolIndex";
  s.append(" WHERE symbol='" + tsymbol + "'");
  s.append(" AND exchange='" + exchange + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::getSymbol:" + q.lastError().text();
    return 1;
  }

  if (q.next())
  {
    int pos = 0;
    symbol->set(Symbol::_NAME, QVariant(q.value(pos++).toString()));
    symbol->set(Symbol::_TYPE, QVariant(q.value(pos++).toString()));
    symbol->set(Symbol::_TABLE, QVariant(q.value(pos++).toString()));
    return 0;
  }

  return 1;
}

int QuoteDataBase::deleteSymbol (Data *bd)
{
  QString exchange = bd->get(Symbol::_EXCHANGE).toString();
  if (exchange.isEmpty())
  {
    qDebug() << "QuoteDataBase::deleteSymbol: invalid EXCHANGE" << exchange;
    return 1;
  }

  QString symbol = bd->get(Symbol::_SYMBOL).toString();
  if (symbol.isEmpty())
  {
    qDebug() << "QuoteDataBase::deleteSymbol: invalid SYMBOL" << symbol;
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
    qDebug() << "QuoteDataBase::deleteSymbol:" + q.lastError().text();
    return 1;
  }

  QString table;
  if (! q.next())
  {
    qDebug() << "QuoteDataBase::deleteSymbol: symbol not found";
    return 1;
  }
  else
    table = q.value(0).toString();

  // drop quote table
  s = "DROP TABLE " + table;
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::deleteSymbol:" + q.lastError().text();
    return 1;
  }

  // remove index record
  s = "DELETE FROM symbolIndex";
  s.append(" WHERE symbol='" + symbol + "'");
  s.append(" AND exchange='" + exchange + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::deleteSymbol:" + q.lastError().text();
    return 1;
  }

  return 0;
}

int QuoteDataBase::getExchange (QStringList &l)
{
  l.clear();

  QSqlQuery q(_db);
  QString s = "SELECT DISTINCT exchange FROM symbolIndex ORDER BY exchange ASC";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::getExchange:" << q.lastError().text();
    return 1;
  }

  while (q.next())
    l << q.value(0).toString();

  return 0;
}

int QuoteDataBase::rename (Data *osymbol, Data *nsymbol)
{
  QString oe = osymbol->get(Symbol::_EXCHANGE).toString();
  if (oe.isEmpty())
  {
    qDebug() << "QuoteDataBase::rename: invalid old exchange" << oe;
    return 1;
  }

  QString os = osymbol->get(Symbol::_SYMBOL).toString();
  if (os.isEmpty())
  {
    qDebug() << "QuoteDataBase::rename: invalid old symbol" << os;
    return 1;
  }

  QString ne = nsymbol->get(Symbol::_EXCHANGE).toString();
  if (ne.isEmpty())
  {
    qDebug() << "QuoteDataBase::rename: invalid new exchange" << ne;
    return 1;
  }

  QString ns = nsymbol->get(Symbol::_SYMBOL).toString();
  if (ns.isEmpty())
  {
    qDebug() << "QuoteDataBase::rename: invalid new symbol" << ns;
    return 1;
  }

  QSqlQuery q(_db);

  // make sure old symbol exists
  QString s = "SELECT tableName FROM symbolIndex";
  s.append(" WHERE symbol='" + os + "'");
  s.append(" AND exchange='" + oe + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::rename:" + q.lastError().text();
    return 1;
  }

  if (! q.next())
  {
    qDebug() << "QuoteDataBase::rename: old exchange/symbol not found";
    return 1;
  }

  // make sure new symbol does not exist
  s = "SELECT tableName FROM symbolIndex";
  s.append(" WHERE symbol='" + ns + "'");
  s.append(" AND exchange='" + ne + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::rename:" + q.lastError().text();
    return 1;
  }

  if (q.next())
  {
    qDebug() << "QuoteDataBase::rename: new exchange/symbol exists";
    return 1;
  }

  s = "UPDATE symbolIndex";
  s.append(" SET symbol='" + ns + "'");
  s.append(", exchange='" + ne + "'");
  s.append(" WHERE symbol='" + os + "'");
  s.append(" AND exchange='" + oe + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::rename:" + q.lastError().text();
    return 1;
  }

  return 0;
}

int QuoteDataBase::search (Data *bd, QList<Data *> &l)
{
  l.clear();

  QString exchange = bd->get(Symbol::_EXCHANGE).toString();
  if (exchange.isEmpty())
  {
    qDebug() << "QuoteDataBase::search: invalid exchange" << exchange;
    return 1;
  }

  QString symbol = bd->get(Symbol::_SYMBOL).toString();
  if (symbol.isEmpty())
  {
    qDebug() << "QuoteDataBase::search: invalid symbol" << symbol;
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
    qDebug() << "QuoteDataBase::search:" + q.lastError().text();
    return 1;
  }

  while (q.next())
  {
    int pos = 0;
    Data *bd = new Data;
    bd->set(Symbol::_EXCHANGE, QVariant(q.value(pos++).toString()));
    bd->set(Symbol::_SYMBOL, QVariant(q.value(pos++).toString()));
    bd->set(Symbol::_NAME, QVariant(q.value(pos++).toString()));
    l.append(bd);
  }

  return 0;
}

int QuoteDataBase::setName (Data *symbol)
{
  QString exchange = symbol->get(Symbol::_EXCHANGE).toString();
  if (exchange.isEmpty())
  {
    qDebug() << "QuoteDataBase::setName: invalid exchange" << exchange;
    return 1;
  }

  QString tsymbol = symbol->get(Symbol::_SYMBOL).toString();
  if (tsymbol.isEmpty())
  {
    qDebug() << "QuoteDataBase::setName: invalid symbol" << tsymbol;
    return 1;
  }

  QString name = symbol->get(Symbol::_NAME).toString();
  if (name.isEmpty())
  {
    qDebug() << "QuoteDataBase::setName: invalid symbol" << name;
    return 1;
  }

  QSqlQuery q(_db);

  QString s = "UPDATE symbolIndex";
  s.append(" SET name='" + name + "'");
  s.append(" WHERE symbol='" + tsymbol + "'");
  s.append(" AND exchange='" + exchange + "'");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::setName:" + q.lastError().text();
    return 1;
  }

  return 0;
}

/*
int QuoteDataBase::dump (Data *bd, QString d)
{
  bd->setBarLength(BarLength::_NONE);
  bd->setRange(DateRange::_ALL);

  // create base directory if needed
  QFileInfo fi(d);
  QString path = fi.absolutePath();

  QDir dir;
  if (! dir.mkpath(path))
  {
    qDebug() << "QuoteDataBase::dump: error creating directory" << path;
    return 1;
  }

  // create exchange directory
  path.append("/");
  path.append(bd->exchange());
  if (! dir.mkpath(path))
  {
    qDebug() << "QuoteDataBase::dump: error creating directory" << path;
    return 1;
  }

  path.append("/");
  path.append(bd->symbol());

  QFile file(path);
  if (! file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    qDebug() << "QuoteDataBase::dump: error opening csv file" << path;
    return 1;
  }
  QTextStream out(&file);

  if (getBars(bd))
  {
    qDebug() << "QuoteDataBase::dump: getBars error";
    return 1;
  }

  Strip strip;
  int loop = 0;
  for (; loop < bd->count(); loop++)
  {
    Bar *b = bd->bar(loop);
    if (! b)
      continue;

    QStringList l;
    l << bd->exchange() << bd->symbol() << bd->name();

    QString s;
    b->dateTimeString(s);
    l << s;

    strip.strip(b->open(), 4, s);
    l << s;

    strip.strip(b->high(), 4, s);
    l << s;

    strip.strip(b->low(), 4, s);
    l << s;

    strip.strip(b->close(), 4, s);
    l << s;

    strip.strip(b->volume(), 4, s);
    l << s;

    strip.strip(b->oi(), 4, s);
    l << s;

    out << l.join(",") << "\n";
  }

  file.close();

  return 0;
}
*/

int QuoteDataBase::transaction ()
{
  _db.transaction();
  return 0;
}

int QuoteDataBase::commit ()
{
  _db.commit();
  return 0;
}
