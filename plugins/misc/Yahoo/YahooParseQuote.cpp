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

#include "YahooParseQuote.h"
#include "DBPlugin.h"
#include "DBPluginFactory.h"
#include "Indicator.h"

#include <QStringList>
#include <QDebug>

YahooParseQuote::YahooParseQuote ()
{
}

void YahooParseQuote::history (QByteArray &ba, YahooUrlData &data)
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
      QString s = data.ysymbol + tr(":line ") + QString::number(line) + ":record not imported";
      emit signalMessage(s);
      continue;
    }

    // construct a script API command for the appropriate plugin
    QStringList apil;
    apil << "QUOTE" << type << "SET_QUOTE" << data.exchange << data.symbol << "yyyy-MM-ddHHmmss";

    if (data.adjustment)
    {
      bool ok;
      double adjclose = l[6].toDouble(&ok);
      if (! ok)
      {
        QString s = data.symbol + tr(":line ") + QString::number(line) + ":record not imported";
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

    l[0].append("160000"); // add the close time to the date
    apil << l[0] << l[1] << l[2] << l[3] << l[4] << l[5];

    // send the script API command
    QByteArray tba;
    if (! plug->scriptCommand(apil, ind, tba))
      records++;
  }

  // construct the script API save quotes command and send it
  QStringList apil;
  QByteArray tba;
  apil << "QUOTE" << type << "SAVE_QUOTES";
  QString s;
  if (plug->scriptCommand(apil, ind, tba))
    s = data.ysymbol + tr(": db error, quotes not saved...import aborted");
  else
    s = data.ysymbol + ":" + QString::number(records) + " records imported";

  emit signalMessage(s);
}

void YahooParseQuote::details (QByteArray &ba, YahooUrlData &data)
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
    QString s = data.ysymbol + ":info not imported";
    emit signalMessage(s);
    return;
  }

  BarData bd;
  bd.setExchange(data.exchange);
  bd.setSymbol(data.symbol);
  ts = "NAME";
  plug->setDetail(ts, &bd, l[2]);
}

