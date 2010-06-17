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

#include "YahooUrl.h"
#include "YahooDataBase.h"

#include <QStringList>
#include <QDateTime>

YahooUrl::YahooUrl ()
{
}

void YahooUrl::history (QDateTime sd, QDateTime ed, int adjustment, QList<YahooUrlData> &urls)
{
  urls.clear();

  YahooDataBase db;
  QStringList l;
  db.getSymbols(l);

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    YahooUrlData data;
    data.ysymbol = l[loop];
    data.type = UrlTypeHistory;
    data.adjustment = adjustment;
    
    db.getSymbol(data.ysymbol, data.symbol, data.exchange);

    data.url = "http://ichart.finance.yahoo.com/table.csv?s=";
    data.url.append(data.ysymbol);
    data.url.append("&d=" + QString::number(ed.date().month() - 1));
    data.url.append("&e=" + ed.date().toString("d"));
    data.url.append("&f=" + ed.date().toString("yyyy"));
    data.url.append("&a=" + QString::number(sd.date().month() - 1));
    data.url.append("&b=" + sd.date().toString("d"));
    data.url.append("&c=" + sd.date().toString("yyyy"));
    data.url.append("&ignore=.csv");

    urls.append(data);
  }
}

void YahooUrl::details (QList<YahooUrlData> &urls)
{
  // s = symbol
  // x = exchange
  // n = name

  urls.clear();

  YahooDataBase db;
  QStringList l;
  db.getSymbols(l);

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    YahooUrlData data;
    data.ysymbol = l[loop];
    data.type = UrlTypeDetails;
    
    db.getSymbol(data.ysymbol, data.symbol, data.exchange);

    data.url = "http://download.finance.yahoo.com/d/quotes.csv?s=";
    data.url.append(data.ysymbol);
    data.url.append("&f=sxn");
    data.url.append("&e=.csv");

    urls.append(data);
  }
}

