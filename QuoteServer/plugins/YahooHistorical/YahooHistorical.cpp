/*
 *  QuoteServer
 *
 *  Copyright (C) 2010 Stefan S. Stratigakos
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

#include "YahooHistorical.h"
#include "YahooParseSymbol.h"
#include "QuoteDataBase.h"
#include "Symbol.h"
#include "Bar.h"
#include "Globals.h"
#include "QSLog.h"

#include <QtDebug>
#include <QDateTime>
#include <QtNetwork>

YahooHistorical::YahooHistorical ()
{
}

int YahooHistorical::command (QStringList &input, QString &dbPath, QString &output)
{
  // input format = YahooHistorical,START DATE,END DATE,SYMBOLS
  //                    0                1        2       3
  // each parm is delimited by a comma
  // the end of data is delimited by a new line
  // START DATE and END DATE is a YYYYMMDD format
  // SYMBOLS are the yahoo symbols to load

  // output format = 0 for ok, ERROR for error condition

  output = "ERROR";
  QSLog log;

  if (input.count() != 4)
  {
    log.message(QSLog::Error, QString("YahooHistorical: invalid parm count " + QString::number(input.count())));
    return 1;
  }

  QDateTime sd = QDateTime::fromString(input.at(1), QString("yyyyMMdd"));
  if (! sd.isValid())
  {
    log.message(QSLog::Error, QString("YahooHistorical: invalid start date " + input.at(1)));
    return 1;
  }
  
  QDateTime ed = QDateTime::fromString(input.at(2), QString("yyyyMMdd"));
  if (! ed.isValid())
  {
    log.message(QSLog::Error, QString("YahooHistorical: invalid end date " + input.at(2)));
    return 1;
  }

  QNetworkAccessManager manager;
  YahooUrlData data;
  data.ysymbol = input.at(3);
  data.adjustment = 1;

  // parse the yahoo symbol and convert
  YahooParseSymbol yps;
  if (yps.parse(data))
  {
    log.message(QSLog::Error, QString("YahooHistorical: symbol parse error " + input.at(3)));
    return 1;
  }

  // get the url
  url(sd, ed, data);

  // download the data
  QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(data.url)));
  QEventLoop e;
  connect(&manager, SIGNAL(finished(QNetworkReply *)), &e, SLOT(quit()));
  e.exec();

  // parse the data and save quotes
  QByteArray ba = reply->readAll();
//qDebug() << QString(ba);
  parse(ba, dbPath, data);

  output = "OK";

  return 0;
}

void YahooHistorical::url (QDateTime &sd, QDateTime &ed, YahooUrlData &data)
{
  data.url = "http://ichart.finance.yahoo.com/table.csv?s=";
  data.url.append(data.ysymbol);
  data.url.append("&d=" + QString::number(ed.date().month() - 1));
  data.url.append("&e=" + ed.date().toString("d"));
  data.url.append("&f=" + ed.date().toString("yyyy"));
  data.url.append("&a=" + QString::number(sd.date().month() - 1));
  data.url.append("&b=" + sd.date().toString("d"));
  data.url.append("&c=" + sd.date().toString("yyyy"));
  data.url.append("&ignore=.csv");
}

void YahooHistorical::parse (QByteArray &ba, QString &dbPath, YahooUrlData &data)
{
  QSLog log;
  QString ts(ba);
  QStringList ll = ts.split('\n');

  int line = 0;

  Symbol symbol;
  symbol.exchange = data.exchange;
  symbol.symbol = data.symbol;
  symbol.type = (int) Bar::Stock;

  QList<Bar> bars;
  int loop = 1; // skip past first line
  for (; loop < ll.count(); loop++)
  {
    line++;

    ts = ll[loop].remove('"');
    ts = ts.trimmed();

    if (! ts.length())
      continue;

    QStringList l = ts.split(",");
    if (l.count() != 7)
    {
      log.message(QSLog::Error, QString("YahooHistorical: " + data.ysymbol + " :line " + QString::number(line) + " # of bar fields, record skipped"));
      continue;
    }

    if (data.adjustment)
    {
      bool ok;
      double adjclose = l[6].toDouble(&ok);
      if (! ok)
      {
        log.message(QSLog::Error, QString("YahooHistorical: " + data.ysymbol + " :line " + QString::number(line) + " invalid adjusted close, record skipped"));
        continue;
      }

      // apply yahoo's adjustments through all the data, not just closing price
      // i.e. adjust for stock splits and dividends
      double factor = l[4].toDouble() / adjclose;
      if (factor != 1)
      {
        l[1] = QString::number(l[1].toDouble() / factor, 'f', 2);
        l[2] = QString::number(l[2].toDouble() / factor, 'f', 2);
        l[3] = QString::number(l[3].toDouble() / factor, 'f', 2);
        l[4] = QString::number(l[4].toDouble() / factor, 'f', 2);
        l[5] = QString::number(l[5].toDouble() * factor);
      }
    }

    l[0].append("160000"); // add the close time to the date

    // save data to database
    Bar bar;
    bar.setBarType(Bar::Stock);
    
    QDateTime dt = QDateTime::fromString(l.at(0), QString("yyyy-MM-ddHHmmss"));
    if (! dt.isValid())
    {
      log.message(QSLog::Error, QString("YahooHistorical: invalid date " + data.ysymbol + " " + l.at(0) + " :line " + QString::number(loop)));
      continue;
    }
    else
      bar.setDate(dt);

    if (bar.setOpen(l[1]))
    {
      log.message(QSLog::Error, QString("YahooHistorical: invalid open " + data.ysymbol + " " + l.at(1) + " :line " + QString::number(loop)));
      continue;
    }

    if (bar.setHigh(l[2]))
    {
      log.message(QSLog::Error, QString("YahooHistorical: invalid high " + data.ysymbol + " " + l.at(2) + " :line " + QString::number(loop)));
      continue;
    }

    if (bar.setLow(l[3]))
    {
      log.message(QSLog::Error, QString("YahooHistorical: invalid low " + data.ysymbol + " " + l.at(3) + " :line " + QString::number(loop)));
      continue;
    }

    if (bar.setClose(l[4]))
    {
      log.message(QSLog::Error, QString("YahooHistorical: invalid close " + data.ysymbol + " " + l.at(4) + " :line " + QString::number(loop)));
      continue;
    }

    if (bar.setVolume(l[5]))
    {
      log.message(QSLog::Error, QString("YahooHistorical: invalid volume " + data.ysymbol + " " + l.at(5) + " :line " + QString::number(loop)));
      continue;
    }

    if (bar.verify())
    {
      log.message(QSLog::Error, QString("YahooHistorical: invalid bar values for " + data.ysymbol + " at line " + QString::number(loop)));
      continue;
    }

    bars.append(bar);
  }

  // lock the mutex so we stop other threads from writing to the database
  g_mutex.lock();

  QuoteDataBase db(dbPath);
  db.transaction();
  db.setBars(symbol, bars);
  db.commit();

  // we are done writing, unlock the mutex so other threads can write
  g_mutex.unlock();
}

//**********************************************************
//**********************************************************
//**********************************************************

Plugin * createPlugin ()
{
  YahooHistorical *o = new YahooHistorical;
  return ((Plugin *) o);
}

