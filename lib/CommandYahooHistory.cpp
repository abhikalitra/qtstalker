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

#include "CommandYahooHistory.h"
#include "DataDateTime.h"
#include "DataBool.h"
#include "DataFile.h"

#include <QDebug>
#include <QtNetwork>
#include <QFile>

CommandYahooHistory::CommandYahooHistory (QObject *p) : Command (p)
{
  _name = "YAHOO_HISTORY";

  _values.insert(_ParmTypeDateStart, new DataDateTime(QDateTime::currentDateTime()));
  _values.insert(_ParmTypeDateEnd, new DataDateTime(QDateTime::currentDateTime()));
  _values.insert(_ParmTypeAdjusted, new DataBool(TRUE));

  QStringList tl;
  tl << "/tmp/yahoo.csv";
  _values.insert(_ParmTypeCSVFile, new DataFile(tl));

  _values.insert(_ParmTypeSymbolFile, new DataFile);
}

void CommandYahooHistory::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandYahooHistory::runScript: parse error");
    return;
  }

  // DATE_START
  QDateTime sd = _values.value(_ParmTypeDateStart)->toDateTime();

  // DATE_END
  QDateTime ed = _values.value(_ParmTypeDateEnd)->toDateTime();

  // ADJUSTED
  bool adjusted = _values.value(_ParmTypeAdjusted)->toBool();

  // CSV_FILE
  QStringList tl = _values.value(_ParmTypeCSVFile)->toList();
  if (! tl.count())
  {
    Command::error("CommandYahooHistory::runScript: invalid CSV file");
    return;
  }

  QString outFile;
  if (tl.count())
    outFile = tl.at(0);

  // SYMBOL_FILE
  QStringList symbolFiles = _values.value(_ParmTypeSymbolFile)->toList();
  if (! symbolFiles.count())
  {
    Command::error("CommandYahooHistory::runScript: invalid symbol file");
    return;
  }

  QFile f2(outFile);
  if (! f2.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    Command::error("CommandYahooHistory::runScript: file error " + outFile);
    return;
  }
  QTextStream out(&f2);

  QNetworkAccessManager manager;

  int loop = 0;
  for (; loop < symbolFiles.count(); loop++)
  {
    QFile f(symbolFiles.at(loop));
    if (! f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      qDebug() << "CommandYahooHistory::runScript: file error " << symbolFiles.at(loop);
      continue;
    }

    while (! f.atEnd())
    {
      QString symbol = f.readLine();
      symbol = symbol.trimmed();
      if (symbol.isEmpty())
        continue;

      // get name
      QString name;
      downloadName(symbol, name);

      // get the url
      QString url;
      getUrl(sd, ed, symbol, url);

      // download the data
      QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));
      QEventLoop e;
      connect(&manager, SIGNAL(finished(QNetworkReply *)), &e, SLOT(quit()));
      e.exec();

      // parse the data and save quotes
      QByteArray ba = reply->readAll();
      parse(ba, symbol, name, out, adjusted);
    }

    f.close();
  }

  Command::done(QString());
}

void CommandYahooHistory::getUrl (QDateTime sd, QDateTime ed, QString symbol, QString &url)
{
//http://ichart.finance.yahoo.com/table.csv?s=AAPL&d=1&e=22&f=2011&g=d&a=8&b=7&c=1984&ignore=.csv
  url = "http://ichart.finance.yahoo.com/table.csv?s=";
  url.append(symbol);
  url.append("&d=" + QString::number(ed.date().month() - 1));
  url.append("&e=" + ed.date().toString("d"));
  url.append("&f=" + ed.date().toString("yyyy"));
  url.append("&a=" + QString::number(sd.date().month() - 1));
  url.append("&b=" + sd.date().toString("d"));
  url.append("&c=" + sd.date().toString("yyyy"));
  url.append("&ignore=.csv");
}

void CommandYahooHistory::parse (QByteArray &ba, QString &symbol, QString &name, QTextStream &out, bool adjusted)
{
  QString ts(ba);
  QStringList ll = ts.split('\n');

  int line = 0;

  int error = 0;
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
      qDebug() << "CommandYahooHistory::parse: symbol" << tr("line") << QString::number(line) << tr("# of bar fields, record skipped");
      error++;
      continue;
    }

    if (adjusted)
    {
      bool ok;
      double adjclose = l[6].toDouble(&ok);
      if (! ok)
      {
        qDebug() << "CommandYahooHistory::parse: symbol"  << symbol << tr("line") << QString::number(line) << tr("invalid adjusted close, record skipped");
        error++;
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
        l[5] = QString::number(l[5].toDouble() * factor, 'f', 0);
      }
    }

    QStringList tl;
    tl << "YAHOO" << symbol << name << l.at(0) << l.at(1) << l.at(2) << l.at(3) << l.at(4) << l.at(5);

    out << tl.join(";") << "\n";
  }
}

int CommandYahooHistory::downloadName (QString symbol, QString &name)
{
  QString url = "http://download.finance.yahoo.com/d/quotes.csv?s=";
  url.append(symbol);
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
    return 1;

  name = s;

  return 0;
}
