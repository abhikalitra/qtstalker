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

#include <QtSql>
#include <QtDebug>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QHash>
#include <QList>

Stock::Stock ()
{
  plugin = "Stock";
  scriptMethods << "SET_QUOTE" << "SET_NAME";
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
    tbar.setBarDate(lastDate, data.getBarLength());
    tbar.getBarDateKey(s);

    Bar *bar = bars[s];
    if (! bar)
    {
      if (bars.count() > data.getBarsRequested())
        break;

      bar = new Bar;
      bar->setBarDate(lastDate, data.getBarLength());
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

void Stock::setBars (BarData &bars)
{
  if (! bars.count())
    return;

  QSqlQuery q(QSqlDatabase::database("quotes"));

  transaction();

  if (getIndexData(bars))
    return;

  QString table = bars.getTableName();
  if (table.isEmpty())
  {
    if(createTable(bars))
      return;
  }
  
  int loop;
  for (loop = 0; loop < bars.count(); loop++)
  {
    Bar *bar = bars.getBar(loop);
    QString s;
    bar->getDateTimeString(s);

    QString ts = "INSERT OR REPLACE INTO " + bars.getTableName() + " VALUES(";
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

  commit();
}

int Stock::createTable (BarData &bars)
{
  if (addSymbolIndex(bars))
    return 1;
  
  // new symbol, create new table for it
  QString s = "CREATE TABLE IF NOT EXISTS " + bars.getTableName() + " (";
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

int Stock::scriptCommand (QStringList &l)
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
    default:
      break;
  }
  
  return rc;
}

int Stock::scriptSetQuote (QStringList &l)
{
  // format = QUOTE,PLUGIN,METHOD,EXCHANGE,SYMBOL,DATE_FORMAT,DATE,OPEN,HIGH,LOW,CLOSE,VOLUME*
  
  if (l.count() < 12)
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
  
  int pos = 1;
  BarData bd;
  bd.setPlugin(l[pos++]);
  pos++;
  bd.setExchange(l[pos++]);
  bd.setSymbol(l[pos++]);
  
  QString format = l[pos++];
  
  int t = (l.count() - 6) % 6;
  if (t)
  {
    qDebug() << "Stock::scriptCommand: # of fields incorrect";
    return 1;
  }
  
  int record = 1;
  for (; pos < l.count(); pos = pos + 6, record++)
  {
    QDateTime date = QDateTime::fromString(l[pos], format);
    if (! date.isValid())
    {
      qDebug() << "Stock::scriptCommand: invalid date or date format, record#" << record << format << l[pos];
      continue;
    }
  
    Bar *bar = new Bar;
    bar->setDate(date);
    bar->setOpen(l[pos + 1]);
    bar->setHigh(l[pos + 2]);
    bar->setLow(l[pos + 3]);
    bar->setClose(l[pos + 4]);
    bar->setVolume(l[pos + 5]);
    if (bar->getError())
    {
      barErrorMessage(bar->getError(), record);
      delete bar;
      continue;
    }
    
    bar->verify();
    if (bar->getError())
    {
      barErrorMessage(bar->getError(), record);
      delete bar;
      continue;
    }
    
    bd.append(bar);
  }

  transaction();
  setBars(bd);
  commit();
  
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
  if (getIndexData(bd))
  {
    qDebug() << "Stock::scriptSetName: symbol not found in database" << l[3] << l[4];
    return 1;
  }
  
  bd.setName(l[5]);
  if (setIndexData(bd))
  {
    qDebug() << "Stock::scriptSetName: error setting index";
    return 1;
  }

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

