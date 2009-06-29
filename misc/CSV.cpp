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
#include <QtSql>
#include <QCoreApplication>



CSV::CSV (CSVRule &r)
{
  rule = r;
  timer = 0;

  QString s;
  rule.getInterval(s);
  interval = s.toInt();
  if (interval)
  {
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(import()));
    timer->start(interval * 1000);
  }
}

void CSV::importSingleShot ()
{
  QTimer::singleShot(200, this, SLOT(import()));
}

void CSV::status ()
{
  QString s;
  rule.getName(s);

  if (timer)
    emit signalActive(s);
  else
    emit signalInactive(s);
}

void CSV::import ()
{
  QString ts, ts2;

  if (! timer)
  {
    rule.getName(ts);
    emit signalActive(ts);
    QCoreApplication::processEvents(QEventLoop::AllEvents);
  }

  QStringList fileList;
  rule.getFileList(fileList);

  QStringList fieldList;
  rule.getFormat(ts);
  fieldList = ts.split(",");

  QString dateFormat;
  rule.getDateFormat(dateFormat);
  
  QString delim;
  rule.getDelimiter(delim);

  int loop;
  for (loop = 0; loop < (int) fileList.count(); loop++)
  {
    QFile f(fileList[loop]);
    if (! f.open(QIODevice::ReadOnly))
    {
      emit signalMessage(QString(tr("Error opening file ") + fileList[loop]));
      continue;
    }
    QTextStream stream(&f);

    emit signalMessage(QString(tr("Importing file ") + fileList[loop]) + QDateTime::currentDateTime().toString(" yyyy-MM-dd HH:mm:ss.zzz"));

    int lineCount = 0;
    QFileInfo fi(f);
    QString fName = fi.fileName();

    typedef QList<Bar> BarList;
    QHash<QString, BarList *> symbolHash;

    while(stream.atEnd() == 0)
    {
      ts = stream.readLine();
      QStringList l = ts.split(delim);

      lineCount++;
      if (l.count() != fieldList.count())
      {
        ts = QString(fName + " - " + tr("Line") + ": " + QString::number(lineCount) + " Number of fields in file (" + QString::number(l.count())
                     + ") != rule format (" + QString::number(fieldList.count()) + ")");
        emit signalMessage(ts);
        continue;
      }
      
      int fieldLoop;
      bool flag = FALSE;
      bool dateFlag = FALSE;
      Bar r;
      for (fieldLoop = 0; fieldLoop < (int) fieldList.count(); fieldLoop++)
      {
        QString listItem = l[fieldLoop].trimmed();
        if (fieldList[fieldLoop].contains("Date"))
	{
          QDateTime dt = QDateTime::fromString(listItem, dateFormat);
          dt.setTime(QTime(0,0,0,0));
          if (! dt.isValid())
	  {
            ts = QString(fName + " - " + tr("Line") + ": " + QString::number(lineCount) + " " + tr("Bad date") + ": " + listItem);
            emit signalMessage(ts);
	    flag = TRUE;
	    break;
	  }
	  
          r.date = dt;
          dateFlag = TRUE;
	  continue;
	}

        if (! fieldList[fieldLoop].compare("Symbol"))
	{
          r.symbol = listItem;
	  continue;
	}

	if (! fieldList[fieldLoop].compare("Open"))
	{
          bool ok;
          double td = listItem.toDouble(&ok);
          if (! ok)
	  {
            ts = QString(fName + " - " + tr("Line") + ": " + QString::number(lineCount) + " " + tr("Bad value") + ": " + fieldList[fieldLoop]);
            emit signalMessage(ts);
	    flag = TRUE;
	    break;
	  }

          r.open = td;
	  continue;
	}
	
	if (! fieldList[fieldLoop].compare("High"))
	{
          bool ok;
          double td = listItem.toDouble(&ok);
          if (! ok)
	  {
            ts = QString(fName + " - " + tr("Line") + ": " + QString::number(lineCount) + " " + tr("Bad value") + ": " + fieldList[fieldLoop]);
            emit signalMessage(ts);
	    flag = TRUE;
	    break;
	  }

          r.high = td;
	  continue;
	}
	
	if (! fieldList[fieldLoop].compare("Low"))
	{
          bool ok;
          double td = listItem.toDouble(&ok);
          if (! ok)
	  {
            ts = QString(fName + " - " + tr("Line") + ": " + QString::number(lineCount) + " " + tr("Bad value") + ": " + fieldList[fieldLoop]);
            emit signalMessage(ts);
	    flag = TRUE;
	    break;
	  }

          r.low = td;
	  continue;
	}
	
	if (! fieldList[fieldLoop].compare("Close"))
	{
          bool ok;
          double td = listItem.toDouble(&ok);
          if (! ok)
	  {
            ts = QString(fName + " - " + tr("Line") + ": " + QString::number(lineCount) + " " + tr("Bad value") + ": " + fieldList[fieldLoop]);
            emit signalMessage(ts);
	    flag = TRUE;
	    break;
	  }

          r.close = td;
	  continue;
	}
	
	if (! fieldList[fieldLoop].compare("Volume"))
	{
          bool ok;
          double td = listItem.toDouble(&ok);
          if (! ok)
	  {
            ts = QString(fName + " - " + tr("Line") + ": " + QString::number(lineCount) + " " + tr("Bad value") + ": " + fieldList[fieldLoop]);
            emit signalMessage(ts);
	    flag = TRUE;
	    break;
	  }

          r.volume = td;
	  continue;
	}

	if (! fieldList[fieldLoop].compare("OI"))
	{
          bool ok;
          int td = listItem.toInt(&ok);
          if (! ok)
	  {
            ts = QString(fName + " - " + tr("Line") + ": " + QString::number(lineCount) + " " + tr("Bad value") + ": " + fieldList[fieldLoop]);
            emit signalMessage(ts);
	    flag = TRUE;
	    break;
	  }

          r.oi = td;
	}
      }
      
      if (flag)
	continue;

      if (! dateFlag)
      {
        emit signalMessage(QString(tr("Date error. Skipping line ") + QString::number(lineCount)));
	continue;
      }
      
      if (rule.getFileNameSymbol())
      {
        ts = fi.baseName();
        r.symbol = ts;
      }

      r.symbol = ts;
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
    }    

    qDeleteAll(symbolHash);

    QCoreApplication::processEvents(QEventLoop::AllEvents);
  }

  if (! timer)
  {
    rule.getName(ts);
    emit signalInactive(ts);
  }

  emit signalMessage(QString(tr("Done ") + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz")));
}

void CSV::setChart (QList<Bar> *bars)
{
  if (! bars->count())
    return;

  QSqlQuery q(QSqlDatabase::database("quotes"));

  Bar bar = bars->at(0);
  QString symbol = bar.symbol;

  // check to see if symbol exists
  QString ts = "SELECT firstDate,lastDate,type FROM symbolIndex WHERE symbol='" + symbol + "'";
  q.exec(ts);
  if (q.lastError().isValid())
  {
    qDebug() << q.lastError().text();
    return;
  }

  QDateTime firstDate, lastDate;

  if (! q.next())
  {
    // new symbol, create new table for it
    ts = "CREATE TABLE " + symbol + " (date INT PRIMARY KEY, open REAL, high REAL, low REAL, close REAL, volume INT, oi INT)";
    q.exec(ts);
    if (q.lastError().isValid())
    {
      qDebug() << "CSV::setChart:create new symbol table: " << q.lastError().text();
      return;
    }

    // add new symbol entry into the symbol index table
    firstDate = bar.date;
    lastDate = bar.date;

    QString s = bar.date.toString("yyyyMMddHHmmsszzz");
    ts = "INSERT OR REPLACE INTO symbolIndex (symbol,firstDate,lastDate) VALUES('" + symbol + "'," + s + "," + s + ")";
    q.exec(ts);
    if (q.lastError().isValid())
    {
      qDebug() << "CSV::setChart:create new symbol index record: " << q.lastError().text();
      return;
    }
  }
  else
  {
    // get the start date
    ts = q.value(0).toString();
    firstDate = QDateTime::fromString(ts, "yyyyMMddHHmmsszzz");

    // get the end date
    ts = q.value(1).toString();
    lastDate = QDateTime::fromString(ts, "yyyyMMddHHmmsszzz");
  }

  ts = "BEGIN TRANSACTION";
  q.exec(ts);
  if (q.lastError().isValid())
  {
    qDebug() << "CSV::setChart:begin transaction: " << q.lastError().text();
    return;
  }

  QDateTime dt;
  int loop;
  QString date;
  int flag = FALSE;
  for (loop = 0; loop < bars->count(); loop++)
  {
    Bar bar = bars->at(loop);
    date = bar.date.toString("yyyyMMddHHmmsszzz");
    dt = bar.date;

    ts = "INSERT OR REPLACE INTO " + symbol + " VALUES(" + date + "," + QString::number(bar.open) + ","
         + QString::number(bar.high) + "," + QString::number(bar.low) + "," + QString::number(bar.close) + ","
         + QString::number(bar.volume) + "," + QString::number(bar.oi) + ")";
    q.exec(ts);
    if (q.lastError().isValid())
      qDebug() << "CSV::setChart:save quote in symbol table: " << q.lastError().text();

    if (dt < firstDate)
    {
      firstDate = dt;    
      flag = TRUE;
    }

    if (dt > lastDate)
    {
      lastDate = dt;    
      flag = TRUE;
    }
  }

  ts = "COMMIT";
  q.exec(ts);
  if (q.lastError().isValid())
  {
    qDebug() << "CSV::setChart:commit: " << q.lastError().text();
    return;
  }

  // update the start and end dates
  if (flag)
  {
    // update the new start and end dates in symbolIndex
    QString sd,ed;
    sd = firstDate.toString("yyyyMMddHHmmsszzz");
    ed = lastDate.toString("yyyyMMddHHmmsszzz");

    // update the start and end dates in the symbolIndex
    ts = "INSERT OR REPLACE INTO symbolIndex (symbol,firstDate,lastDate) VALUES('" + symbol + "'," + sd + "," + ed + ")";
    q.exec(ts);
    if (q.lastError().isValid())
      qDebug() << "CSV::setChart:update first and last dates in symbol index table: " << q.lastError().text();
  }
}

