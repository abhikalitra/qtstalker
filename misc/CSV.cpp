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

#include <QtDebug>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QDir>
#include <QObject>



CSV::CSV ()
{
  QDir dir(QDir::homePath());
  QString home = dir.absolutePath() + "/.CSV";
  if (! dir.exists(home))
  {
    if (! dir.mkdir(home))
      qDebug() << "CSV::Unable to create" << home;
  }

  QString s = home + "/quotes.sqlite";

  QSqlDatabase db = QSqlDatabase::database("quotes");
  if (! db.isValid())
  {
    db = QSqlDatabase::addDatabase("QSQLITE", "quotes");
    db.setHostName("localhost");
    db.setDatabaseName(s);
    db.setUserName("CSV");
    db.setPassword("CSV");
    if (! db.open())
      qDebug() << "CSV::CSV: quotes db open failed";
  }

  QSqlQuery q(db);
  s = "CREATE TABLE IF NOT EXISTS symbolIndex (";
  s.append(" symbol VARCHAR(50) PRIMARY KEY UNIQUE");
  s.append(", name VARCHAR(100)");
  s.append(", exchange VARCHAR(25)");
  s.append(", data VARCHAR(250)");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "CSV::CSV:createSymbolIndexTable: " << q.lastError().text();

  clear();
}

void CSV::clear ()
{
  date.clear();
  time.clear();
  symbol.clear();
  open = "0";
  high = "0";
  low = "0";
  close = "0";
  volume = "0";
  oi = "0";
}

void CSV::import ()
{
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

    if (fi.suffix() == "txt" || fi.suffix() == "TXT")
      ts = fi.completeBaseName();
    else
      ts = fi.fileName();
    r.setSymbol(ts);

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

  Bar bar = bars->at(0);
  QString tsymbol;
  bar.getSymbol(tsymbol);

  QStringList formatList = format.split(",");

  // check to see if symbol exists
  QSqlDatabase db = QSqlDatabase::database("quotes");
  if (! db.isValid())
  {
    qDebug() << "CSV::setChart:" << db.lastError().text();
    return;
  }

  QSqlQuery q(db);
  QString ts = "SELECT * FROM symbolIndex WHERE symbol='" + tsymbol + "'";
  q.exec(ts);
  if (q.lastError().isValid())
  {
    qDebug() << q.lastError().text();
    qDebug() << ts;
    return;
  }

  if (! q.next())
  {
    // new symbol, create new table for it
    ts = "CREATE TABLE IF NOT EXISTS " + tsymbol + " (date DATETIME PRIMARY KEY UNIQUE";
    ts.append(", open REAL, high REAL, low REAL, close REAL, volume INT, oi INT)");
    q.exec(ts);
    if (q.lastError().isValid())
    {
      qDebug() << "CSV::setChart:create new symbol table: " << q.lastError().text();
      qDebug() << ts;
      return;
    }

    // add new symbol entry into the symbol index table
    ts = "REPLACE INTO symbolIndex (symbol) VALUES('" + tsymbol + "')";
    q.exec(ts);
    if (q.lastError().isValid())
    {
      qDebug() << "CSV::setChart:create new symbol index record: " << q.lastError().text();
      qDebug() << ts;
      return;
    }
  }

  db.transaction();

  int loop;
  QString tdate;
  for (loop = 0; loop < bars->count(); loop++)
  {
    Bar bar = bars->at(loop);
    bar.getDateTimeString(tdate);

    ts = "REPLACE INTO " + tsymbol + " VALUES('" + tdate + "'";

    QString k = "Open";
    QString d;
    bar.getData(k, d);
    if (d.isEmpty())
      ts.append(",0");
    else
      ts.append("," + d);

    k = "High";
    bar.getData(k, d);
    if (d.isEmpty())
      ts.append(",0");
    else
      ts.append("," + d);

    k = "Low";
    bar.getData(k, d);
    if (d.isEmpty())
      ts.append(",0");
    else
      ts.append("," + d);

    k = "Close";
    bar.getData(k, d);
    if (d.isEmpty())
      ts.append(",0");
    else
      ts.append("," + d);

    k = "Volume";
    bar.getData(k, d);
    if (d.isEmpty())
      ts.append(",0");
    else
      ts.append("," + d);

    k = "OI";
    bar.getData(k, d);
    if (d.isEmpty())
      ts.append(",0)");
    else
      ts.append("," + d + ")");

    q.exec(ts);
    if (q.lastError().isValid())
    {
      qDebug() << "CSV::setChart:save quote in symbol table: " << q.lastError().text();
      qDebug() << ts;
    }
  }

  db.commit();
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
  // remove any unwanted characters in the symbol name
  d.replace(QString(" "), QString("_")); // replace spaces
  d.replace(QString("-"), QString("_")); // replace minus signs
  d.replace(QString("&"), QString("_")); // replace ampersand
  d.replace(QString("."), QString("_")); // replace dot
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
  d.toDouble(&ok);
  if (! ok)
    flag = TRUE;
  else
  {
    QString k("OI");
    r.setData(k, d);
  }
  return flag;
}



