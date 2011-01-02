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
}

int YAHOO_HISTORY::command (Command *command)
{
  // YAHOO_HISTORY,<START DATE>,<END DATE>,SYMBOL*
  //       0             1           2       3           
  
  if (command->count() < 4)
  {
    qDebug() << "YAHOO_HISTORY::command: invalid parm count" << command->count();
    return 1;
  }

  int pos = 1;
  QDateTime sd = QDateTime::fromString(command->parm(pos), Qt::ISODate);
  if (! sd.isValid())
  {
    qDebug() << "YAHOO_HISTORY::command: invalid start date" << command->parm(pos);
    return 1;
  }

  pos++;
  QDateTime ed = QDateTime::fromString(command->parm(pos), Qt::ISODate);
  if (! ed.isValid())
  {
    qDebug() << "YAHOO_HISTORY::command: invalid end date" << command->parm(pos);
    return 1;
  }

  ScriptPluginFactory fac;
  ScriptPlugin *plug = fac.plugin("YAHOO_DATABASE");
  if (! plug)
  {
    qDebug() << "YAHOO_HISTORY::command: no plugin";
    return 1;
  }

  pos++;
  for (; pos < command->count(); pos++)
  {
    QNetworkAccessManager manager;
    Setting data;
    data.setData("ysymbol", command->parm(pos));
    data.setData("adjustment", 1);

    QStringList cl;
    cl << "YAHOO_DATABASE" << "LOAD" << command->parm(pos);
    Command tcommand(cl.join(","));
    if (plug->command(&tcommand))
    {
      qDebug() << "YahooDialog::done: command error";
      continue;
    }

    cl << tcommand.stringData().split(",", QString::SkipEmptyParts);
    if (cl.count() != 2)
      continue;

    data.setData("exchange", cl.at(0));
    data.setData("symbol", cl.at(1));

    // get the url
    getUrl(sd, ed, data);

    // download the data
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(data.data("url"))));
    QEventLoop e;
    connect(&manager, SIGNAL(finished(QNetworkReply *)), &e, SLOT(quit()));
    e.exec();

    // parse the data and save quotes
    QByteArray ba = reply->readAll();
    parse(ba, data);
  }

  delete plug;

  command->setReturnData("0");

  return 0;
}

void YAHOO_HISTORY::getUrl (QDateTime &sd, QDateTime &ed, Setting &data)
{
  QString s = "http://ichart.finance.yahoo.com/table.csv?s=";
  s.append(data.data("ysymbol"));
  s.append("&d=" + QString::number(ed.date().month() - 1));
  s.append("&e=" + ed.date().toString("d"));
  s.append("&f=" + ed.date().toString("yyyy"));
  s.append("&a=" + QString::number(sd.date().month() - 1));
  s.append("&b=" + sd.date().toString("d"));
  s.append("&c=" + sd.date().toString("yyyy"));
  s.append("&ignore=.csv");
  data.setData("url", s);
}

void YAHOO_HISTORY::parse (QByteArray &ba, Setting &data)
{
  QString ts(ba);
  QStringList ll = ts.split('\n');

  int line = 0;

  BarData symbol;
  symbol.setExchange(data.data("exchange"));
  symbol.setSymbol(data.data("symbol"));
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
      qDebug() << "YAHOO_HISTORY::parse:" << data.data("ysymbol") << "line" << QString::number(line) << " # of bar fields, record skipped";
      continue;
    }

    if (data.getInt("adjustment"))
    {
      bool ok;
      double adjclose = l[6].toDouble(&ok);
      if (! ok)
      {
        qDebug() << "YAHOO_HISTORY::parse:" << data.data("ysymbol") << "line" << QString::number(line) << " invalid adjusted close, record skipped";
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
      qDebug() << "YAHOO_HISTORY::parse: invalid date" << data.data("ysymbol") << l.at(0) << "line" << QString::number(loop);
      continue;
    }
    else
      bar->setDates(dt, dt);

    if (bar->setOpen(l.at(1)))
    {
      qDebug() << "YAHOO_HISTORY::parse: invalid open" << data.data("ysymbol") << l.at(1) << "line" << QString::number(loop);
      continue;
    }

    if (bar->setHigh(l.at(2)))
    {
      qDebug() << "YAHOO_HISTORY::parse: invalid high" << data.data("ysymbol") << l.at(2) << "line" << QString::number(loop);
      continue;
    }

    if (bar->setLow(l.at(3)))
    {
      qDebug() << "YAHOO_HISTORY::parse: invalid low" << data.data("ysymbol") << l.at(3) << "line" << QString::number(loop);
      continue;
    }

    if (bar->setClose(l.at(4)))
    {
      qDebug() << "YAHOO_HISTORY::parse: invalid close" << data.data("ysymbol") << l.at(4) << "line" << QString::number(loop);
      continue;
    }

    if (bar->setVolume(l.at(5)))
    {
      qDebug() << "YAHOO_HISTORY::parse: invalid volume" << data.data("ysymbol") << l.at(5) << "line" << QString::number(loop);
      continue;
    }

    symbol.append(bar);
  }

  QuoteDataBase db;
  db.transaction();
  db.setBars(&symbol);
  db.commit();
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  YAHOO_HISTORY *o = new YAHOO_HISTORY;
  return ((ScriptPlugin *) o);
}
