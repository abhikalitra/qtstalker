/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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
#include "CSVDialog.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qdir.h>
#include <qsettings.h>

CSV::CSV ()
{
  pluginName = "CSV";
  delim = ",";
  db = 0;
  dateFlag = FALSE;
  
  edate = QDateTime::currentDateTime();
  if (edate.date().dayOfWeek() == 6)
    edate = edate.addDays(-1);
  else
  {
    if (edate.date().dayOfWeek() == 7)
      edate = edate.addDays(-2);
  }

  sdate = QDateTime::currentDateTime();
  sdate = sdate.addDays(-1);
  if (sdate.date().dayOfWeek() == 6)
    sdate = sdate.addDays(-1);
  else
  {
    if (sdate.date().dayOfWeek() == 7)
      sdate = sdate.addDays(-2);
  }
  
  loadSettings();
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
  setDelimiter();

  if (dateFlag)
  {
    if (sdate >= edate || edate <= sdate)
    {
      emit statusLogMessage(tr("Done"));
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

    QString symbol = symbolOveride;
    if (! symbol.length() && format.compare("SDOHLCV"))
    {
      QStringList l = QStringList::split("/", list[loop], FALSE);
      symbol = l[l.count() - 1];

      if (symbol.right(4).contains(".txt"))
        symbol.truncate(symbol.length() - 4);

      while (symbol.contains("_"))
        symbol = symbol.remove(symbol.find("_", 0, TRUE), 1);
    }

    QString type = chartType;

    QString path;
    if (! type.compare("Stock"))
    {
      path = createDirectory("Stocks");
      if (! path.length())
      {
        emit statusLogMessage("Unable to create stocks directory");
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
          emit statusLogMessage("Unable to create futures directory");
          f.close();
          return;
        }

        fd.setSymbol(futuresSymbol);

	QString s = "Futures/";
	s.append(fd.getSymbol());
        path = createDirectory(s);
        if (! path.length())
        {
          emit statusLogMessage("Unable to create futures symbol directory");
          f.close();
          return;
        }
      }
    }

    path.append("/");

    if (symbol.length())
    {
      QString s = path;
      s.append(symbol);
      openDb(s, symbol, type);
    }

    while(stream.atEnd() == 0)
    {
      QString s = stream.readLine();
      s = stripJunk(s);

      QStringList l = QStringList::split(delim, s, FALSE);

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

        if (! format.compare("SDOHLCV"))
        {
	  r = getSDOHLCV(l);
          break;
        }

	break;
      }

      if (r)
      {
        Bar *bar = new Bar;
        if (bar->setDate(r->getData("Date")))
        {
          delete bar;
          emit statusLogMessage("Bad date " + r->getData("Date"));
	  delete r;
          continue;
        }
        bar->setOpen(r->getFloat("Open"));
        bar->setHigh(r->getFloat("High"));
        bar->setLow(r->getFloat("Low"));
        bar->setClose(r->getFloat("Close"));
        bar->setVolume(r->getFloat("Volume"));
        bar->setOI(r->getInt("OI"));
      
        if (! symbol.length())
	{
	  s = path;
	  s.append(r->getData("Symbol"));
	  openDb(s, r->getData("Symbol"), type);
          db->setBar(bar);
	  emit dataLogMessage(r->getData("Symbol") + " " + bar->getString());
          delete db;
	  db = 0;
          s = tr("Updating ");
          s.append(r->getData("Symbol"));
	}
	else
	{
          db->setBar(bar);
	  emit dataLogMessage(r->getData("Symbol") + " " + bar->getString());
          s = tr("Updating ");
          s.append(symbol);
	}

        emit statusLogMessage(s);
        delete r;
	delete bar;
      }
    }

    if (db)
    {
      delete db;
      db = 0;
    }
    f.close();
  }

  emit statusLogMessage(tr("Done"));
  emit done();
}

void CSV::setDelimiter ()
{
  if (! delimiter.compare(tr("Comma")))
  {
    delim = ",";
    return;
  }

  if (! delimiter.compare(tr("Tab")))
  {
    delim = "	";
    return;
  }

  if (! delimiter.compare(tr("Space")))
  {
    delim = " ";
    return;
  }
}

QDate CSV::getDate (QString d)
{
  QDate date;
  QStringList l;
  QString s = d;

  while (1)
  {
    if (s.contains("/"))
    {
      l = QStringList::split("/", s, FALSE);
      if (l.count() != 3)
        return date;
      break;
    }

    if (s.contains("-"))
    {
      l = QStringList::split("-", s, FALSE);
      if (l.count() != 3)
        return date;
      break;
    }

    if (s.contains("."))
    {
      l = QStringList::split(".", s, FALSE);
      if (l.count() != 3)
        return date;
      break;
    }

    break;
  }

  while (1)
  {
    if (! dateFormat.compare(tr("YYYYMMDD")))
    {
      if (l.count())
        date.setYMD(l[0].toInt(), l[1].toInt(), l[2].toInt());
      else
      {
        if (s.length() == 8)
          date.setYMD(s.left(4).toInt(), s.mid(4, 2).toInt(), s.right(2).toInt());
      }
      break;
    }

    if (! dateFormat.compare(tr("YYMMDD")))
    {
      if (l.count())
        date.setYMD(l[0].toInt(), l[1].toInt(), l[2].toInt());
      else
      {
        if (s.length() == 6)
          date.setYMD(s.left(2).toInt(), s.mid(2, 2).toInt(), s.right(2).toInt());
      }
      break;
    }

    if (! dateFormat.compare(tr("MMDDYYYY")))
    {
      if (l.count())
        date.setYMD(l[2].toInt(), l[0].toInt(), l[1].toInt());
      else
      {
        if (s.length() == 8)
          date.setYMD(s.right(4).toInt(), s.left(2).toInt(), s.mid(2, 2).toInt());
      }
      break;
    }

    if (! dateFormat.compare(tr("MMDDYY")))
    {
      if (l.count())
        date.setYMD(l[2].toInt(), l[0].toInt(), l[1].toInt());
      else
      {
        if (s.length() == 6)
          date.setYMD(s.right(2).toInt(), s.left(2).toInt(), s.mid(2, 2).toInt());
      }
      break;
    }

    if (! dateFormat.compare(tr("DDMMYYYY")))
    {
      if (l.count())
        date.setYMD(l[2].toInt(), l[1].toInt(), l[0].toInt());
      else
      {
        if (s.length() == 8)
          date.setYMD(s.right(4).toInt(), s.mid(2, 2).toInt(), s.left(2).toInt());
      }
      break;
    }

    break;
  }

  return date;
}

void CSV::openDb (QString path, QString symbol, QString type)
{
  db = new ChartDb();
  db->openChart(path);

  QString s = db->getDetail(ChartDb::Symbol);
  if (! s.length())
  {
    db->setDetail(ChartDb::Symbol, symbol);
    db->setDetail(ChartDb::Type, type);

    if (! type.compare("Futures"))
    {
      db->setDetail(ChartDb::FuturesType, fd.getSymbol());
      db->setDetail(ChartDb::FuturesMonth, futuresMonth);
      db->setDetail(ChartDb::Title, fd.getName());
    }
    else
      db->setDetail(ChartDb::Title, type);
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
  r->setData("Date", date);
  r->setData("Open", open);
  r->setData("High", high);
  r->setData("Low", low);
  r->setData("Close", close);
  r->setData("Volume", volume);

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
  r->setData("Date", date);
  r->setData("Open", open);
  r->setData("High", high);
  r->setData("Low", low);
  r->setData("Close", close);
  r->setData("Volume", volume);
  r->setData("OI", oi);

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
  r->setData("Date", date);
  r->setData("Open", open);
  r->setData("High", high);
  r->setData("Low", low);
  r->setData("Close", close);

  return r;
}

Setting * CSV::getSDOHLCV (QStringList l)
{
  Setting *r = 0;

  QDate dt = getDate(l[1]);
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

  // symbol
  QString symbol = l[0];

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

  // volume
  QString volume;
  if (setTFloat(l[6]))
    return r;
  else
    volume = QString::number(tfloat);

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
  r->setData("Symbol", symbol);
  r->setData("Date", date);
  r->setData("Open", open);
  r->setData("High", high);
  r->setData("Low", low);
  r->setData("Close", close);
  r->setData("Volume", volume);

  return r;
}

void CSV::prefDialog ()
{
  CSVDialog *dialog = new CSVDialog();
  dialog->setCaption(tr("CSV Prefs"));
  dialog->setFuturesSymbol(fd.getSymbolList(), futuresSymbol);
  dialog->setFuturesMonth(fd.getMonths(), futuresMonth);

  QStringList l;
  l.append("DOHLCV");
  l.append("DOHLCVI");
  l.append("DTOHLC");
  l.append("SDOHLCV");
  dialog->setFormat(l, format);

  l.clear();
  l.append(tr("Comma"));
  l.append(tr("Tab"));
  l.append(tr("Space"));
  dialog->setDelimiter(l, delimiter);

  l.clear();
  l.append(tr("YYYYMMDD"));
  l.append(tr("YYMMDD"));
  l.append(tr("MMDDYY"));
  l.append(tr("MMDDYYYY"));
  l.append(tr("DDMMYYYY"));
  dialog->setDateFormat(l, dateFormat);

  dialog->setStartDate(sdate);
  dialog->setEndDate(edate);
  dialog->setDateRange(dateFlag);
          
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    list = dialog->getFiles();
    symbolOveride = dialog->getSymbol();
    chartType = dialog->getType();
    futuresSymbol = dialog->getFuturesSymbol();
    futuresMonth = dialog->getFuturesMonth();
    format = dialog->getFormat();
    delimiter = dialog->getDelimiter();
    dateFormat = dialog->getFormat();
    sdate = dialog->getStartDate();
    edate = dialog->getEndDate();
    dateFlag = dialog->getDateRange();
    
    saveFlag = TRUE;
  }
  
  delete dialog;
}

void CSV::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/CSV plugin");

  chartType = settings.readEntry("/ChartType", tr("Stock"));
  futuresSymbol = settings.readEntry("/FuturesSymbol", "AD");
  futuresMonth = settings.readEntry("/FuturesMonth", "F");
  format = settings.readEntry("/Format", "DOHLCV");
  delimiter = settings.readEntry("/Delimiter", tr("Comma"));
  dateFormat = settings.readEntry("/DateFormat", tr("YYYYMMDD"));
  
  QString s = settings.readEntry("/DateRange", "0");
  dateFlag = s.toInt();
  
  settings.endGroup();
}

void CSV::saveSettings ()
{
  if (! saveFlag)
    return;

  QSettings settings;
  settings.beginGroup("/Qtstalker/CSV plugin");
  
  settings.writeEntry("/ChartType", chartType);
  settings.writeEntry("/FuturesSymbol", futuresSymbol);
  settings.writeEntry("/FuturesMonth", futuresMonth);
  settings.writeEntry("/Delimiter", delimiter);
  settings.writeEntry("/DateFormat", dateFormat);
  settings.writeEntry("/DateRange", QString::number(dateFlag));
  
  settings.endGroup();
}

Plugin * create ()
{
  CSV *o = new CSV;
  return ((Plugin *) o);
}


