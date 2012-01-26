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
#include "CurveBar.h"
#include "DateRange.h"
#include "TypeDateRange.h"
#include "KeySymbol.h"
#include "KeyQuoteDataBase.h"
#include "TypeData.h"
#include "Symbol.h"
#include "KeyCurveBar.h"
#include "TypeBarLength.h"

#include <QDateTime>
#include <QtDebug>
#include <QUuid>

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
  s.append("a INTEGER PRIMARY KEY"); // auto increment index
  s.append(", name TEXT"); // entity name
  s.append(", key TEXT"); // attribute
  s.append(", value TEXT"); // attribute value
  s.append(", type TEXT"); // data type
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "QuoteDataBase::init:" << _db.lastError().text();
}

int QuoteDataBase::getBars (Symbol &bd)
{
  KeySymbol symkeys;
  Data td;
  bd.toData(symkeys.indexToString(KeySymbol::_SYMBOL), td);
  
  QStringList tl = td.toString().split(":");
  if (tl.size() != 2)
  {
    qDebug() << "QuoteDataBase::getBars: invalid SYMBOL" << tl;
    return 1;
  }

  QString exchange = tl.at(0);
  QString tsymbol = tl.at(1);

  Data length;
  bd.toData(symkeys.indexToString(KeySymbol::_LENGTH), length);
  if (length.toInteger() == -1)
  {
    qDebug() << "QuoteDataBase::getBars: invalid length";
    return 1;
  }

  bd.toData(symkeys.indexToString(KeySymbol::_START_DATE), td);
  QDateTime startDate = td.toDateTime();
  
  bd.toData(symkeys.indexToString(KeySymbol::_END_DATE), td);
  QDateTime endDate = td.toDateTime();

  Data dateRange;
  bd.toData(symkeys.indexToString(KeySymbol::_RANGE), dateRange);

  if (getSymbol(&bd))
    return 1;

  QSqlQuery q(_db);

  // check if we want to return most recent bars
  Data table;
  bd.toData(symkeys.indexToString(KeySymbol::_TABLE), table);

  if (! startDate.isValid() && ! endDate.isValid())
  {
    QString s = "SELECT max(date) FROM " + table.toString();
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "QuoteDataBase::getBars:" + q.lastError().text();
      qDebug() << s;
      return 1;
    }

    if (q.next())
    {
      endDate = QDateTime::fromString(q.value(0).toString(), "yyyyMMddHHmmss");
      DateRange dr;
      if (dr.dateRange((TypeDateRange::Key) dateRange.toInteger(), endDate, startDate))
      {
        qDebug() << "QuoteDataBase::getBars: date range error" << dateRange.toString();
        return 1;
      }
    }
    else
      return 1;
  }

  QString s = "SELECT date,open,high,low,close,volume,oi";
  s.append(" FROM " + table.toString());
  s.append(" WHERE date >=" + startDate.toString("yyyyMMddHHmmss"));
  s.append(" AND date <=" + endDate.toString("yyyyMMddHHmmss"));
  s.append(" ORDER BY date DESC");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::getBars:" + q.lastError().text();
    qDebug() << s;
    return 1;
  }

  QHash<QString, CurveBar> bars;
  QStringList order;
  DateRange dr;
  KeyCurveBar cbkeys;

  while (q.next())
  {
    QDateTime lastDate = QDateTime::fromString(q.value(0).toString(), "yyyyMMddHHmmss");

    QDateTime tsd, ted;
    dr.dateInterval(lastDate, (TypeBarLength::Key) length.toInteger(), tsd, ted);
    s = dr.rangeKey(tsd, ted);

//qDebug() << lastDate << tsd << ted;

    CurveBar bar;
    if (! bars.contains(s))
    {
      bar.set(cbkeys.indexToString(KeyCurveBar::_OPEN), Data(q.value(1).toDouble()));
      bar.set(cbkeys.indexToString(KeyCurveBar::_HIGH), Data(q.value(2).toDouble()));
      bar.set(cbkeys.indexToString(KeyCurveBar::_LOW), Data(q.value(3).toDouble()));
      bar.set(cbkeys.indexToString(KeyCurveBar::_CLOSE), Data(q.value(4).toDouble()));
      bar.set(cbkeys.indexToString(KeyCurveBar::_VOLUME), Data(q.value(5).toDouble()));
      bar.set(cbkeys.indexToString(KeyCurveBar::_OI), Data(q.value(6).toDouble()));
      bar.set(cbkeys.indexToString(KeyCurveBar::_DATE), Data(lastDate));
      bars.insert(s, bar);
      order.prepend(s);
    }
    else
    {
      bar = bars.value(s);
      
      bar.set(cbkeys.indexToString(KeyCurveBar::_OPEN), Data(q.value(1).toDouble()));

      Data td;
      bar.toData(cbkeys.indexToString(KeyCurveBar::_HIGH), td);
      double v = q.value(2).toDouble();
      if (v > td.toDouble())
        bar.set(cbkeys.indexToString(KeyCurveBar::_HIGH), Data(v));

      v = q.value(3).toDouble();
      bar.toData(cbkeys.indexToString(KeyCurveBar::_LOW), td);
      if (v < td.toDouble())
        bar.set(cbkeys.indexToString(KeyCurveBar::_LOW), Data(v));

      bar.toData(cbkeys.indexToString(KeyCurveBar::_VOLUME), td);
      v = q.value(5).toDouble();
      v += td.toDouble();
      bar.set(cbkeys.indexToString(KeyCurveBar::_VOLUME), Data(v));

      bar.set(cbkeys.indexToString(KeyCurveBar::_OI), Data(q.value(6).toDouble()));
      
      bars.insert(s, bar);
    }
  }

  // insert bars into Symbol bars in proper order
  int loop = 0;
  for (; loop < order.size(); loop++)
  {
    CurveBar bar = bars.value(order.at(loop));
    bd.setEntity(loop, bar);
  }

  return 0;
}

int QuoteDataBase::setBars (Symbol *symbol)
{
  // save bars
  KeySymbol symkeys;
  QSqlQuery q(_db);

  if (getSymbol(symbol))
  {
    if (newSymbol(symbol))
      return 1;
  }
  else
  {
    // check if we need to save the name
    Data name;
    symbol->toData(symkeys.indexToString(KeySymbol::_NAME), name);
    if (! name.toString().isEmpty())
      setName(symbol);
  }

  Data table;
  symbol->toData(symkeys.indexToString(KeySymbol::_TABLE), table);

  KeyCurveBar cbkeys;
  QList<int> keys;
  symbol->ekeys(keys);
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Entity bar;
    if (symbol->toIndex(keys.at(loop), bar))
      continue;

    Data td;
    bar.toData(cbkeys.indexToString(KeyCurveBar::_DATE), td);
    QDateTime dt = td.toDateTime();
    
    QString date = dt.toString("yyyyMMddHHmmss");

    // first check if record exists so we know to do an update or insert
    QString s = "SELECT date FROM " + table.toString() + " WHERE date =" + date;
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "QuoteDataBase::setBars:" << q.lastError().text();
      qDebug() << s;
      continue;
    }

    if (q.next()) // record exists, use update
    {
      s = "UPDATE " + table.toString() + " SET ";

      QStringList tl;

      Data set;
      if (! bar.toData(cbkeys.indexToString(KeyCurveBar::_OPEN), set))
      {
        if (! set.toString().isEmpty())
          tl << "open=" + set.toString();
      }

      if (! bar.toData(cbkeys.indexToString(KeyCurveBar::_HIGH), set))
      {
        if (! set.toString().isEmpty())
          tl << "high=" + set.toString();
      }

      if (! bar.toData(cbkeys.indexToString(KeyCurveBar::_LOW), set))
      {
        if (! set.toString().isEmpty())
          tl << "low=" + set.toString();
      }

      if (! bar.toData(cbkeys.indexToString(KeyCurveBar::_CLOSE), set))
      {
        if (! set.toString().isEmpty())
          tl << "close=" + set.toString();
      }

      if (! bar.toData(cbkeys.indexToString(KeyCurveBar::_VOLUME), set))
      {
        if (! set.toString().isEmpty())
          tl << "volume=" + set.toString();
      }

      if (! bar.toData(cbkeys.indexToString(KeyCurveBar::_OI), set))
      {
        if (! set.toString().isEmpty())
          tl << "oi=" + set.toString();
      }

      s.append(tl.join(","));

      s.append(" WHERE date=" + date);
    }
    else // new record, use insert
    {
      s = "INSERT INTO " + table.toString() + " (date,open,high,low,close,volume,oi) VALUES(";

      QStringList tl;
      tl << date;

      QString ts;
      Data set;
      if (! bar.toData(cbkeys.indexToString(KeyCurveBar::_OPEN), set))
        ts = set.toString();
      if (ts.isEmpty())
        ts = "0";
      tl << ts;

      ts.clear();
      if (! bar.toData(cbkeys.indexToString(KeyCurveBar::_HIGH), set))
        ts = set.toString();
      if (ts.isEmpty())
        ts = "0";
      tl << ts;

      ts.clear();
      if (! bar.toData(cbkeys.indexToString(KeyCurveBar::_LOW), set))
        ts = set.toString();
      if (ts.isEmpty())
        ts = "0";
      tl << ts;

      ts.clear();
      if (! bar.toData(cbkeys.indexToString(KeyCurveBar::_CLOSE), set))
        ts = set.toString();
      if (ts.isEmpty())
        ts = "0";
      tl << ts;

      ts.clear();
      if (! bar.toData(cbkeys.indexToString(KeyCurveBar::_VOLUME), set))
        ts = set.toString();
      if (ts.isEmpty())
        ts = "0";
      tl << ts;

      ts.clear();
      if (! bar.toData(cbkeys.indexToString(KeyCurveBar::_OI), set))
        ts = set.toString();
      if (ts.isEmpty())
        ts = "0";
      tl << ts;

      s.append(tl.join(","));
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

int QuoteDataBase::newSymbol (Symbol *symbol)
{
  KeySymbol symkeys;
  Data td;
  symbol->toData(symkeys.indexToString(KeySymbol::_SYMBOL), td);
  
  QStringList tl = td.toString().split(":");
  if (tl.size() != 2)
  {
    qDebug() << "QuoteDataBase::newSymbol: invalid SYMBOL" << tl;
    return 1;
  }
  QString tsymbol = td.toString();

  Data type;
  symbol->toData(symkeys.indexToString(KeySymbol::_TYPE), type);

  if (! getSymbol(symbol))
  {
    qDebug() << "QuoteDataBase::newSymbol: duplicate symbol";
    return 1;
  }

  QSqlQuery q(_db);

  QString table = "Q" + QUuid::createUuid().toString();
  table.remove("{");
  table.remove("}");
  table.remove("-");
  symbol->set(symkeys.indexToString(KeySymbol::_TABLE), Data(table));

  // set default name to symbol
  Data name;
  symbol->toData(symkeys.indexToString(KeySymbol::_NAME), name);
  if (name.toString().isEmpty())
  {
    name.set(tsymbol);
    symbol->set(symkeys.indexToString(KeySymbol::_NAME), name);
  }

  // add new symbol entry into the symbolIndex table

  // add SYMBOL
  KeyQuoteDataBase qkeys;
  QString s = "INSERT INTO symbolIndex VALUES(";
  s.append("NULL"); // auto increment
  s.append(",'" + tsymbol + "'");
  s.append(",'" + qkeys.indexToString(KeyQuoteDataBase::_SYMBOL) + "'");
  s.append(",'" + tsymbol + "'");
  s.append(",'" + QString::number(TypeData::_STRING) + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::newSymbol:" + q.lastError().text();
    qDebug() << s;
    return 1;
  }

  // add TABLE
  s = "INSERT INTO symbolIndex VALUES(";
  s.append("NULL"); // auto increment
  s.append(",'" + tsymbol + "'");
  s.append(",'" + qkeys.indexToString(KeyQuoteDataBase::_TABLE) + "'");
  s.append(",'" + table + "'");
  s.append(",'" + QString::number(TypeData::_STRING) + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::newSymbol:" + q.lastError().text();
    qDebug() << s;
    return 1;
  }

  // add NAME
  s = "INSERT INTO symbolIndex VALUES(";
  s.append("NULL"); // auto increment
  s.append(",'" + tsymbol + "'");
  s.append(",'" + qkeys.indexToString(KeyQuoteDataBase::_NAME) + "'");
  s.append(",'" + name.toString() + "'");
  s.append(",'" + QString::number(TypeData::_STRING) + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::newSymbol:" + q.lastError().text();
    qDebug() << s;
    return 1;
  }

  // add TYPE
  s = "INSERT INTO symbolIndex VALUES(";
  s.append("NULL"); // auto increment
  s.append(",'" + tsymbol + "'");
  s.append(",'" + qkeys.indexToString(KeyQuoteDataBase::_TYPE) + "'");
  s.append(",'" + type.toString() + "'");
  s.append(",'" + QString::number(TypeData::_STRING) + "'");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::newSymbol:" + q.lastError().text();
    qDebug() << s;
    return 1;
  }

  // create new table
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

int QuoteDataBase::getSymbol (Symbol *symbol)
{
  KeySymbol symkeys;
  Data tsymbol;
  symbol->toData(symkeys.indexToString(KeySymbol::_SYMBOL), tsymbol);

  QSqlQuery q(_db);

  QString s = "SELECT key,value FROM symbolIndex WHERE name='" + tsymbol.toString() + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::getSymbol:" + q.lastError().text();
    qDebug() << s;
    return 1;
  }

  KeyQuoteDataBase qkeys;
  int rc = 1;
  while (q.next())
  {
    rc = 0;
    
    switch ((KeyQuoteDataBase::Key) qkeys.stringToIndex(q.value(0).toString()))
    {
      case KeyQuoteDataBase::_TYPE:
        symbol->set(symkeys.indexToString(KeySymbol::_TYPE), Data(q.value(1).toString()));
	break;
      case KeyQuoteDataBase::_NAME:
        symbol->set(symkeys.indexToString(KeySymbol::_NAME), Data(q.value(1).toString()));
	break;
      case KeyQuoteDataBase::_TABLE:
        symbol->set(symkeys.indexToString(KeySymbol::_TABLE), Data(q.value(1).toString()));
	break;
      default:
	break;
    }
  }

  return rc;
}

int QuoteDataBase::deleteSymbol (Symbol *bd)
{
  if (getSymbol(bd))
  {
    qDebug() << "QuoteDataBase::deleteSymbol: invalid SYMBOL";
    return 1;
  }

  KeySymbol symkeys;
  Data table;
  bd->toData(symkeys.indexToString(KeySymbol::_TABLE), table);

  // drop quote table
  QSqlQuery q(_db);

  QString s = "DROP TABLE " + table.toString();
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::deleteSymbol:" + q.lastError().text();
    return 1;
  }

  Data symbol;
  bd->toData(symkeys.indexToString(KeySymbol::_SYMBOL), symbol);

  // remove index records for symbol
  s = "DELETE FROM symbolIndex WHERE name='" + symbol.toString() + "'";
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
  QString s = "SELECT DISTINCT name FROM symbolIndex";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::getExchange:" << q.lastError().text();
    return 1;
  }

  while (q.next())
  {
    QStringList tl = q.value(0).toString().split(":");
    l << tl.at(0);
  }
  
  l.removeDuplicates();

  return 0;
}

/*
int QuoteDataBase::rename (Symbol *osymbol, Symbol *nsymbol)
{
  QString oe = osymbol.exchange();
  if (oe.isEmpty())
  {
    qDebug() << "QuoteDataBase::rename: invalid old exchange" << oe;
    return 1;
  }

  QString os = osymbol.symbol();
  if (os.isEmpty())
  {
    qDebug() << "QuoteDataBase::rename: invalid old symbol" << os;
    return 1;
  }

  QString ne = nsymbol.exchange();
  if (ne.isEmpty())
  {
    qDebug() << "QuoteDataBase::rename: invalid new exchange" << ne;
    return 1;
  }

  QString ns = nsymbol.symbol();
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
*/

int QuoteDataBase::search (QString symbol, QList<Symbol> &l)
{
  l.clear();

  KeySymbol symkeys;

  QSqlQuery q(_db);

  // first find matching names
  QString s = "SELECT DISTINCT name FROM symbolIndex";

  if (symbol.contains("%"))
    s.append(" WHERE name LIKE '" + symbol + "'");
  else
    s.append(" WHERE name='" + symbol + "'");

  s.append(" ORDER BY name ASC");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "QuoteDataBase::search:" + q.lastError().text();
    return 1;
  }

  QStringList names;
  while (q.next())
    names << q.value(0).toString();

  // load names with data
  int loop = 0;
  for (; loop < names.size(); loop++)
  {
    Symbol sym;
    sym.set(symkeys.indexToString(KeySymbol::_SYMBOL), Data(names.at(loop)));
    
    if (getSymbol(&sym))
      continue;
    
    l << sym;
  }

  return 0;
}

int QuoteDataBase::setName (Symbol *symbol)
{
  KeySymbol symkeys;
  Data name;
  symbol->toData(symkeys.indexToString(KeySymbol::_NAME), name);
  
  if (name.toString().isEmpty())
  {
    qDebug() << "QuoteDataBase::getSymbol: invalid NAME";
    return 1;
  }

  Data tsymbol;
  symbol->toData(symkeys.indexToString(KeySymbol::_SYMBOL), tsymbol);

  QSqlQuery q(_db);

  KeyQuoteDataBase qkeys;
  QString s = "UPDATE symbolIndex";
  s.append(" SET value='" + name.toString() + "'");
  s.append(" WHERE name='" + tsymbol.toString() + "'");
  s.append(" AND key='" + qkeys.indexToString(KeyQuoteDataBase::_NAME) + "'");
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
  bd.setBarLength(BarLength::_NONE);
  bd.setRange(DateRange::_ALL);

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
  path.append(bd.exchange());
  if (! dir.mkpath(path))
  {
    qDebug() << "QuoteDataBase::dump: error creating directory" << path;
    return 1;
  }

  path.append("/");
  path.append(bd.symbol());

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
  for (; loop < bd.count(); loop++)
  {
    Bar *b = bd.bar(loop);
    if (! b)
      continue;

    QStringList l;
    l << bd.exchange() << bd.symbol() << bd.name();

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
