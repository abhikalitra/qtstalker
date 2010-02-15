/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2008 Stefan S. Stratigakos
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
#include "QuoteDataBase.h"

#include <QtDebug>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QDir>
#include <QObject>



CSV::CSV ()
{
  clear();
}

int CSV::openDb (QString &file)
{
  if (file.isEmpty())
  {
    // no dbFile given, so we create the default one
    QDir dir(QDir::homePath());
    QString home = dir.absolutePath() + "/.CSV";
    if (! dir.exists(home))
    {
      if (! dir.mkdir(home))
      {
        qDebug() << "CSV::open: Unable to create" << home;
	return 1;
      }
    }

    dbFile = home + "/quotes.sqlite";
  }
  else
    dbFile = file;

  QuoteDataBase db;
  db.init(dbFile);

  return 0;
}

void CSV::clear ()
{
  date.clear();
  time.clear();
  symbol.clear();
  dbFile.clear();
  name.clear();
  exchange.clear();
  type.clear();
  open = "0";
  high = "0";
  low = "0";
  close = "0";
  volume = "0";
  oi = "0";
  delimiter = ",";
}

void CSV::import ()
{
  openDb(dbFile);

  if (inputFile.isEmpty())
  {
    importQuote();
    return;
  }

  QString ts, ts2;
  int records = 0;

  QStringList fieldList;
  fieldList = format.split(",");

  QFile f(inputFile);
  if (! f.open(QIODevice::ReadOnly))
  {
    qDebug() << "Error opening file" << inputFile;
    return;
  }
  QTextStream stream(&f);

  int lineCount = 0;
  QFileInfo fi(f);
  QString fName = fi.fileName();

  typedef QList<Bar> BarList;
  QHash<QString, BarList *> symbolHash;

  while(stream.atEnd() == 0)
  {
    ts = stream.readLine();
    QStringList l = ts.split(delimiter);

    // check if the # of fieldlist and data fields match
    lineCount++;

    if (l.count() != fieldList.count())
    {
      qDebug() << "Line:" << lineCount << "Number of fields in file (" << l.count() << ") != rule format (" << fieldList.count() << ")";
      continue;
    }

    int fieldLoop;
    bool flag = FALSE;
    bool dateFlag = FALSE;
    Bar r;
    for (fieldLoop = 0; fieldLoop < (int) fieldList.count(); fieldLoop++)
    {
      QString listItem = l[fieldLoop].trimmed();
      QByteArray ba;
      ba.append(fieldList[fieldLoop]);
      switch (ba[0])
      {
	case 'D':
	  if (setBarDate(r, listItem))
          {
            qDebug() << "Line:" << lineCount << "Bad date" << listItem;
            flag = TRUE;
	  }
	  else
            dateFlag = TRUE;
	  break;
	case 'T':
	  if (setBarTime(r, listItem))
	  {
            qDebug() << "Line:" << lineCount << "Bad time:" << listItem;
	    flag = TRUE;
	  }
	  break;
	case 'S':
          setBarSymbol(r, listItem);
          break;
	case 'O':
          if (setBarOpen(r, listItem))
	  {
            qDebug() << "Line:" << lineCount << "Bad value:" << listItem;
	    flag = TRUE;
          }
	  break;
	case 'H':
          if (setBarHigh(r, listItem))
	  {
            qDebug() << "Line:" << lineCount << "Bad value:" << listItem;
	    flag = TRUE;
          }
	  break;
	case 'L':
          if (setBarLow(r, listItem))
	  {
            qDebug() << "Line:" << lineCount << "Bad value:" << listItem;
	    flag = TRUE;
          }
	  break;
	case 'C':
          if (setBarClose(r, listItem))
	  {
            qDebug() << "Line:" << lineCount << "Bad value:" << listItem;
	    flag = TRUE;
          }
	  break;
	case 'V':
          if (setBarVolume(r, listItem))
	  {
            qDebug() << "Line:" << lineCount << "Bad value:" << listItem;
	    flag = TRUE;
          }
	  break;
	case 'I':
          if (setBarOI(r, listItem))
	  {
            qDebug() << "Line:" << lineCount << "Bad value:" << listItem;
	    flag = TRUE;
          }
	  break;
	case 'G':
	  // we just ignore this field. its a place holder
	  break;
	default:
	  break;
      }
    }

    if (flag)
      continue;

    if (! dateFlag)
    {
      qDebug() << "Date error. Skipping line" << lineCount;
      continue;
    }

    // check if we have a symbol, if not we use the filename
    r.getSymbol(ts);
    if (ts.isEmpty())
    {
      if (symbol.isEmpty())
      {
	QString ts2 = fName;
	convertSymbol(ts2);
        r.setSymbol(ts2);
	ts = ts2;
      }
      else
      {
	ts = symbol;  // use symbol from command line option
	convertSymbol(ts);
        r.setSymbol(ts);
      }
    }

    if (name.length())
      r.setName(name);

    if (exchange.length())
      r.setExchange(exchange);

    if (type.length())
      r.setType(type);

    if (symbolHash.contains(ts))
    {
      BarList *l = symbolHash.value(ts);
      l->append(r);
    }
    else
    {
      BarList *l = new BarList;
      l->append(r);
      symbolHash.insert(ts, l);
    }
  }

  f.close();

  QHashIterator<QString, BarList *> it(symbolHash);
  while (it.hasNext())
  {
    it.next();
    setChart(it.value());
    records = records + it.value()->count();
  }

  qDeleteAll(symbolHash);

  qDebug() << records << "Records Imported";
}

void CSV::importQuote ()
{
  Bar r;

  if (setBarDate(r, date))
  {
    qDebug() << "Bad date" << date;
    return;
  }

  if (setBarTime(r, time))
  {
    qDebug() << "Bad time" << time;
    return;
  }

  setBarSymbol(r, symbol);

  if (setBarOpen(r, open))
  {
    qDebug() << "Bad open value" << open;
    return;
  }

  if (setBarHigh(r, high))
  {
    qDebug() << "Bad high value" << high;
    return;
  }

  if (setBarLow(r, low))
  {
    qDebug() << "Bad low value" << low;
    return;
  }

  if (setBarClose(r, close))
  {
    qDebug() << "Bad close value" << close;
    return;
  }

  if (setBarVolume(r, volume))
  {
    qDebug() << "Bad volume value" << volume;
    return;
  }

  if (setBarOI(r, oi))
  {
    qDebug() << "Bad oi value" << oi;
    return;
  }

  if (name.length())
    r.setName(name);

  if (exchange.length())
    r.setExchange(exchange);

  if (type.length())
    r.setType(type);

  QList<Bar> *l = new QList<Bar>;
  l->append(r);
  setChart(l);
  delete l;

  qDebug() << "1 quote imported";
}

void CSV::setChart (QList<Bar> *bars)
{
  if (! bars->count())
    return;

  QuoteDataBase db;
  db.setChart(bars);
}

void CSV::setFormat (QString &d)
{
  format = d;
}

void CSV::setDateFormat (QString &d)
{
  dateFormat = d;
}

void CSV::setTimeFormat (QString &d)
{
  timeFormat = d;
}

void CSV::setDelimiter (QString &d)
{
  delimiter = d;
}

void CSV::setInputFile (QString &d)
{
  inputFile = d;
}

void CSV::setDate (QString &d)
{
  date = d;
}

void CSV::setTime (QString &d)
{
  time = d;
}

void CSV::setOpen (QString &d)
{
  open = d;
}

void CSV::setHigh (QString &d)
{
  high = d;
}

void CSV::setLow (QString &d)
{
  low = d;
}

void CSV::setClose (QString &d)
{
  close = d;
}

void CSV::setVolume (QString &d)
{
  volume = d;
}

void CSV::setOI (QString &d)
{
  oi = d;
}

void CSV::setSymbol (QString &d)
{
  convertSymbol(d);
  symbol = d;
}

int CSV::setBarDate (Bar &r, QString &d)
{
  int flag = FALSE;
  QDateTime dt = QDateTime::fromString(d, dateFormat);
  if (! dt.isValid())
    flag = TRUE;
  else
    r.setDate(dt);
  return flag;
}

int CSV::setBarTime (Bar &r, QString &d)
{
  int flag = FALSE;
  QTime dt = QTime::fromString(d, timeFormat);
  if (! dt.isValid())
    flag = TRUE;
  else
    r.setTime(dt);
  return flag;
}

void CSV::setBarSymbol (Bar &r, QString &d)
{
  convertSymbol(d);
  r.setSymbol(d);
}

int CSV::setBarOpen (Bar &r, QString &d)
{
  int flag = FALSE;
  bool ok;
  d.toDouble(&ok);
  if (! ok)
    flag = TRUE;
  else
  {
    QString k("Open");
    r.setData(k, d);
  }
  return flag;
}

int CSV::setBarHigh (Bar &r, QString &d)
{
  int flag = FALSE;
  bool ok;
  d.toDouble(&ok);
  if (! ok)
    flag = TRUE;
  else
  {
    QString k("High");
    r.setData(k, d);
  }
  return flag;
}

int CSV::setBarLow (Bar &r, QString &d)
{
  int flag = FALSE;
  bool ok;
  d.toDouble(&ok);
  if (! ok)
    flag = TRUE;
  else
  {
    QString k("Low");
    r.setData(k, d);
  }
  return flag;
}

int CSV::setBarClose (Bar &r, QString &d)
{
  int flag = FALSE;
  bool ok;
  d.toDouble(&ok);
  if (! ok)
    flag = TRUE;
  else
  {
    QString k("Close");
    r.setData(k, d);
  }
  return flag;
}

int CSV::setBarVolume (Bar &r, QString &d)
{
  int flag = FALSE;
  bool ok;
  d.toDouble(&ok);
  if (! ok)
    flag = TRUE;
  else
  {
    QString k("Volume");
    r.setData(k, d);
  }
  return flag;
}

int CSV::setBarOI (Bar &r, QString &d)
{
  int flag = FALSE;
  bool ok;
  d.toInt(&ok);
  if (! ok)
    flag = TRUE;
  else
  {
    QString k("OI");
    r.setData(k, d);
  }
  return flag;
}

// remove any unwanted characters in the symbol name
void CSV::convertSymbol (QString &d)
{
  d = d.remove(QString("'"), Qt::CaseSensitive); // remove '
}

void CSV::setDbFile (QString &d)
{
  dbFile = d;
}

void CSV::setName (QString &d)
{
  d = d.remove(QString("'"), Qt::CaseSensitive); // remove '
  name = d;
}

void CSV::setExchange (QString &d)
{
  d = d.remove(QString("'"), Qt::CaseSensitive); // remove '
  exchange = d;
}

void CSV::setType (QString &d)
{
  d = d.remove(QString("'"), Qt::CaseSensitive); // remove '
  type = d;
}

