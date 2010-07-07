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

#include "YahooQuotes.h"
#include "YahooParseSymbol.h"
#include "QSQuoteDataBase.h"
#include "QSSymbol.h"
#include "QSBar.h"
#include "QSGlobals.h"
#include "QSLog.h"

#include <QtDebug>
#include <QDateTime>
#include <QtNetwork>

YahooQuotes::YahooQuotes ()
{
}

int YahooQuotes::command (QStringList &input, QString &dbPath, QString &output)
{
  QSLog log;
  output = "ERROR";

  if (input.count() < 2)
  {
    log.message(QSLog::Error, QString("YahooQuotes: invalid parm count " + QString::number(input.count())));
    return 1;
  }

  if (input.at(1) == "Start")
    return startUpdate(input, dbPath, output);
  else if (input.at(1) == "Stop")
    return stopUpdate(output);

  log.message(QSLog::Error, QString("YahooQuotes: invalid command " + input.at(1)));
  return 1;
}

int YahooQuotes::startUpdate (QStringList &input, QString &dbPath, QString &output)
{
  // input format = YahooQuotes,Start,SYMBOLS
  //                    0         1     2
  // each parm is delimited by a comma
  // the end of data is delimited by a new line
  // START DATE and END DATE is a YYYYMMDD format
  // SYMBOLS are the yahoo symbols to load

  // output format = 0 for ok, ERROR for error condition


  // check if an instance is already running
/*  
  if (yahooQuotesStatus == 1)
  {
    qDebug() << "YahooQuotes::startUpdate: an instance is currently running. Command ignored.";
    return 1;
  }
*/

  QSLog log;
  
  if (input.count() < 3)
  {
    log.message(QSLog::Error, QString("YahooQuotes: invalid parm count " + QString::number(input.count())));
    return 1;
  }

  // remove the first two items to get just the symbols
  QStringList symbols = input;
  symbols.removeFirst();
  symbols.removeFirst();

  // check if we are over the yahoo 200 symbol limit
  if (symbols.count() > 200)
  {
    log.message(QSLog::Error, QString("YahooQuotes: over the yahoo 200 symbol limit (" + QString::number(symbols.count()) + "). Command ignored"));
    return 1;
  }

  // get the url
  YahooUrlData data;
  url(symbols, data);

  g_mutex.lock();
  g_yahooQuotesStatus = 1;
  g_mutex.unlock();

//  qDebug() << "YahooQuotes::startUpdate: update started";
  
  while (g_yahooQuotesStatus ==  1)
  {
    QNetworkAccessManager manager;
    
    // download the data
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(data.url)));
    QEventLoop e;
    connect(&manager, SIGNAL(finished(QNetworkReply *)), &e, SLOT(quit()));
    e.exec();

    // parse the data and save quotes
    QByteArray ba = reply->readAll();
    parse(ba, dbPath);
    qDebug() << QString(ba);

//    qDebug() << "YahooQuotes::startUpdate: a cycle has completed";

    sleep(60);
  }

  log.message(QSLog::Info, QString("YahooQuotes: update has been stopped"));

  output = "OK";

  return 0;
}

int YahooQuotes::stopUpdate (QString &output)
{
  // input format = YahooQuotes,Stop

  QSLog log;
  
  if (g_yahooQuotesStatus != 1)
  {
    log.message(QSLog::Error, QString("YahooQuotes: update is either shutting down or already stopped. Command ignored."));
    return 1;
  }

  g_mutex.lock();
  g_yahooQuotesStatus = 0;
  g_mutex.unlock();

  log.message(QSLog::Info, QString("YahooQuotes: update shutting down...waiting for next cycle to complete"));

  output = "OK";

  return 0;
}

void YahooQuotes::url (QStringList &symbols, YahooUrlData &data)
{
  data.url = "http://download.finance.yahoo.com/d/quotes.csv?s=";
  data.url.append(symbols.join("+"));
  data.url.append("&f=sd1t1l1v"); // symbol,last date, last time, last price, volume
  data.url.append("&e=.csv");
}

void YahooQuotes::parse (QByteArray &ba, QString &dbPath)
{
  QSLog log;
  QString ts(ba);
  QStringList ll = ts.split('\n');

  QList<QSBar> bars;
  QList<QSSymbol> symbols;
  
  int loop = 0;
  for (; loop < ll.count(); loop++)
  {
    ts = ll[loop].remove('"');
    ts = ts.trimmed();

    if (ts.isEmpty())
      continue;

    QStringList l = ts.split(",");
    if (l.count() != 5)
    {
      log.message(QSLog::Error, QString("YahooQuotes: " + l.at(0) + " invalid # bar fields at line " + QString::number(loop) + " record skipped"));
      continue;
    }

    YahooUrlData data;
    data.ysymbol = l.at(0);

    // parse the yahoo symbol and convert
    YahooParseSymbol yps;
    if (yps.parse(data))
    {
      log.message(QSLog::Error, QString("YahooQuotes: symbol parse error " + l.at(0)));
      continue;
    }

    // save data to database
    QSBar bar;
    bar.setBarType(QSBar::Stock);

    QDateTime dt = QDateTime::fromString(l.at(1) + l.at(2), QString("M/d/yyyyh:mmap"));
    if (! dt.isValid())
    {
      log.message(QSLog::Error, QString("YahooQuotes: invalid date " + l.at(1) + l.at(2) + " at line " + QString::number(loop)));       
      continue;
    }
    else
      bar.setDate(dt);

    if (bar.setOpen(l[3]))
    {
      log.message(QSLog::Error, QString("YahooQuotes: invalid open " + l.at(3) + " at line " + QString::number(loop)));
      continue;
    }

    if (bar.setHigh(l[3]))
    {
      log.message(QSLog::Error, QString("YahooQuotes: invalid high " + l.at(3) + " at line " + QString::number(loop)));
      continue;
    }

    if (bar.setLow(l[3]))
    {
      log.message(QSLog::Error, QString("YahooQuotes: invalid low " + l.at(3) + " at line " + QString::number(loop)));
      continue;
    }

    if (bar.setClose(l[3]))
    {
      log.message(QSLog::Error, QString("YahooQuotes: invalid close " + l.at(3) + " at line " + QString::number(loop)));
      continue;
    }

    if (bar.verify())
    {
      log.message(QSLog::Error, QString("YahooQuotes: invalid bar values at line " + QString::number(loop)));
      continue;
    }

    QSSymbol symbol;
    symbol.symbol = data.symbol;
    symbol.exchange = data.exchange;
    symbols.append(symbol);
    symbol.type = (int) QSBar::Stock;
    
    bars.append(bar);
  }

  // lock the mutex so we stop other threads from writing to the database
  g_mutex.lock();

  QSQuoteDataBase db(dbPath);
  db.transaction();
  loop = 0;
  for (; loop < bars.count(); loop++)
  {
    QList<QSBar> tbars;
    tbars.append(bars.at(loop));

    db.setBars(symbols[loop], tbars);
  }
  db.commit();

  // we are done writing, unlock the mutex so other threads can write
  g_mutex.unlock();
}

//**********************************************************
//**********************************************************
//**********************************************************

QSPlugin * createPlugin ()
{
  YahooQuotes *o = new YahooQuotes;
  return ((QSPlugin *) o);
}

