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
#include <qfile.h>
#include <qtextstream.h>
#include <qnetwork.h>
#include <qdir.h>
#include <qdatetime.h>
#include <qstringlist.h>

CBOT::CBOT ()
{
  connect(&op, SIGNAL(finished(QNetworkOperation *)), this, SLOT(parse(QNetworkOperation *)));

  pluginName = "CBOT";
  version = 0.2;

  QDateTime dt = QDateTime::currentDateTime();
  set("Date", dt.toString("yyyyMMdd"), Setting::Date);

  about = "Downloads daily settlement quotes from CBOT\n";
  about.append("and imports it directly into qtstalker.");
}

CBOT::~CBOT ()
{
}

void CBOT::download ()
{
  QDir dir = QDir::home();
  file = dir.path();
  file.append("/Qtstalker/download");

  qInitNetworkProtocols();

  QString s  = "http://www.cbot.com/cbot/shared/charts/bot";

  QString s2 = getData("Date");
  s2.insert(4, "-");
  s2.insert(7, "-");
  s2.append("00:00:00");
  QDateTime sdate = QDateTime::fromString(s2, Qt::ISODate);

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

  dir.remove(file, TRUE);

  op.copy(s, file, FALSE, FALSE);
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

void CBOT::parse (QNetworkOperation *o)
{
  if (o->state() != QNetworkProtocol::StDone)
    return;

  if (o->errorCode() != QNetworkProtocol::NoError)
  {
    QString s = o->protocolDetail();
    qDebug(s.latin1());
    emit done();
    return;
  }

  QFile f(file);
  if (! f.open(IO_ReadOnly))
    return;
  QTextStream stream(&f);

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

    if (setFutures(l[0]))
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

    Setting *r = new Setting;
    r->set("Date", date, Setting::Date);
    r->set("Open", open, Setting::Float);
    r->set("High", high, Setting::Float);
    r->set("Low", low, Setting::Float);
    r->set("Close", close, Setting::Float);
    r->set("Volume", volume, Setting::Float);
    r->set("Open Interest", oi, Setting::Float);

    s = dataPath;
    s.append("/");
    s.append(symbol);
    ChartDb *db = new ChartDb();
    db->setPath(s);
    db->openChart();
    
    Setting *details = db->getDetails();
    if (! details->count())
    {
      details->set("Chart Type", tr("Futures"), Setting::None);
      details->set("Symbol", symbol, Setting::None);
      details->set("Source", pluginName, Setting::None);
      details->set("Futures Month", month, Setting::None);
      details->set("Futures Type", futureSymbol, Setting::None);
      details->set("Title", futureName, Setting::Text);
      db->setDetails(details);
    }
    delete details;

    db->setRecord(r);
    delete db;

    if (! symbol.compare(cc))
    {
      symbol = "CC";
      symbol.append(futureSymbol);

      s = dataPath;
      s.append("/");
      s.append(symbol);
      ChartDb *db = new ChartDb();
      db->setPath(s);
      db->openChart();

      details = db->getDetails();
      if (! details->count())
      {
        details->set("Chart Type", tr("CC Futures"), Setting::None);
        details->set("Symbol", symbol, Setting::None);
        details->set("Source", pluginName, Setting::None);
        details->set("Futures Type", futureSymbol, Setting::None);
        s = QObject::tr("Continuous ");
        s.append(futureSymbol);
        details->set("Title", s, Setting::Text);
        db->setDetails(details);
      }
      delete details;

      db->setRecord(r);
      delete db;
    }

    delete r;
  }

  f.close();

  emit done();
}

void CBOT::cancelDownload ()
{
  op.stop();
  emit done();
}

Plugin * create ()
{
  CBOT *o = new CBOT;
  return ((Plugin *) o);
}

