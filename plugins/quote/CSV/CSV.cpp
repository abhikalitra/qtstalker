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
#include "../../../src/FuturesData.h"
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

  set(tr("Input"), "", Setting::FileList);
  
  set(tr("Symbol"), "", Setting::Text);

  QStringList l;
  l.append("Stock");
  l.append("Futures");
  set(tr("Chart Type"), "Stock", Setting::List);
  setList(tr("Chart Type"), l);

  FuturesData *fd = new FuturesData;

  l = fd->getSymbolList();
  set(tr("Futures Symbol"), l[0], Setting::List);
  setList(tr("Futures Symbol"), l);

  l = fd->getMonths();
  set(tr("Futures Month"), l[0], Setting::List);
  setList(tr("Futures Month"), l);
  
  l.clear();
  l.append("DOHLCV");
  l.append("DOHLCVI");
  set(tr("Format"), l[0], Setting::List);
  setList(tr("Format"), l);

  delete fd;
  
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
  FuturesData *fd = new FuturesData;

  QStringList list = getList(tr("Input"));

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

    QString path = dataPath;
    if (! type.compare("Stock"))
    {
      path.append("/Stocks");

      QDir dir(path);
      if (! dir.exists(path, TRUE))
      {
        if (! dir.mkdir(path, TRUE))
        {
          qDebug("CSV plugin: Unable to create stocks directory");
          f.close();
          return;
        }
      }
    }
    else
    {
      if (! type.compare("Futures"))
      {
        path.append("/Futures");

        QDir dir(path);
        if (! dir.exists(path, TRUE))
        {
          if (! dir.mkdir(path, TRUE))
          {
            qDebug("CSV plugin: Unable to create futures directory");
            f.close();
            return;
          }
        }

        path.append("/");

        fd->setSymbol(getData(tr("Futures Symbol")));

        path.append(fd->getSymbol());
        if (! dir.exists(path, TRUE))
        {
          if (! dir.mkdir(path, TRUE))
          {
            qDebug("CSV plugin: Unable to create futures symbol directory");
            f.close();
            return;
          }
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
      if (! format.compare("DOHLCV"))
        details->set("Format", "Open|High|Low|Close|Volume", Setting::None);
      else
        details->set("Format", "Open|High|Low|Close|Volume|Open Interest", Setting::None);

      details->set("Chart Type", type, Setting::None);

      details->set("Symbol", symbol, Setting::None);

      if (! type.compare("Futures"))
      {
        details->set("Futures Type", fd->getSymbol(), Setting::None);
        details->set("Futures Month", getData(tr("Futures Month")), Setting::None);
        details->set("Title", fd->getName(), Setting::Text);
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

      QStringList l = QStringList::split(",", s, FALSE);
      
      // date
      QString date = l[0];
      if (date.length() != 8)
        continue;
      date.append("000000");

      // open
      QString open;
      if (setTFloat(l[1]))
        continue;
      else
        open = QString::number(tfloat);

      // high
      QString high;
      if (setTFloat(l[2]))
        continue;
      else
        high = QString::number(tfloat);

      // low
      QString low;
      if (setTFloat(l[3]))
        continue;
      else
        low = QString::number(tfloat);

      // close
      QString close;
      if (setTFloat(l[4]))
        continue;
      else
        close = QString::number(tfloat);

      // volume
      QString volume;
      if (setTFloat(l[5]))
        continue;
      else
        volume = QString::number(tfloat);

      // oi
      QString oi;
      if (l.count() == 7)
      {
        if (setTFloat(l[6]))
          continue;
        else
          oi = QString::number(tfloat);
      }

      // check for bad values
      if (close.toFloat() == 0)
        continue;

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

      Setting *r = new Setting;
      r->set("Date", date, Setting::Date);
      r->set("Open", open, Setting::Float);
      r->set("High", high, Setting::Float);
      r->set("Low", low, Setting::Float);
      r->set("Close", close, Setting::Float);
      r->set("Volume", volume, Setting::Float);
      if (oi.length())
        r->set("Open Interest", oi, Setting::Float);

      db->setRecord(r);

      delete r;
    }

    delete db;
    f.close();
  }

  delete fd;

  emit done();
}

Plugin * create ()
{
  CSV *o = new CSV;
  return ((Plugin *) o);
}


