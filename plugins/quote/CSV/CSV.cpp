/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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

#include "CSV.h"
#include "../../../src/ChartDb.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qdir.h>

CSV::CSV ()
{
  pluginName = "CSV";
  createFlag = FALSE;
  delimiter = ",";

  set(tr("Input"), "", Setting::FileList);

  set(tr("Symbol"), "", Setting::Text);

  QStringList l;
  l.append("Stock");
  l.append("Futures");
  set(tr("Chart Type"), "Stock", Setting::List);
  setList(tr("Chart Type"), l);

  l = fd.getSymbolList();
  set(tr("Futures Symbol"), l[0], Setting::List);
  setList(tr("Futures Symbol"), l);

  l = fd.getMonths();
  set(tr("Futures Month"), l[0], Setting::List);
  setList(tr("Futures Month"), l);

  l.clear();
  l.append("DOHLCV");
  l.append("DOHLCVI");
  l.append("DTOHLC");
  set(tr("Format"), l[0], Setting::List);
  setList(tr("Format"), l);

  l.clear();
  l.append(tr("Comma"));
  l.append(tr("Tab"));
  l.append(tr("Space"));
  set(tr("Delimiter"), tr("Comma"), Setting::List);
  setList(tr("Delimiter"), l);

  l.clear();
  l.append(tr("YYYYMMDD"));
  l.append(tr("YYMMDD"));
  l.append(tr("MMDDYY"));
  l.append(tr("MMDDYYYY"));
  set(tr("Date Format"), tr("YYYYMMDD"), Setting::List);
  setList(tr("Date Format"), l);

  QDate date = QDate::currentDate();
  if (date.dayOfWeek() == 6)
    date = date.addDays(-1);
  else
  {
    if (date.dayOfWeek() == 7)
      date = date.addDays(-2);
  }
  set("Date End", date.toString("yyyyMMdd"), Setting::Date);

  date = date.addDays(-1);
  if (date.dayOfWeek() == 6)
    date = date.addDays(-1);
  else
  {
    if (date.dayOfWeek() == 7)
      date = date.addDays(-2);
  }
  set("Date Start", date.toString("yyyyMMdd"), Setting::Date);

  set("Select Date Range", tr("False"), Setting::Bool);

  about = "Imports ASCII CSV files.\n";
}

CSV::~CSV ()
{
}

void CSV::update ()
{
  QTimer::singleShot(250, this, SLOT(parse()));
}

void CSV::parse ()
{
  QStringList list = getList(tr("Input"));

  dateFormat = getData(tr("Date Format"));

  setDelimiter();

  dateFlag = FALSE;
  if (! getData(tr("Select Date Range")).compare(tr("True")))
  {
    dateFlag = TRUE;
    sdate = QDateTime::fromString(getDateTime("Date Start"), Qt::ISODate);
    edate = QDateTime::fromString(getDateTime("Date End"), Qt::ISODate);
    if (sdate >= edate || edate <= sdate)
    {
      emit done();
      return;
    }
  }

  int loop;
  for (loop = 0; loop < (int) list.count(); loop++)
  {
    QFile f(list[loop]);
    if (! f.open(IO_ReadOnly))
      continue;
    QTextStream stream(&f);

    QString symbol = getData(tr("Symbol"));
    if (! symbol.length())
    {
      QStringList l = QStringList::split("/", list[loop], FALSE);
      symbol = l[l.count() - 1];

      if (symbol.right(4).contains(".txt"))
        symbol.truncate(symbol.length() - 4);
	
      while (symbol.contains("_"))
        symbol = symbol.remove(symbol.find("_", 0, TRUE), 1);
    }

    QString type = getData(tr("Chart Type"));

    QString path;
    if (! type.compare("Stock"))
    {
      path = createDirectory("Stocks");
      if (! path.length())
      {
        qDebug("CSV plugin: Unable to create stocks directory");
        f.close();
        return;
      }
    }
    else
    {
      if (! type.compare("Futures"))
      {
        path = createDirectory("Futures");
        if (! path.length())
	{
          qDebug("CSV plugin: Unable to create futures directory");
          f.close();
          return;
        }

        fd.setSymbol(getData(tr("Futures Symbol")));

	QString s = "Futures/";
	s.append(fd.getSymbol());
        path = createDirectory(s);
        if (! path.length())
        {
          qDebug("CSV plugin: Unable to create futures symbol directory");
          f.close();
          return;
        }
      }
    }

    path.append("/");
    path.append(symbol);
    ChartDb *db = new ChartDb();
    db->openChart(path);

    QString format = getData(tr("Format"));

    Setting *details = db->getDetails();
    if (! details->count())
    {
      newChart(details);

      details->set("Chart Type", type, Setting::None);
      details->set("Symbol", symbol, Setting::None);

      if (! type.compare("Futures"))
      {
        details->set("Futures Type", fd.getSymbol(), Setting::None);
        details->set("Futures Month", getData(tr("Futures Month")), Setting::None);
        details->set("Title", fd.getName(), Setting::Text);
      }
      else
        details->set("Title", symbol, Setting::Text);

      db->saveDetails();
      db->setFormat();
    }

    QString s = tr("Updating ");
    s.append(symbol);
    emit message(s);

    while(stream.atEnd() == 0)
    {
      s = stream.readLine();
      s = stripJunk(s);

      QStringList l = QStringList::split(delimiter, s, FALSE);

      Setting *r = 0;

      while (1)
      {
        if (! format.compare("DOHLCV"))
        {
	  r = getDOHLCV(l);
          break;
        }

        if (! format.compare("DOHLCVI"))
        {
	  r = getDOHLCVI(l);
          break;
        }

        if (! format.compare("DTOHLC"))
        {
	  r = getDTOHLC(l);
          break;
        }

        break;
      }

      if (r)
      {
        db->setRecord(r);
        delete r;
      }
    }

    delete db;
    f.close();
  }

  emit done();
}

void CSV::setDelimiter ()
{
  if (! getData(tr("Delimiter")).compare(tr("Comma")))
  {
    delimiter = ",";
    return;
  }

  if (! getData(tr("Delimiter")).compare(tr("Tab")))
  {
    delimiter = "	";
    return;
  }

  if (! getData(tr("Delimiter")).compare(tr("Space")))
  {
    delimiter = " ";
    return;
  }
}

QDate CSV::getDate (QString d)
{
  QDate date;

  QString s = d;
  while (s.contains("/"))
    s.remove(s.find("/", 0, TRUE), 1);

  while (s.contains("-"))
    s.remove(s.find("-", 0, TRUE), 1);

  while (1)
  {
    if (! dateFormat.compare(tr("YYYYMMDD")))
    {
      if (s.length() != 8)
        break;
      date.setYMD(s.left(4).toInt(), s.mid(4, 2).toInt(), s.right(2).toInt());
      break;
    }

    if (! dateFormat.compare(tr("YYMMDD")))
    {
      if (s.length() != 6)
        break;
      date.setYMD(s.left(2).toInt(), s.mid(2, 2).toInt(), s.right(2).toInt());
      break;
    }

    if (! dateFormat.compare(tr("MMDDYYYY")))
    {
      if (s.length() != 8)
        break;
      date.setYMD(s.right(4).toInt(), s.left(2).toInt(), s.mid(2, 2).toInt());
      break;
    }

    if (! dateFormat.compare(tr("MMDDYY")))
    {
      if (s.length() != 6)
        break;
      date.setYMD(s.right(2).toInt(), s.left(2).toInt(), s.mid(2, 2).toInt());
      break;
    }

    break;
  }

  return date;
}

void CSV::newChart (Setting *details)
{
  QString format = getData(tr("Format"));

  while (1)
  {
    if (! format.compare("DOHLCV"))
    {
      details->set("Format", "Open|High|Low|Close|Volume", Setting::None);
      break;
    }

    if (! format.compare("DOHLCVI"))
    {
      details->set("Format", "Open|High|Low|Close|Volume|Open Interest", Setting::None);
      break;
    }

    if (! format.compare("DTOHLC"))
    {
      details->set("Format", "Open|High|Low|Close", Setting::None);
      break;
    }

    break;
  }
}

Setting * CSV::getDOHLCV (QStringList l)
{
  Setting *r = 0;

  QDate dt = getDate(l[0]);
  if (! dt.isValid())
    return r;

  if (dateFlag)
  {
    if (dt < sdate.date() || dt > edate.date())
      return r;
  }

  // date
  QString date = dt.toString("yyyyMMdd");
  date.append("000000");

  // open
  QString open;
  if (setTFloat(l[1]))
    return r;
  else
    open = QString::number(tfloat);

  // high
  QString high;
  if (setTFloat(l[2]))
    return r;
  else
    high = QString::number(tfloat);

  // low
  QString low;
  if (setTFloat(l[3]))
    return r;
  else
    low = QString::number(tfloat);

  // close
  QString close;
  if (setTFloat(l[4]))
    return r;
  else
    close = QString::number(tfloat);

  // volume
  QString volume;
  if (setTFloat(l[5]))
    return r;
  else
    volume = QString::number(tfloat);

  // check for bad values
  if (close.toFloat() == 0)
    return r;

  if (open.toFloat() == 0 || high.toFloat() == 0 || low.toFloat() == 0 || volume.toFloat() == 0)
  {
    open = close;
    high = close;
    low = close;
  }

  if (open.toFloat() > high.toFloat() || open.toFloat() < low.toFloat())
  {
    open = close;
    high = close;
    low = close;
  }

  if (close.toFloat() > high.toFloat() || close.toFloat() < low.toFloat())
  {
    open = close;
    high = close;
    low = close;
  }

  r = new Setting;
  r->set("Date", date, Setting::Date);
  r->set("Open", open, Setting::Float);
  r->set("High", high, Setting::Float);
  r->set("Low", low, Setting::Float);
  r->set("Close", close, Setting::Float);
  r->set("Volume", volume, Setting::Float);

  return r;
}

Setting * CSV::getDOHLCVI (QStringList l)
{
  Setting *r = 0;

  QDate dt = getDate(l[0]);
  if (! dt.isValid())
    return r;

  if (dateFlag)
  {
    if (dt < sdate.date() || dt > edate.date())
      return r;
  }

  // date
  QString date = dt.toString("yyyyMMdd");
  date.append("000000");

  // open
  QString open;
  if (setTFloat(l[1]))
    return r;
  else
    open = QString::number(tfloat);

  // high
  QString high;
  if (setTFloat(l[2]))
    return r;
  else
    high = QString::number(tfloat);

  // low
  QString low;
  if (setTFloat(l[3]))
    return r;
  else
    low = QString::number(tfloat);

  // close
  QString close;
  if (setTFloat(l[4]))
    return r;
  else
    close = QString::number(tfloat);

  // volume
  QString volume;
  if (setTFloat(l[5]))
    return r;
  else
    volume = QString::number(tfloat);

  // oi
  QString oi;
  if (setTFloat(l[6]))
    return r;
  else
    oi = QString::number(tfloat);

  // check for bad values
  if (close.toFloat() == 0)
    return r;

  if (open.toFloat() == 0 || high.toFloat() == 0 || low.toFloat() == 0 || volume.toFloat() == 0)
  {
    open = close;
    high = close;
    low = close;
  }

  if (open.toFloat() > high.toFloat() || open.toFloat() < low.toFloat())
  {
    open = close;
    high = close;
    low = close;
  }

  if (close.toFloat() > high.toFloat() || close.toFloat() < low.toFloat())
  {
    open = close;
    high = close;
    low = close;
  }

  r = new Setting;
  r->set("Date", date, Setting::Date);
  r->set("Open", open, Setting::Float);
  r->set("High", high, Setting::Float);
  r->set("Low", low, Setting::Float);
  r->set("Close", close, Setting::Float);
  r->set("Volume", volume, Setting::Float);
  r->set("Open Interest", oi, Setting::Float);

  return r;
}

Setting * CSV::getDTOHLC (QStringList l)
{
  Setting *r = 0;

  QDate dt = getDate(l[0]);
  if (! dt.isValid())
    return r;

  if (dateFlag)
  {
    if (dt < sdate.date() || dt > edate.date())
      return r;
  }

  // date
  QString date = dt.toString("yyyyMMdd");
  date.append("000000");

  // open
  QString open;
  if (setTFloat(l[2]))
    return r;
  else
    open = QString::number(tfloat);

  // high
  QString high;
  if (setTFloat(l[3]))
    return r;
  else
    high = QString::number(tfloat);

  // low
  QString low;
  if (setTFloat(l[4]))
    return r;
  else
    low = QString::number(tfloat);

  // close
  QString close;
  if (setTFloat(l[5]))
    return r;
  else
    close = QString::number(tfloat);

  // check for bad values
  if (close.toFloat() == 0)
    return r;

  if (open.toFloat() == 0 || high.toFloat() == 0 || low.toFloat() == 0)
  {
    open = close;
    high = close;
    low = close;
  }

  if (open.toFloat() > high.toFloat() || open.toFloat() < low.toFloat())
  {
    open = close;
    high = close;
    low = close;
  }

  if (close.toFloat() > high.toFloat() || close.toFloat() < low.toFloat())
  {
    open = close;
    high = close;
    low = close;
  }

  r = new Setting;
  r->set("Date", date, Setting::Date);
  r->set("Open", open, Setting::Float);
  r->set("High", high, Setting::Float);
  r->set("Low", low, Setting::Float);
  r->set("Close", close, Setting::Float);

  return r;
}

Plugin * create ()
{
  CSV *o = new CSV;
  return ((Plugin *) o);
}


