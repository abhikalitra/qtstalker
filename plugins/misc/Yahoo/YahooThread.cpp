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

#include "YahooThread.h"
#include "YahooDataBase.h"
#include "DBPlugin.h"
#include "DBPluginFactory.h"

#include <QStringList>
#include <QDebug>

YahooThread::YahooThread ()
{
  _type = History;
  _manager = new QNetworkAccessManager(this);
  connect(_manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(requestFinished(QNetworkReply *)));
}

void YahooThread::downloadHistory (QDateTime sd, QDateTime ed, int adj, QString &ysymbol)
{
  _type = History;
  _sdate = sd;
  _edate = ed;
  _adjustment = adj;
  _data.ysymbol = ysymbol;
  
  run();
}

void YahooThread::downloadInfo (QString &d)
{
  _type = Info;
  _data.ysymbol = d;

  run();
}

void YahooThread::run ()
{
  switch (_type)
  {
    case History:
      createHistoryURL();
      break;
    case Info:
      createInfoURL();
      break;
    default:
      break;
  }

  _manager->get(QNetworkRequest(QUrl(_data.url)));
}

void YahooThread::requestFinished (QNetworkReply *reply)
{
  QByteArray ba = reply->readAll();
  reply->deleteLater();

  switch (_type)
  {
    case History:
      parseHistory(ba);
      break;
    case Info:
      parseInfo(ba);
      break;
    default:
      break;
  }
      
  emit signalDone();
}

void YahooThread::createHistoryURL ()
{
  // dividend download
  // http://ichart.finance.yahoo.com/table.csv?s=XIU.TO&a=09&b=4&c=1999&d=05&e=11&f=2010&g=v&ignore=.csv
  
  YahooDataBase ydb;
  ydb.getSymbol(_data.ysymbol, _data.symbol, _data.exchange);
  
  _data.url = "http://ichart.finance.yahoo.com/table.csv?s=";
  _data.url.append(_data.ysymbol);
  _data.url.append("&d=" + QString::number(_edate.date().month() - 1));
  _data.url.append("&e=" + _edate.date().toString("d"));
  _data.url.append("&f=" + _edate.date().toString("yyyy"));
  _data.url.append("&a=" + QString::number(_sdate.date().month() - 1));
  _data.url.append("&b=" + _sdate.date().toString("d"));
  _data.url.append("&c=" + _sdate.date().toString("yyyy"));
  _data.url.append("&ignore=.csv");
}

void YahooThread::createInfoURL ()
{
  // dividend download
  // http://ichart.finance.yahoo.com/table.csv?s=XIU.TO&a=09&b=4&c=1999&d=05&e=11&f=2010&g=v&ignore=.csv
  // s = symbol
  // x = exchange
  // n = name

  YahooDataBase ydb;
  ydb.getSymbol(_data.ysymbol, _data.symbol, _data.exchange);

  _data.url = "http://download.finance.yahoo.com/d/quotes.csv?s=";
  _data.url.append(_data.ysymbol);
  _data.url.append("&f=sxn");
  _data.url.append("&e=.csv");
}

void YahooThread::parseHistory (QByteArray &ba)
{
  DBPluginFactory fac;
  QString type = "Stock";
  Indicator ind;
  DBPlugin *plug = fac.plugin(type);
  if (! plug)
  {
    emit signalMessage(tr("Invalid plugin"));
    return;
  }

  QString ts(ba);
  QStringList ll = ts.split('\n');

  int records = 0;
  int line = 0;

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
      QString s = _data.ysymbol + tr(":line ") + QString::number(line) + ":record not imported";
      emit signalMessage(s);
      continue;
    }

    // construct a script API command for the appropriate plugin
    QStringList apil;
    apil << "QUOTE" << type << "SET_QUOTE" << _data.exchange << _data.symbol << "yyyy-MM-dd";

    if (_adjustment)
    {
      bool ok;
      double adjclose = l[6].toDouble(&ok);
      if (! ok)
      {
        QString s = _data.symbol + tr(":line ") + QString::number(line) + ":record not imported";
        emit signalMessage(s);
        continue;
      }
      
      // apply yahoo's adjustments through all the data, not just closing price
      // i.e. adjust for stock splits and dividends
      float factor = l[4].toDouble() / adjclose;
      if (factor != 1)
      {
        l[1] = QString::number(l[1].toDouble() / factor);
        l[2] = QString::number(l[2].toDouble() / factor);
        l[3] = QString::number(l[3].toDouble() / factor);
        l[4] = QString::number(l[4].toDouble() / factor);
        l[5] = QString::number(l[5].toDouble() * factor);
      }
    }
    
    apil << l[0] << l[1] << l[2] << l[3] << l[4] << l[5];

    // send the script API command
    if (! plug->scriptCommand(apil, ind))
      records++;
  }

  // construct the script API save quotes command and send it
  QStringList apil;
  apil << "QUOTE" << type << "SAVE_QUOTES";
  QString s;
  if (plug->scriptCommand(apil, ind))
    s = _data.ysymbol + tr(": db error, quotes not saved...import aborted");
  else
    s = _data.ysymbol + ":" + QString::number(records) + " records imported";

  emit signalMessage(s);
}

void YahooThread::parseInfo (QByteArray &ba)
{
  DBPluginFactory fac;
  QString type = "Stock";
  Indicator ind;
  DBPlugin *plug = fac.plugin(type);
  if (! plug)
  {
    emit signalMessage(tr("Invalid plugin"));
    return;
  }

  QString ts(ba);
  ts = ts.remove('"');
  ts = ts.trimmed();

  if (! ts.length())
    return;

  QStringList l = ts.split(",");
  if (l.count() != 3)
  {
    QString s = _data.ysymbol + ":info not imported";
    emit signalMessage(s);
    return;
  }

  // construct a script API command for the appropriate plugin
  QStringList apil;
  apil << "QUOTE" << type << "SET_NAME" << _data.exchange << _data.symbol << l[2];

  // send the script API command
  plug->scriptCommand(apil, ind);
}


/*
  // test code for an exchange rename to fix yahoo code
  UrlData data;
  data = _data;
  if (l[1] == "NasdaqNM")
  {
    data.exchange = "XNAS";
  }
  else if (l[1] == "AMEX")
  {
    data.exchange = "ARCX";
  }

  if (_data.exchange != data.exchange)
  {
    QStringList l;
    l << "QUOTE" << type << "RENAME" << _data.exchange << _data.symbol << data.exchange << _data.symbol;

    if (plug->scriptCommand(l, ind))
    {
      qDebug() << "YahooThread::parseInfo: rename failed";
    }
    else
    {
      YahooDataBase ydb;
      ydb.setSymbol(_data.ysymbol, _data.symbol, data.exchange);
      _data = data;
    }
  }

*/

