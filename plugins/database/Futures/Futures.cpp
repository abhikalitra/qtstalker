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
#include "FuturesDataBase.h"

#include <QtSql>
#include <QtDebug>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QHash>
#include <QList>

Futures::Futures ()
{
  plugin = "Futures";
  scriptMethods << "SET_QUOTE" << "SET_NAME" << "SET_CODE" << "SET_MONTH" << "SET_YEAR";
}

void Futures::getBars (BarData &data)
{
  QSqlQuery q(QSqlDatabase::database("quotes"));

  QDateTime firstDate;
  getFirstDate(data.getTableName(), firstDate);

  QDateTime lastDate;
  getLastDate(data.getTableName(), lastDate);

  QHash<QString, Bar *> bars;
  QList<Bar *> dateList;

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

    if (bars.count() > data.getBarsRequested())
      break;
  }
    
  // order the bars from most recent to first
  int loop;
  for (loop = 0; loop < dateList.count(); loop++)
    data.prepend(dateList.at(loop));
}

void Futures::setBars (BarData &bars)
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
    ts.append("," + QString::number(bar->getOI()));
    ts.append(")");
    q.exec(ts);
    if (q.lastError().isValid())
    {
      qDebug() << "Futures::setBars: save quote" << q.lastError().text();
      qDebug() << ts;
    }
  }

  commit();
}

int Futures::createTable (BarData &bars)
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
  s.append(", oi INT");
  s.append(")");
  int rc = command(s, QString("Futures::createTable: create new symbol table"));
  
  if (addParms(bars))
    return 1;
  
  return rc;
}

int Futures::addParms (BarData &bars)
{
  // create a parm table if needed
  QString s = "CREATE TABLE IF NOT EXISTS futuresParms (";
  s.append(" record INTEGER PRIMARY KEY AUTOINCREMENT");
  s.append(", symbol TEXT");
  s.append(", exchange TEXT");
  s.append(", code TEXT");
  s.append(", month TEXT");
  s.append(", year INT");
  s.append(")");
  int rc = command(s, QString("Futures::addParms: create new parm table"));
  
  // add new record
  s = "INSERT OR REPLACE INTO futuresParms (symbol,exchange) VALUES(";
  s.append("'" + bars.getSymbol() + "'");
  s.append(",'" + bars.getExchange() + "'");
  s.append(")");
  rc = command(s, QString("Futures::addParms: add new record: "));
  
  return rc;
}

//*************************************************************
//************** SCRIPT FUNCTIONS *****************************
//*************************************************************

int Futures::scriptCommand (QStringList &l)
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
    case SET_CODE:
      rc = scriptSetCode(l);
      break;
    case SET_MONTH:
      rc = scriptSetMonth(l);
      break;
    case SET_YEAR:
      rc = scriptSetYear(l);
      break;
    default:
      break;
  }
  
  return rc;
}

int Futures::scriptSetQuote (QStringList &l)
{
  // format = QUOTE,PLUGIN,METHOD,EXCHANGE,SYMBOL,DATE_FORMAT,DATE,OPEN,HIGH,LOW,CLOSE,VOLUME,OI*
  
  if (l.count() < 13)
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
  
  int pos = 1;
  BarData bd;
  bd.setPlugin(l[pos++]);
  pos++;
  bd.setExchange(l[pos++]);
  bd.setSymbol(l[pos++]);
  
  QString format = l[pos++];
  
  int t = (l.count() - 6) % 7;
  if (t)
  {
    qDebug() << "Futures::scriptCommand: # of fields incorrect";
    return 1;
  }
  
  int record = 1;
  for (; pos < l.count(); pos = pos + 7, record++)
  {
    QDateTime date = QDateTime::fromString(l[pos], format);
    if (! date.isValid())
    {
      qDebug() << "Futures::scriptCommand: invalid date or date format, record#" << record << format << l[pos];
      continue;
    }
  
    Bar *bar = new Bar;
    bar->setDate(date);
    bar->setOpen(l[pos + 1]);
    bar->setHigh(l[pos + 2]);
    bar->setLow(l[pos + 3]);
    bar->setClose(l[pos + 4]);
    bar->setVolume(l[pos + 5]);
    bar->setOI(l[pos + 6]);
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

int Futures::scriptSetName (QStringList &l)
{
  // format = QUOTE,PLUGIN,METHOD,EXCHANGE,SYMBOL,NAME

  if (l.count() != 6)
  {
    qDebug() << "Futures::scriptSetName: invalid parm count" << l.count();
    return 1;
  }
  
  BarData bd;
  bd.setExchange(l[3]);
  bd.setSymbol(l[4]);
  if (getIndexData(bd))
  {
    qDebug() << "Futures::scriptSetName: symbol not found in database" << l[3] << l[4];
    return 1;
  }
  
  bd.setName(l[5]);
  if (setIndexData(bd))
  {
    qDebug() << "Futures::scriptSetName: error setting index";
    return 1;
  }

  return 0;
}

int Futures::scriptSetCode (QStringList &l)
{
  // format = QUOTE,PLUGIN,METHOD,EXCHANGE,SYMBOL,CODE

  if (l.count() != 6)
  {
    qDebug() << "Futures::scriptSetCode: invalid parm count" << l.count();
    return 1;
  }
  
  BarData bd;
  bd.setExchange(l[3]);
  bd.setSymbol(l[4]);
  if (getIndexData(bd))
  {
    qDebug() << "Futures::scriptSetCode: symbol not found in database" << l[3] << l[4];
    return 1;
  }
  
  FuturesDataBase fdb;
  QStringList cl;
  fdb.getCodeList(cl);
  if (cl.indexOf(l[5]) == -1)
  {
    qDebug() << "Futures::scriptSetCode: code not found in database" << l[5];
    return 1;
  }
  
  QString s = "UPDATE futuresParms SET code='" + l[5] + "'";
  s.append(" WHERE symbol='" + bd.getSymbol() + "'");
  s.append(" AND exchange='" + bd.getExchange() + "'");
  int rc = command(s, QString("Futures::scriptSetCode: "));
  
  return rc;
}

int Futures::scriptSetMonth (QStringList &l)
{
  // format = QUOTE,PLUGIN,METHOD,EXCHANGE,SYMBOL,MONTH

  if (l.count() != 6)
  {
    qDebug() << "Futures::scriptSetMonth: invalid parm count" << l.count();
    return 1;
  }
  
  BarData bd;
  bd.setExchange(l[3]);
  bd.setSymbol(l[4]);
  if (getIndexData(bd))
  {
    qDebug() << "Futures::scriptSetMonth: symbol not found in database" << l[3] << l[4];
    return 1;
  }
  
  QStringList ml;
  ml << "F" << "G" << "H" << "J" << "K" << "M" << "N" << "Q" << "U" << "V" << "X" << "Z";
  if (ml.indexOf(l[5]) == -1)
  {
    qDebug() << "Futures::scriptSetMonth: month not found in database" << l[5];
    return 1;
  }
  
  QString s = "UPDATE futuresParms SET month='" + l[5] + "'";
  s.append(" WHERE symbol='" + bd.getSymbol() + "'");
  s.append(" AND exchange='" + bd.getExchange() + "'");
  int rc = command(s, QString("Futures::scriptSetMonth: "));
  
  return rc;
}

int Futures::scriptSetYear (QStringList &l)
{
  // format = QUOTE,PLUGIN,METHOD,EXCHANGE,SYMBOL,YEAR

  if (l.count() != 6)
  {
    qDebug() << "Futures::scriptSetYear: invalid parm count" << l.count();
    return 1;
  }
  
  BarData bd;
  bd.setExchange(l[3]);
  bd.setSymbol(l[4]);
  if (getIndexData(bd))
  {
    qDebug() << "Futures::scriptSetYear: symbol not found in database" << l[3] << l[4];
    return 1;
  }
  
  if (l[5].length() == 4)
  {
    bool ok;
    l[5].toInt(&ok);
    if (! ok)
    {
      qDebug() << "Futures::scriptSetYear: invalid year" << l[5];
      return 1;
    }
  }
  else
  {
    qDebug() << "Futures::scriptSetYear: year must be 4 digits" << l[5];
    return 1;
  }
      
  QString s = "UPDATE futuresParms SET year=" + l[5];
  s.append(" WHERE symbol='" + bd.getSymbol() + "'");
  s.append(" AND exchange='" + bd.getExchange() + "'");
  int rc = command(s, QString("Futures::scriptSetYear: "));
  
  return rc;
}

//*************************************************************
//*************************************************************
//*************************************************************

DBPlugin * createDBPlugin ()
{
  Futures *o = new Futures;
  return ((DBPlugin *) o);
}

