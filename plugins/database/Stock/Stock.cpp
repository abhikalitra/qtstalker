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

#include "Stock.h"
#include "Bar.h"

#include <QtSql>
#include <QtDebug>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QHash>
#include <QList>

Stock::Stock ()
{
}

void Stock::getBars (BarData &data)
{
  QSqlQuery q(QSqlDatabase::database("quotes"));

  if (getIndexData(data))
    return;

  QString table = data.getTableName();
  
  QDateTime firstDate;
  getFirstDate(table, firstDate);

  QDateTime lastDate;
  getLastDate(table, lastDate);

  QHash<QString, Bar *> bars;
  QStringList dateList;

  while (1)
  {
    QString sd = firstDate.toString(Qt::ISODate);
    QString ed = lastDate.toString(Qt::ISODate);

    QString s = "SELECT date,open,high,low,close,volume FROM " + table;
    s.append(" WHERE date >='" + sd + "' AND date <='" + ed + "'");
    s.append(" ORDER BY date DESC");
    s.append(" LIMIT " + QString::number(data.getBarsRequested() * 2));
    q.exec(s);
    if (q.lastError().isValid())
    {
      qDebug() << "Stock::getBars:" << q.lastError().text();
      break;
    }

    while (q.next())
    {
      QDateTime dt = q.value(0).toDateTime();
      lastDate = dt;
      
      QDateTime sd, ed;
      setStartEndDates(dt, sd, ed, data.getBarLength());
      
      s = sd.toString(Qt::ISODate) + ed.toString(Qt::ISODate);
      Bar *bar = bars[s];
      if (! bar)
      {
	if (bars.count() == data.getBarsRequested())
	  break;

	dateList.append(s); // save new dateList entry
	bar = new Bar;
        bar->setDate(dt); // save actual date

	bars.insert(s, bar);

        bar->setOpen(q.value(1).toDouble());
        bar->setHigh(q.value(2).toDouble());
        bar->setLow(q.value(3).toDouble());
        bar->setClose(q.value(4).toDouble());
        bar->setVolume(q.value(5).toDouble());

	continue;
      }

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
    }

    if (bars.count() == data.getBarsRequested())
      break;

    if (lastDate == firstDate)
      break;
  }

  // order the bars from most recent to first
  int loop;
  for (loop = 0; loop < dateList.count(); loop++)
    data.prepend(bars[dateList[loop]]);
}

void Stock::setBars (BarData &bars)
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
    ts.append(")");
    q.exec(ts);
    if (q.lastError().isValid())
    {
      qDebug() << "Stock::setBars: save quote" << q.lastError().text();
      qDebug() << ts;
    }
  }

  commit();
}

int Stock::createTable (BarData &bars)
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
  s.append(")");
  int rc = command(s, QString("Stock::createTable: create new symbol table"));
  
  return rc;
}

int Stock::scriptCommand (QStringList &l)
{
  // format = QUOTE_SET,PLUGIN,EXCHANGE,SYMBOL,DATE_FORMAT,DATE,OPEN,HIGH,LOW,CLOSE,VOLUME*
  
  if (l.count() < 11)
  {
    qDebug() << "Stock::scriptCommand: invalid parm count" << l.count();
    qDebug() << l;
    return 1;
  }

  if (verifyExchangeName(l[2]))
  {
    qDebug() << "Stock::scriptCommand: invalid exchange" << l[2];
    return 1;
  }
  
  int pos = 1;
  BarData bd;
  bd.setPlugin(l[pos++]);
  bd.setExchange(l[pos++]);
  bd.setSymbol(l[pos++]);
  
  QString format = l[pos++];
  
  int t = (l.count() - 5) % 6;
  if (t)
  {
    qDebug() << "Stock::scriptCommand: # of fields incorrect";
    return 1;
  }
  
  int count = (l.count() - 5) / 6;
  int record = 1;
  for (; pos < count; pos = pos + 6, record++)
  {
    QDateTime date = QDateTime::fromString(l[pos], format);
    if (! date.isValid())
    {
      qDebug() << "Stock::scriptCommand: invalid date or date format, record#" << record << format << l[pos];
      continue;
    }
  
    bool ok;
    double open = l[pos + 1].toDouble(&ok);
    if (! ok)
    {
      qDebug() << "Stock::scriptCommand: invalid open, record#" << record << l[pos + 1];
      continue;
    }

    double high = l[pos + 2].toDouble(&ok);
    if (! ok)
    {
      qDebug() << "Stock::scriptCommand: invalid high, record#" << record << l[pos + 2];
      continue;
    }

    double low = l[pos + 3].toDouble(&ok);
    if (! ok)
    {
      qDebug() << "Stock::scriptCommand: invalid low, record#" << record << l[pos + 3];
      continue;
    }

    double close = l[pos + 4].toDouble(&ok);
    if (! ok)
    {
      qDebug() << "Stock::scriptCommand: invalid close, record#" << record << l[pos + 4];
      continue;
    }

    double volume = l[pos + 5].toDouble(&ok);
    if (! ok)
    {
      qDebug() << "Stock::scriptCommand: invalid volume, record#" << record << l[pos + 5];
      continue;
    }
  
    if (open > high)
    {
      qDebug() << "Stock::scriptCommand: open > high, record#" << record << open << high;
      continue;
    }
    
    if (open < low)
    {
      qDebug() << "Stock::scriptCommand: open < low, record#" << record << open << low;
      continue;
    }
    
    if (close > high)
    {
      qDebug() << "Stock::scriptCommand: close > high, record#" << record << close << high;
      continue;
    }
    
    if (close < low)
    {
      qDebug() << "Stock::scriptCommand: close < low, record#" << record << close << low;
      continue;
    }
    
    if (volume < 0)
    {
      qDebug() << "Stock::scriptCommand: negative volume, record#" << record << volume;
      continue;
    }
    
    if (low > high)
    {
      qDebug() << "Stock::scriptCommand: low > high, record#" << record << low << high;
      continue;
    }
    
    if (high < low)
    {
      qDebug() << "Stock::scriptCommand: high < low, record#" << record << high << low;
      continue;
    }
    
    Bar *bar = new Bar;
    bar->setDate(date);
    bar->setOpen(open);
    bar->setHigh(high);
    bar->setLow(low);
    bar->setClose(close);
    bar->setVolume(volume);
    bd.append(bar);
  }

  transaction();
  setBars(bd);
  commit();
  
  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

DBPlugin * createDBPlugin ()
{
  Stock *o = new Stock;
  return ((DBPlugin *) o);
}

