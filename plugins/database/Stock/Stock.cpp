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

#include "Stock.h"
#include "Bar.h"
#include "ExchangeDataBase.h"
#include "CODataBase.h"

#include <QtSql>
#include <QtDebug>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QList>
#include <QHashIterator>

Stock::Stock ()
{
  plugin = "Stock";
  scriptMethods << "SET_QUOTE" << "SET_NAME" << "SAVE_QUOTES" << "DELETE" << "GET_QUOTES";
}

void Stock::getBars (BarData &data)
{
  QSqlQuery q(QSqlDatabase::database("quotes"));

  QDateTime firstDate;
  getFirstDate(data.getTableName(), firstDate);

  QDateTime lastDate;
  getLastDate(data.getTableName(), lastDate);

  QHash<QString, Bar *> bars;
  QList<Bar *> dateList;

  QString s = "SELECT date,open,high,low,close,volume FROM " + data.getTableName();
  s.append(" WHERE date >='" + firstDate.toString(Qt::ISODate) + "'");
  s.append(" AND date <='" + lastDate.toString(Qt::ISODate) + "'");
  s.append(" ORDER BY date DESC");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "Stock::getBars:" << q.lastError().text();
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
      if (bars.count() > data.getBarsRequested())
        break;

      bar = new Bar;
      bar->setDateRange(lastDate, data.getBarLength());
      bar->setOpen(q.value(1).toDouble());
      bar->setHigh(q.value(2).toDouble());
      bar->setLow(q.value(3).toDouble());
      bar->setClose(q.value(4).toDouble());
      bar->setVolume(q.value(5).toDouble());
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
    }

    if (bars.count() > data.getBarsRequested())
      break;
  }

  // prepend bars in order
  int loop;
  for (loop = 0; loop < dateList.count(); loop++)
    data.prepend(dateList.at(loop));
}

void Stock::setBars ()
{
  if (! quotes.count())
    return;

  QSqlQuery q(QSqlDatabase::database("quotes"));
  
  QHashIterator<QString, BarData *> it(quotes);
  while (it.hasNext())
  {
    it.next();
    BarData *bd = it.value();

    if (getIndexData(bd))
     continue;

    QString table = bd->getTableName();
    if (table.isEmpty())
    {
      if(createTable(bd))
        continue;
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
      ts.append(")");
      q.exec(ts);
      if (q.lastError().isValid())
      {
        qDebug() << "Stock::setBars: save quote" << q.lastError().text();
        qDebug() << ts;
      }
    }
  }
  
  qDeleteAll(quotes);
  quotes.clear();
}

int Stock::createTable (BarData *bars)
{
  if (addSymbolIndex(bars))
    return 1;
  
  // new symbol, create new table for it
  QString s = "CREATE TABLE IF NOT EXISTS " + bars->getTableName() + " (";
  s.append("date DATETIME PRIMARY KEY UNIQUE");
  s.append(", open REAL");
  s.append(", high REAL");
  s.append(", low REAL");
  s.append(", close REAL");
  s.append(", volume INT");
  s.append(")");
  int rc = command(s, QString("Stock::createTable: create new symbol table"));
  
  return rc;
}

//*************************************************************
//************** SCRIPT FUNCTIONS *****************************
//*************************************************************

int Stock::scriptCommand (QStringList &l, QHash<QString, PlotLine *> &tlines)
{
  // format = QUOTE,PLUGIN,METHOD,*
  
  int rc = -1;
  switch ((ScriptMethod) scriptMethods.indexOf(l[2]))
  {
    case SET_QUOTE:
      rc = scriptSetQuote(l);
      break;
    case SET_NAME:
      rc = scriptSetName(l);
      break;
    case SAVE_QUOTES:
      rc = scriptSaveQuotes(l);
      break;
    case DELETE:
      rc = scriptDelete(l);
      break;
    case GET_QUOTES:
      rc = scriptGetQuotes(l, tlines);
      break;
    default:
      break;
  }
  
  return rc;
}

int Stock::scriptSetQuote (QStringList &l)
{
  // format = QUOTE,PLUGIN,METHOD,EXCHANGE,SYMBOL,DATE_FORMAT,DATE,OPEN,HIGH,LOW,CLOSE,VOLUME
  //            0     1      2       3       4        5        6    7    8    9   10     11
  
  if (l.count() != 12)
  {
    qDebug() << "Stock::scriptCommand: invalid parm count" << l.count();
    qDebug() << l;
    return 1;
  }

  ExchangeDataBase edb;
  if (edb.verifyExchangeName(l[3]))
  {
    qDebug() << "Stock::scriptCommand: invalid exchange" << l[3];
    return 1;
  }
  
  QDateTime date = QDateTime::fromString(l[6], l[5]);
  if (! date.isValid())
  {
    qDebug() << "Stock::scriptCommand: invalid date or date format" << l[5] << l[6];
    return 1;
  }

  Bar *bar = new Bar;
  bar->setDate(date);
  bar->setOpen(l[7]);
  bar->setHigh(l[8]);
  bar->setLow(l[9]);
  bar->setClose(l[10]);
  bar->setVolume(l[11]);
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

int Stock::scriptSetName (QStringList &l)
{
  // format = QUOTE,PLUGIN,METHOD,EXCHANGE,SYMBOL,NAME

  if (l.count() != 6)
  {
    qDebug() << "Stock::scriptSetName: invalid parm count" << l.count();
    return 1;
  }
  
  BarData bd;
  bd.setExchange(l[3]);
  bd.setSymbol(l[4]);
  if (getIndexData(&bd))
  {
    qDebug() << "Stock::scriptSetName: symbol not found in database" << l[3] << l[4];
    return 1;
  }
  
  bd.setName(l[5]);
  if (setIndexData(&bd))
  {
    qDebug() << "Stock::scriptSetName: error setting index";
    return 1;
  }

  return 0;
}

int Stock::scriptSaveQuotes (QStringList &l)
{
  // format = QUOTE,PLUGIN,SAVE_QUOTES

  if (l.count() != 3)
  {
    qDebug() << "Stock::scriptSaveQuotes: invalid parm count" << l.count();
    return 1;
  }

  transaction();
  setBars();
  commit();

  return 0;
}

int Stock::scriptDelete (QStringList &l)
{
  // format = QUOTE,PLUGIN,DELETE,EXCHANGE,SYMBOL
  //            0     1      2      3        4

  if (l.count() != 5)
  {
    qDebug() << "Stock::scriptDelete: invalid parm count" << l.count();
    return 1;
  }
  
  BarData bd;
  bd.setExchange(l[3]);
  bd.setSymbol(l[4]);

  transaction();
  
  if (getIndexData(&bd))
    return 1;

  // delete any chart objects
  CODataBase db;
  db.deleteChartObjects(&bd);
  
  // drop quote table
  QString s = "DROP TABLE " + bd.getTableName();
  if (command(s, QString("Stock::scriptDelete: drop quotes table")))
    return 1;
  
  // remove index record
  s = "DELETE FROM symbolIndex";
  s.append(" WHERE symbol='" + bd.getSymbol() + "'");
  s.append(" AND exchange='" + bd.getExchange() + "'");
  if (command(s, QString("Stock::scriptDelete: remove symbolIndex record")))
    return 1;
  
  commit();

  return 0;
}

int Stock::scriptGetQuotes (QStringList &l, QHash<QString, PlotLine *> &tlines)
{
  // format = QUOTE,PLUGIN,GET_QUOTES,<NAME>,<EXCHANGE>,<SYMBOL>,<BAR_FIELD>,<BAR_LENGTH>,<BARS>
  //            0     1       2         3         4         5            6         7        8

  if (l.count() != 9)
  {
    qDebug() << "Stock::scriptGetQuotes: invalid parm count" << l.count();
    return 1;
  }

  PlotLine *tl = tlines.value(l[3]);
  if (tl)
  {
    qDebug() << "Stock::scriptGetQuotes: duplicate name" << l[3];
    return 1;
  }

  bool ok;
  int bars = l[8].toInt(&ok);
  if (! ok)
  {
    qDebug() << "Stock::scriptGetQuotes: invalid bars" << l[8];
    return 1;
  }
  
  BarData bd;
  bd.setExchange(l[4]);
  bd.setSymbol(l[5]);
  bd.setBarLength(l[7]);
  bd.setBarsRequested(bars);
  getBars(bd);
  
  BarData::InputType it = bd.getInputType(l[6]);
  PlotLine *line = bd.getInput(it);
  if (! line)
    return 1;
  
  tlines.insert(l[3], line);

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

DBPlugin * createDBPlugin ()
{
  Stock *o = new Stock;
  return ((DBPlugin *) o);
}

