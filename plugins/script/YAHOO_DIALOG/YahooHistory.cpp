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

#include "YahooHistory.h"
#include "BarData.h"
#include "QuoteDataBase.h"

#include <QDebug>
#include <QtNetwork>

YahooHistory::YahooHistory (QObject *p, QList<Setting> l) : QThread (p)
{
  _symbols = l;
  _stopFlag = FALSE;

  connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void YahooHistory::run ()
{
  int loop = 0;
  for (; loop < _symbols.count(); loop++)
  {
    if (_stopFlag)
    {
      emit signalMessage(QString("*** Download cancelled ***"));
      break;
    }
    
    Setting symbol = _symbols.at(loop);
    
    QDateTime sd = QDateTime::fromString(symbol.data("DATE_START"), Qt::ISODate);

    QDateTime ed = QDateTime::fromString(symbol.data("DATE_END"), Qt::ISODate);

    // get name
    downloadName(symbol);
    
    // get the url
    QString url;
    getUrl(sd, ed, symbol, url);

    // download the data
    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));
    QEventLoop e;
    connect(&manager, SIGNAL(finished(QNetworkReply *)), &e, SLOT(quit()));
    e.exec();
    
    // parse the data and save quotes
    QByteArray ba = reply->readAll();
    parse(ba, symbol);

    QStringList l;
    l << symbol.data("YSYMBOL") << "OK";
    emit signalMessage(l.join(" "));
  }

//  emit signalDone();
}

void YahooHistory::getUrl (QDateTime &sd, QDateTime &ed, Setting &data, QString &url)
{
  url = "http://ichart.finance.yahoo.com/table.csv?s=";
  url.append(data.data("YSYMBOL"));
  url.append("&d=" + QString::number(ed.date().month() - 1));
  url.append("&e=" + ed.date().toString("d"));
  url.append("&f=" + ed.date().toString("yyyy"));
  url.append("&a=" + QString::number(sd.date().month() - 1));
  url.append("&b=" + sd.date().toString("d"));
  url.append("&c=" + sd.date().toString("yyyy"));
  url.append("&ignore=.csv");
}

void YahooHistory::parse (QByteArray &ba, Setting &data)
{
  QString ts(ba);
  QStringList ll = ts.split('\n');

  int line = 0;

  BarData symbol;
  symbol.setExchange(data.data("EXCHANGE"));
  symbol.setSymbol(data.data("SYMBOL"));
  symbol.setName(data.data("NAME"));
  symbol.setType(QString("Stock"));

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
      QStringList tl;
      tl << data.data("YSYMBOL") << "line" << QString::number(line) << "# of bar fields, skipped";
      emit signalMessage(tl.join(" "));
      continue;
    }

    if (data.getInt("ADJUSTMENT"))
    {
      bool ok;
      double adjclose = l[6].toDouble(&ok);
      if (! ok)
      {
        QStringList tl;
        tl << data.data("YSYMBOL") << "line" << QString::number(line) << "invalid adjusted close, skipped";
        emit signalMessage(tl.join(" "));
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

    Bar *bar = new Bar;

    QDateTime dt = QDateTime::fromString(l.at(0), QString("yyyy-MM-ddHHmmss"));
    if (! dt.isValid())
    {
      QStringList tl;
      tl << data.data("YSYMBOL") << "invalid date" << l.at(0);
      emit signalMessage(tl.join(" "));
      continue;
    }
    else
      bar->setDates(dt, dt);

    if (bar->setOpen(l.at(1)))
    {
      QStringList tl;
      tl << data.data("YSYMBOL") << "invalid open" << l.at(1);
      emit signalMessage(tl.join(" "));
      continue;
    }

    if (bar->setHigh(l.at(2)))
    {
      QStringList tl;
      tl << data.data("YSYMBOL") << "invalid high" << l.at(2);
      emit signalMessage(tl.join(" "));
      continue;
    }

    if (bar->setLow(l.at(3)))
    {
      QStringList tl;
      tl << data.data("YSYMBOL") << "invalid low" << l.at(3);
      emit signalMessage(tl.join(" "));
      continue;
    }

    if (bar->setClose(l.at(4)))
    {
      QStringList tl;
      tl << data.data("YSYMBOL") << "invalid close" << l.at(4);
      emit signalMessage(tl.join(" "));
      continue;
    }

    if (bar->setVolume(l.at(5)))
    {
      QStringList tl;
      tl << data.data("YSYMBOL") << "invalid volume" << l.at(5);
      emit signalMessage(tl.join(" "));
      continue;
    }

    symbol.append(bar);
  }

  QuoteDataBase db;
  db.transaction();
  db.setBars(&symbol);
  db.commit();
}

void YahooHistory::downloadName (Setting &data)
{
  QString url = "http://download.finance.yahoo.com/d/quotes.csv?s=";
  url.append(data.data("YSYMBOL"));
  url.append("&f=n");
  url.append("&e=.csv");

  QNetworkAccessManager manager;
  QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));
  QEventLoop e;
  connect(&manager, SIGNAL(finished(QNetworkReply *)), &e, SLOT(quit()));
  e.exec();

  // parse the data and save quotes
  QByteArray ba = reply->readAll();
  QString s(ba);
  s = s.remove('"');
  s = s.remove(',');
  s = s.trimmed();
  if (s.isEmpty())
    return;

  data.setData("NAME", s);
}

void YahooHistory::stop ()
{
  _stopFlag = TRUE;
}
