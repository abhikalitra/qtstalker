/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#include "CBOT.h"
#include "ChartDb.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qnetwork.h>
#include <qdir.h>
#include <qdatetime.h>
#include <qstringlist.h>

CBOT::CBOT ()
{
  pluginName = "CBOT";
  version = 0.2;
  fd = new FuturesData;
  createFlag = FALSE;
  op = 0;

  QDateTime dt = QDateTime::currentDateTime();
  set("Date", dt.toString("yyyyMMdd"), Setting::Date);

  about = "Downloads daily settlement quotes from CBOT\n";
  about.append("and imports it directly into qtstalker.");
  
  qInitNetworkProtocols();
}

CBOT::~CBOT ()
{
  delete fd;
}

void CBOT::update ()
{
  data.truncate(0);
  op = 0;

  QDir dir = QDir::home();
  file = dir.path();
  file.append("/Qtstalker/download");

  QString s  = "http://www.cbot.com/cbot/shared/charts/bot";

  QDateTime sdate = QDateTime::fromString(getDateTime("Date"), Qt::ISODate);
  
  QDateTime dt = QDateTime::currentDateTime();
  if (sdate > dt)
  {
    emit done();
    return;
  }

  if (sdate.date().dayOfWeek() == 6 || sdate.date().dayOfWeek() == 7)
  {
    emit done();
    return;
  }

  if (sdate.date().month() < 10)
    s.append("0");
  s.append(QString::number(sdate.date().month()));
  if (sdate.date().day() < 10)
    s.append("0");
  s.append(QString::number(sdate.date().day()));
  QString y = QString::number(sdate.date().year());
  y.remove(0, 2);
  if (y.toInt() < 10)
    y.remove(0, 1);
  s.append(y);
  s.append(".txt");

  if (op)
    delete op;
  op = new QUrlOperator(s);
  connect(op, SIGNAL(finished(QNetworkOperation *)), this, SLOT(opDone(QNetworkOperation *)));
  connect(op, SIGNAL(data(const QByteArray &, QNetworkOperation *)), this, SLOT(dataReady(const QByteArray &, QNetworkOperation *)));
  op->get();
  
  emit message(tr("Downloading CBOT data"));
}

void CBOT::dataReady (const QByteArray &d, QNetworkOperation *)
{
  int loop;
  for (loop = 0; loop < (int) d.size(); loop++)
    data.append(d[loop]);
}

QString CBOT::translateFraction (QString d)
{
  QString f = d.right(1);
  QString s = d;
  s.truncate(s.length() - 1);

  if (f.toInt() == 2)
    s.append(".25");
  else
  {
    if (f.toInt() == 4)
      s.append(".5");
    else
    {
      if (f.toInt() == 6)
        s.append(".75");
    }
  }

  return s;
}

QString CBOT::translateFraction2 (QString d)
{
  QString f = d.right(2);
  QString s = d;
  s.truncate(s.length() - 2);
  QString s2 = QString::number(f.toFloat() / 32);
  s2.remove(0, 1);
  s.append(s2);
  return s;
}

QString CBOT::translateFraction3 (QString d)
{
  QString f = d.right(3);
  QString s = d;
  s.truncate(s.length() - 3);
  QString s2 = QString::number((f.toFloat() / 32) / 10);
  s2.remove(0, 1);
  s.append(s2);
  return s;
}

void CBOT::opDone (QNetworkOperation *o)
{
  if (! o)
    return;

  if (o->state() == QNetworkProtocol::StDone && o->operation() == QNetworkProtocol::OpGet)
  {
    parse();
    emit done();
    delete op;
    return;
  }
  
  if (o->state() == QNetworkProtocol::StFailed)
  {
    emit message("Download error");
    emit done();
    delete op;
    return;
  }
}
  
void CBOT::parse ()
{
  QFile f(file);
  if (! f.open(IO_WriteOnly))
    return;
  QTextStream stream(&f);
  stream << data;
  f.close();

  f.setName(file);
  if (! f.open(IO_ReadOnly))
    return;
  stream.setDevice(&f);

  while(stream.atEnd() == 0)
  {
    QString s = stream.readLine();
    s = stripJunk(s);

    QStringList l = QStringList::split(" ", s, FALSE);
    if (l.count() != 9)
      continue;

    // symbol
    QString symbol = l[1];
    symbol.remove(0, 1);
    symbol.prepend(l[0]);
    symbol.append(l[1]);
    symbol.remove(symbol.length() - 4, 4);

    // futures month
    QString month = l[1];
    month.truncate(1);

    if (fd->setSymbol(l[0]))
      continue;

    // date
    QStringList l2 = QStringList::split("/", l[2], FALSE);
    if (l2.count() != 3)
      continue;
    QString date = l2[2];
    date.append(l2[0]);
    date.append(l2[1]);
    date.append("000000");

    bool flag = FALSE;
    QString open;
    QString high;
    QString low;
    QString close;
    QString volume;
    QString oi;
    while (1)
    {
      if (! l[0].compare("BO"))
      {
        if (setTFloat(l[3]))
        {
          flag = TRUE;
	  break;
	}
        else
          open = QString::number(tfloat / 100);

        if (setTFloat(l[4]))
	{
          flag = TRUE;
	  break;
	}
        else
          high = QString::number(tfloat / 100);

        if (setTFloat(l[5]))
	{
          flag = TRUE;
	  break;
	}
        else
          low = QString::number(tfloat / 100);

        if (setTFloat(l[6]))
          flag = TRUE;
        else
          close = QString::number(tfloat / 100);

	break;
      }

      if (! l[0].compare("C") || ! l[0].compare("O") || ! l[0].compare("S") || ! l[0].compare("W"))
      {
        if (setTFloat(translateFraction(l[3])))
	{
          flag = TRUE;
	  break;
	}
        else
          open = QString::number(tfloat);

        if (setTFloat(translateFraction(l[4])))
	{
          flag = TRUE;
	  break;
	}
        else
          high = QString::number(tfloat);

        if (setTFloat(translateFraction(l[5])))
	{
          flag = TRUE;
	  break;
	}
        else
          low = QString::number(tfloat);

        if (setTFloat(translateFraction(l[6])))
          flag = TRUE;
        else
          close = QString::number(tfloat);

	break;
      }

      if (! l[0].compare("DJ"))
      {
        if (setTFloat(l[3]))
	{
          flag = TRUE;
	  break;
	}
        else
          open = QString::number(tfloat);

        if (setTFloat(l[4]))
	{
          flag = TRUE;
	  break;
	}
        else
          high = QString::number(tfloat);

        if (setTFloat(l[5]))
	{
          flag = TRUE;
	  break;
	}
        else
          low = QString::number(tfloat);

        if (setTFloat(l[6]))
          flag = TRUE;
        else
          close = QString::number(tfloat);

	break;
      }

      if (! l[0].compare("SM"))
      {
        if (setTFloat(l[3]))
	{
          flag = TRUE;
	  break;
	}
        else
          open = QString::number(tfloat / 10);

        if (setTFloat(l[4]))
	{
          flag = TRUE;
	  break;
	}
        else
          high = QString::number(tfloat / 10);

        if (setTFloat(l[5]))
	{
          flag = TRUE;
	  break;
	}
        else
          low = QString::number(tfloat / 10);

        if (setTFloat(l[6]))
          flag = TRUE;
        else
          close = QString::number(tfloat / 10);

	break;
      }

      if (! l[0].compare("TY"))
      {
        if (setTFloat(translateFraction3(l[3])))
	{
          flag = TRUE;
	  break;
	}
        else
          open = QString::number(tfloat);

        if (setTFloat(translateFraction3(l[4])))
	{
          flag = TRUE;
	  break;
	}
        else
          high = QString::number(tfloat);

        if (setTFloat(translateFraction3(l[5])))
	{
          flag = TRUE;
	  break;
	}
        else
          low = QString::number(tfloat);

        if (setTFloat(translateFraction3(l[6])))
          flag = TRUE;
        else
          close = QString::number(tfloat);

	break;
      }

      if (! l[0].compare("US"))
      {
        if (setTFloat(translateFraction2(l[3])))
	{
          flag = TRUE;
	  break;
	}
        else
          open = QString::number(tfloat);

        if (setTFloat(translateFraction2(l[4])))
	{
          flag = TRUE;
	  break;
	}
        else
          high = QString::number(tfloat);

        if (setTFloat(translateFraction2(l[5])))
	{
          flag = TRUE;
	  break;
	}
        else
          low = QString::number(tfloat);

        if (setTFloat(translateFraction2(l[6])))
          flag = TRUE;
        else
          close = QString::number(tfloat);

	break;
      }

      break;
    }

    if (flag)
      continue;

    // check for bad values
    if (open.toFloat() == 0)
      open = close;
    if (high.toFloat() == 0)
      high = close;
    if (low.toFloat() == 0)
      low = close;

    // volume
    if (setTFloat(l[7]))
      continue;
    else
    {
      if (tfloat == 0)
        continue;
      else
        volume = QString::number(tfloat);
    }

    // oi
    if (setTFloat(l[8]))
      continue;
    else
      oi = QString::number(tfloat);
      
    s = dataPath;
    s.append("/Futures");
    QDir dir(s);
    if (! dir.exists(s, TRUE))
    {
      if (! dir.mkdir(s, TRUE))
      {
        qDebug("CBOT plugin: Unable to create futures directory");
        return;
      }
    }

    s.append("/");
    s.append(fd->getSymbol());
    if (! dir.exists(s, TRUE))
    {
      if (! dir.mkdir(s, TRUE))
      {
        qDebug("CBOT plugin: Unable to create symbol directory");
        return;
      }
    }

    Setting *r = new Setting;
    r->set("Date", date, Setting::Date);
    r->set("Open", open, Setting::Float);
    r->set("High", high, Setting::Float);
    r->set("Low", low, Setting::Float);
    r->set("Close", close, Setting::Float);
    r->set("Volume", volume, Setting::Float);
    r->set("Open Interest", oi, Setting::Float);

    s = dataPath;
    s.append("/Futures/");
    s.append(fd->getSymbol());
    s.append("/");
    s.append(symbol);
    ChartDb *db = new ChartDb();
    db->openChart(s);

    Setting *details = db->getDetails();
    if (! details->count())
    {
      details->set("Format", "Open|High|Low|Close|Volume|Open Interest", Setting::None);
      details->set("Chart Type", tr("Futures"), Setting::None);
      details->set("Symbol", symbol, Setting::None);
      details->set("Source", pluginName, Setting::None);
      details->set("Futures Month", month, Setting::None);
      details->set("Futures Type", fd->getSymbol(), Setting::None);
      details->set("Title", fd->getName(), Setting::Text);
    }

    db->setRecord(r);
    delete db;
    delete r;
  }

  f.close();

  emit done();
}

void CBOT::cancelUpdate ()
{
  if (op)
    op->stop();
  emit done();
}

Plugin * create ()
{
  CBOT *o = new CBOT;
  return ((Plugin *) o);
}

