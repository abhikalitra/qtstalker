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

#include "Futures.h"
#include "Bar.h"
#include "ExchangeDataBase.h"
#include "DateRange.h"

#include <QtSql>
#include <QtDebug>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QHashIterator>
#include <QList>

Futures::Futures ()
{
  plugin = "Futures";
  
  scriptMethods << "SET_QUOTE" << "SAVE_QUOTES" << "DELETE" << "GET_QUOTES" << "RENAME";
  scriptMethods << "SET_DETAIL" << "GET_DETAIL";
}

void Futures::getBars (BarData &data)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));

  QHash<QString, Bar *> bars;
  QList<Bar *> dateList;

  // get symbol name
  QString name;
  _ddb.detail((int) FuturesDetailsDataBase::Name, &data, name);
  data.setName(name);

  QDateTime firstDate, lastDate;
  if (! data.dateRangeOverride())
  {
    getLastDate(data.getTableName(), lastDate);
    if (! lastDate.isValid())
      lastDate = QDateTime::currentDateTime();

    DateRange dr;
    if (dr.dateRange((DateRange::Range) data.dateRange(), lastDate, firstDate))
      firstDate = lastDate;
  }
  else
  {
    lastDate = data.endDate();
    firstDate = data.startDate();
  }
  
  QString s = "SELECT date,open,high,low,close,volume,oi FROM " + data.getTableName();
  s.append(" WHERE date >='" + firstDate.toString(Qt::ISODate) + "'");
  s.append(" AND date <='" + lastDate.toString(Qt::ISODate) + "'");
  s.append(" ORDER BY date DESC");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "Futures::getBars:" << q.lastError().text();
    return;
  }

  while (q.next())
  {
    lastDate = q.value(0).toDateTime();

    Bar tbar;
    tbar.setDateRange(lastDate, data.getBarLength());
    tbar.getRangeKey(s);

    Bar *bar = bars[s];
    if (! bar)
    {
      bar = new Bar;
      bar->setDateRange(lastDate, data.getBarLength());
      bar->setOpen(q.value(1).toDouble());
      bar->setHigh(q.value(2).toDouble());
      bar->setLow(q.value(3).toDouble());
      bar->setClose(q.value(4).toDouble());
      bar->setVolume(q.value(5).toDouble());
      bar->setOI(q.value(6).toInt());
      bars.insert(s, bar);
      dateList.append(bar);
    }
    else
    {
      bar->setOpen(q.value(1).toDouble());

      double v = q.value(2).toDouble();
      if (v > bar->getHigh())
        bar->setHigh(v);

      v = q.value(3).toDouble();
      if (v < bar->getLow())
        bar->setLow(v);

      v = q.value(5).toDouble();
      double v2 = bar->getVolume();
      bar->setVolume(v + v2);

      bar->setOI(q.value(6).toInt());
    }
  }
    
  // order the bars from most recent to first
  int loop;
  for (loop = 0; loop < dateList.count(); loop++)
    data.prepend(dateList.at(loop));
}

void Futures::setBars ()
{
  if (! quotes.count())
    return;

  QSqlQuery q(QSqlDatabase::database(_dbName));

  QuoteIndexDataBase idb;

  QHashIterator<QString, BarData *> it(quotes);
  while (it.hasNext())
  {
    it.next();
    BarData *bd = it.value();

    if (idb.getIndexData(bd))
      return;

    QString table = bd->getTableName();
    if (table.isEmpty())
    {
      if(createTable(bd, idb))
        return;
    }
  
    int loop;
    for (loop = 0; loop < bd->count(); loop++)
    {
      Bar *bar = bd->getBar(loop);
      QString s;
      bar->getDateTimeString(s);

      QString ts = "INSERT OR REPLACE INTO " + bd->getTableName() + " VALUES(";
      ts.append("'" + s + "'");
      ts.append("," + QString::number(bar->getOpen()));
      ts.append("," + QString::number(bar->getHigh()));
      ts.append("," + QString::number(bar->getLow()));
      ts.append("," + QString::number(bar->getClose()));
      ts.append("," + QString::number(bar->getVolume()));
      ts.append("," + QString::number(bar->getOI()));
      ts.append(")");
      q.exec(ts);
      if (q.lastError().isValid())
      {
        qDebug() << "Futures::setBars: save quote" << q.lastError().text();
        qDebug() << ts;
      }
    }
  }
  
  qDeleteAll(quotes);
  quotes.clear();
}

int Futures::createTable (BarData *bars, QuoteIndexDataBase &idb)
{
  if (idb.addSymbolIndex(bars))
    return 1;
  
  // new symbol, create new table for it
  QString s = "CREATE TABLE IF NOT EXISTS " + bars->getTableName() + " (";
  s.append("date DATETIME PRIMARY KEY UNIQUE");
  s.append(", open REAL");
  s.append(", high REAL");
  s.append(", low REAL");
  s.append(", close REAL");
  s.append(", volume INT");
  s.append(", oi INT");
  s.append(")");
  int rc = command(s, QString("Futures::createTable: create new symbol table"));

  return rc;
}

int Futures::deleteSymbol (BarData *symbol)
{
  QuoteIndexDataBase idb;
  if (idb.deleteSymbol(symbol))
    return 1;
  
  _ddb.remove(symbol);

  return 0;
}

int Futures::setDetail (QString &key, BarData *bd, QString &data)
{
  return _ddb.setDetail(key, bd, data);
}

int Futures::setDetail (int key, BarData *bd, QString &data)
{
  return _ddb.setDetail(key, bd, data);
}

int Futures::detail (QString &key, BarData *bd, QString &data)
{
  return _ddb.detail(key, bd, data);
}

int Futures::detail (int key, BarData *bd, QString &data)
{
  return _ddb.detail(key, bd, data);
}

//*************************************************************
//************** SCRIPT FUNCTIONS *****************************
//*************************************************************

int Futures::scriptCommand (QStringList &l, Indicator &ind, QByteArray &ba)
{
  // QUOTE,PLUGIN,METHOD,*
  //   0      1     2
  
  int rc = -1;
  switch ((ScriptMethod) scriptMethods.indexOf(l[2]))
  {
    case SET_QUOTE:
      rc = scriptSetQuote(l);
      if (! rc)
      {
        ba.clear();
        ba.append("0\n");
      }
      break;
    case SAVE_QUOTES:
      rc = scriptSaveQuotes(l);
      if (! rc)
      {
        ba.clear();
        ba.append("0\n");
      }
      break;
    case DELETE:
      rc = scriptDelete(l);
      if (! rc)
      {
        ba.clear();
        ba.append("0\n");
      }
      break;
    case GET_QUOTES:
      rc = scriptGetQuotes(l, ind);
      if (! rc)
      {
        ba.clear();
        ba.append("0\n");
      }
      break;
    case RENAME:
      rc = scriptRename(l);
      if (! rc)
      {
        ba.clear();
        ba.append("0\n");
      }
      break;
    case SET_DETAIL:
      rc = scriptSetDetail(l);
      if (! rc)
      {
        ba.clear();
        ba.append("0\n");
      }
      break;
    case GET_DETAIL:
      rc = scriptGetDetail(l, ba);
      break;
    default:
      break;
  }
  
  return rc;
}

int Futures::scriptSetQuote (QStringList &l)
{
  // QUOTE,PLUGIN,METHOD,EXCHANGE,SYMBOL,DATE_FORMAT,DATE,OPEN,HIGH,LOW,CLOSE,VOLUME,OI
  //   0     1      2       3       4        5        6    7    8    9    10    11   12
  
  if (l.count() != 13)
  {
    qDebug() << "Futures::scriptCommand: invalid parm count" << l.count();
    qDebug() << l;
    return 1;
  }

  ExchangeDataBase edb;
  if (edb.verifyExchangeName(l[3]))
  {
    qDebug() << "Futures::scriptCommand: invalid exchange" << l[3];
    return 1;
  }
  
  QDateTime date = QDateTime::fromString(l[6], l[5]);
  if (! date.isValid())
  {
    qDebug() << "Futures::scriptCommand: invalid date or date format" << l[5] << l[6];
    return 1;
  }
  
  Bar *bar = new Bar;
  bar->setDate(date);
  bar->setOpen(l[7]);
  bar->setHigh(l[8]);
  bar->setLow(l[9]);
  bar->setClose(l[10]);
  bar->setVolume(l[11]);
  bar->setOI(l[12]);
  if (bar->getError())
  {
    barErrorMessage(bar->getError());
    delete bar;
    return 1;
  }
    
  bar->verify();
  if (bar->getError())
  {
    barErrorMessage(bar->getError());
    delete bar;
    return 1;
  }
    
  QString key = l[3] + l[4];
  BarData *bd = quotes.value(key);
  if (! bd)
  {
    bd = new BarData;
    bd->setPlugin(l[1]);
    bd->setExchange(l[3]);
    bd->setSymbol(l[4]);
    quotes.insert(key, bd);    
  }
  
  bd->append(bar);
  
  return 0;
}

int Futures::scriptSaveQuotes (QStringList &l)
{
  // QUOTE,PLUGIN,SAVE_QUOTES
  //   0     1        2

  if (l.count() != 3)
  {
    qDebug() << "Futures::scriptSaveQuotes: invalid parm count" << l.count();
    return 1;
  }

  transaction();
  setBars();
  commit();

  return 0;
}

int Futures::scriptDelete (QStringList &l)
{
  // QUOTE,PLUGIN,DELETE,EXCHANGE,SYMBOL
  //   0     1      2      3        4

  if (l.count() != 5)
  {
    qDebug() << "Futures::scriptDelete: invalid parm count" << l.count();
    return 1;
  }
  
  BarData bd;
  bd.setExchange(l[3]);
  bd.setSymbol(l[4]);

  return deleteSymbol(&bd);
}

int Futures::scriptGetQuotes (QStringList &l, Indicator &ind)
{
  // QUOTE,PLUGIN,GET_QUOTES,<NAME>,<EXCHANGE>,<SYMBOL>,<BAR_FIELD>,<BAR_LENGTH>,<DATE RANGE>
  //   0     1       2         3         4         5            6         7           8

  if (l.count() != 9)
  {
    qDebug() << "Futures::scriptGetQuotes: invalid parm count" << l.count();
    return 1;
  }

  PlotLine *tl = ind.line(l[3]);
  if (tl)
  {
    qDebug() << "Futures::scriptGetQuotes: duplicate name" << l[3];
    return 1;
  }

  QStringList rl;
  DateRange dr;
  dr.list(rl);
  int range = rl.indexOf(l[8]);
  if (range == -1)
  {
    qDebug() << "Futures::scriptGetQuotes: invalid date range" << l[8];
    return 1;
  }

  BarData bd;
  bd.setExchange(l[4]);
  bd.setSymbol(l[5]);
  bd.setBarLength(l[7]);
  bd.setDateRange(range);
  getBars(bd);
  
  BarData::InputType it = bd.getInputType(l[6]);
  PlotLine *line = bd.getInput(it);
  if (! line)
    return 1;
  
  ind.setLine(l[3], line);

  return 0;
}

int Futures::scriptRename (QStringList &l)
{
  // QUOTE,PLUGIN,RENAME,OLD_EXCHANGE,OLD_SYMBOL,NEW_EXCHANGE,NEW_SYMBOL
  //   0     1      2          3           4          5          6

  if (l.count() != 7)
  {
    qDebug() << "Futures::scriptRename: invalid parm count" << l.count();
    return 1;
  }
  
  BarData obd;
  obd.setExchange(l[3]);
  obd.setSymbol(l[4]);

  BarData nbd;
  nbd.setExchange(l[5]);
  nbd.setSymbol(l[6]);

  QuoteIndexDataBase idb;
  if (idb.rename(&obd, &nbd))
    return 1;

  return 0;
}

int Futures::scriptSetDetail (QStringList &l)
{
  // QUOTE,PLUGIN,METHOD,DETAIL METHOD,EXCHANGE,SYMBOL,DATA
  //   0     1      2          3          4       5      6

  if (l.count() != 7)
  {
    qDebug() << "Futures::scriptSetDetail: invalid parm count" << l.count();
    return 1;
  }

  BarData bd;
  bd.setExchange(l[4]);
  bd.setSymbol(l[5]);
  
  if (_ddb.setDetail(l[3], &bd, l[6]))
    return 1;

  return 0;
}

int Futures::scriptGetDetail (QStringList &l, QByteArray &ba)
{
  // QUOTE,PLUGIN,METHOD,DETAIL METHOD,EXCHANGE,SYMBOL
  //   0     1      2          3          4       5

  if (l.count() != 6)
  {
    qDebug() << "Futures::scriptGetDetail: invalid parm count" << l.count();
    return 1;
  }

  QString data;
  BarData bd;
  bd.setExchange(l[4]);
  bd.setSymbol(l[5]);
  
  if (_ddb.detail(l[3], &bd, data))
    return 1;

  ba.clear();
  ba.append(data + "\n");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

DBPlugin * createDBPlugin ()
{
  Futures *o = new Futures;
  return ((DBPlugin *) o);
}

