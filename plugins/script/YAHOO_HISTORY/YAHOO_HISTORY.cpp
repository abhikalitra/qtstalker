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

#include "YAHOO_HISTORY.h"
#include "BarData.h"
#include "QuoteDataBase.h"
#include "ScriptPluginFactory.h"

#include <QDebug>
#include <QtNetwork>

YAHOO_HISTORY::YAHOO_HISTORY ()
{
  _plugin = "YAHOO_HISTORY";
}

int YAHOO_HISTORY::command (Command *command)
{
  // PARMS:
  // DATE_START
  // DATE_END
  // SYMBOL - colon delimited list

  // get DATE_START
  QString key = command->parm("DATE_START");
  if (key.isEmpty())
  {
    qDebug() << _plugin << "::command: DATE_START empty";
    return 1;
  }
  QDateTime sd = QDateTime::fromString(command->returnData(key), Qt::ISODate);
  if (! sd.isValid())
  {
    qDebug() << _plugin << "::command: invalid DATE_START" << command->returnData(key);
    return 1;
  }

  // get DATE_END
  key = command->parm("DATE_END");
  if (key.isEmpty())
  {
    qDebug() << _plugin << "::command: DATE_END empty";
    return 1;
  }
  QDateTime ed = QDateTime::fromString(command->returnData(key), Qt::ISODate);
  if (! ed.isValid())
  {
    qDebug() << _plugin << "::command: invalid DATE_END" << command->returnData(key);;
    return 1;
  }

  // get SYMBOL
  key = command->parm("SYMBOL");
  if (key.isEmpty())
  {
    qDebug() << _plugin << "::command: SYMBOL empty";
    return 1;
  }
  QStringList l = command->returnData(key).split(",", QString::SkipEmptyParts);

  ScriptPluginFactory fac;
  ScriptPlugin *plug = fac.plugin("YAHOO_DATABASE");
  if (! plug)
  {
    qDebug() << _plugin << "::command: no plugin";
    return 1;
  }

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QNetworkAccessManager manager;
    Setting data;
    data.setData("ysymbol", l.at(loop));
    data.setData("adjustment", 1);

    QStringList cl;
    cl << "PLUGIN=YAHOO_DATABASE" << "METHOD=LOAD" << "YSYMBOL=" + l.at(loop);
    Command tcommand(cl.join(","));
    if (plug->command(&tcommand))
    {
      qDebug() << _plugin << "::command: command error";
      continue;
    }

    data.setData("exchange", tcommand.returnData("YAHOO_DATABASE_EXCHANGE"));
    data.setData("symbol", tcommand.returnData("YAHOO_DATABASE_SYMBOL"));
    data.setData("name", tcommand.returnData("YAHOO_DATABASE_SYMBOL"));

    // get name
    downloadName(data);
    
    // get the url
    QString url;
    getUrl(sd, ed, data, url);

    // download the data
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));
    QEventLoop e;
    connect(&manager, SIGNAL(finished(QNetworkReply *)), &e, SLOT(quit()));
    e.exec();
    
    // parse the data and save quotes
    QByteArray ba = reply->readAll();
    parse(ba, data);
  }

  delete plug;

  command->setReturnCode("0");

  return 0;
}

void YAHOO_HISTORY::getUrl (QDateTime &sd, QDateTime &ed, Setting &data, QString &url)
{
  url = "http://ichart.finance.yahoo.com/table.csv?s=";
  url.append(data.data("ysymbol"));
  url.append("&d=" + QString::number(ed.date().month() - 1));
  url.append("&e=" + ed.date().toString("d"));
  url.append("&f=" + ed.date().toString("yyyy"));
  url.append("&a=" + QString::number(sd.date().month() - 1));
  url.append("&b=" + sd.date().toString("d"));
  url.append("&c=" + sd.date().toString("yyyy"));
  url.append("&ignore=.csv");
}

void YAHOO_HISTORY::parse (QByteArray &ba, Setting &data)
{
  QString ts(ba);
  QStringList ll = ts.split('\n');

  int line = 0;

  BarData symbol;
  symbol.setExchange(data.data("exchange"));
  symbol.setSymbol(data.data("symbol"));
  symbol.setName(data.data("name"));
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
      qDebug() << _plugin << "::parse:" << data.data("ysymbol") << "line" << QString::number(line) << " # of bar fields, record skipped";
      continue;
    }

    if (data.getInt("adjustment"))
    {
      bool ok;
      double adjclose = l[6].toDouble(&ok);
      if (! ok)
      {
        qDebug() << _plugin << "::parse:" << data.data("ysymbol") << "line" << QString::number(line) << " invalid adjusted close, record skipped";
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
      qDebug() << _plugin << "::parse: invalid date" << data.data("ysymbol") << l.at(0) << "line" << QString::number(loop);
      continue;
    }
    else
      bar->setDates(dt, dt);

    if (bar->setOpen(l.at(1)))
    {
      qDebug() << _plugin << "::parse: invalid open" << data.data("ysymbol") << l.at(1) << "line" << QString::number(loop);
      continue;
    }

    if (bar->setHigh(l.at(2)))
    {
      qDebug() << _plugin << "::parse: invalid high" << data.data("ysymbol") << l.at(2) << "line" << QString::number(loop);
      continue;
    }

    if (bar->setLow(l.at(3)))
    {
      qDebug() << _plugin << "::parse: invalid low" << data.data("ysymbol") << l.at(3) << "line" << QString::number(loop);
      continue;
    }

    if (bar->setClose(l.at(4)))
    {
      qDebug() << _plugin << "::parse: invalid close" << data.data("ysymbol") << l.at(4) << "line" << QString::number(loop);
      continue;
    }

    if (bar->setVolume(l.at(5)))
    {
      qDebug() << _plugin << "::parse: invalid volume" << data.data("ysymbol") << l.at(5) << "line" << QString::number(loop);
      continue;
    }

    symbol.append(bar);
  }

  QuoteDataBase db;
  db.transaction();
  db.setBars(&symbol);
  db.commit();
}

void YAHOO_HISTORY::downloadName (Setting &data)
{
  QNetworkAccessManager manager;

  QString url = "http://download.finance.yahoo.com/d/quotes.csv?s=";
  url.append(data.data("ysymbol"));
  url.append("&f=n");
  url.append("&e=.csv");

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

  data.setData("name", s);
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  YAHOO_HISTORY *o = new YAHOO_HISTORY;
  return ((ScriptPlugin *) o);
}
