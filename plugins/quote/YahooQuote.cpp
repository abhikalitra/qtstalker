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

#include "YahooQuote.h"
#include "ChartDb.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qdir.h>
#include <qnetwork.h>

YahooQuote::YahooQuote ()
{
  pluginName = "YahooQuote";
  version = 0.2;
  createFlag = TRUE;
  op = 0;

  about = "Downloads YahooQuote data\n";
  about.append("and imports it directly into qtstalker.\n");
}

YahooQuote::~YahooQuote ()
{
  if (op)
    delete op;
}

void YahooQuote::update ()
{
  symbolList.clear();
  urlList.clear();
  data.truncate(0);
  symbolLoop = 0;
  op = 0;

  QDir dir = QDir::home();
  file = dir.path();
  file.append("/Qtstalker/download");

  QString s = dataPath;
  s.append("/Stocks");
  dir.setPath(s);

  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    s = "http://finance.yahoo.com/d/quotes.csv?s=";
    s.append(dir[loop]);
    s.append("&f=snl1d1t1c1ohgv&e=.csv");

    symbolList.append(dir[loop]);
    urlList.append(s);
  }

  if (! symbolList.count())
  {
    emit done();
    return;
  }

  qInitNetworkProtocols();

  QTimer::singleShot(250, this, SLOT(getFile()));
}

void YahooQuote::opDone (QNetworkOperation *o)
{
  if (o->state() == QNetworkProtocol::StDone && o->operation() == QNetworkProtocol::OpGet)
  {
    parse();

    symbolLoop++;

    if (symbolLoop == (int) symbolList.count())
    {
      emit done();
      delete op;
      return;
    }

    data.truncate(0);

    getFile();
  }
}

void YahooQuote::getFile ()
{
  if (op)
    delete op;

  op = new QUrlOperator(urlList[symbolLoop]);
  connect(op, SIGNAL(finished(QNetworkOperation *)), this, SLOT(opDone(QNetworkOperation *)));
  connect(op, SIGNAL(data(const QByteArray &, QNetworkOperation *)), this, SLOT(dataReady(const QByteArray &, QNetworkOperation *)));
  op->get();
}

void YahooQuote::dataReady (const QByteArray &d, QNetworkOperation *)
{
  int loop;
  for (loop = 0; loop < (int) d.size(); loop++)
    data.append(d[loop]);
}

void YahooQuote::parse ()
{
  if (! data.length())
    return;

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
  
  QString s = dataPath;
  s.append("/Stocks/");
  s.append(symbolList[symbolLoop]);
  ChartDb *db = new ChartDb();
  db->openChart(s);

  while(stream.atEnd() == 0)
  {
    QString s = stream.readLine();
    s = stripJunk(s);

    QStringList l = QStringList::split(",", s, FALSE);
    if (l.count() < 9 || l.count() > 10)
      continue;

    // get date
    QStringList l2 = QStringList::split("/", l[3], FALSE);
    if (l2.count() != 3)
      continue;
    QString date = l2[2];
    if (l2[0].toInt() < 10)
      date.append("0");
    date.append(l2[0]);
    if (l2[1].toInt() < 10)
      date.append("0");
    date.append(l2[1]);
    date.append("000000");

    // open
    QString open;
    if (setTFloat(l[6]))
      continue;
    else
      open = QString::number(tfloat);

    // high
    QString high;
    if (setTFloat(l[7]))
      continue;
    else
      high = QString::number(tfloat);

    // low
    QString low;
    if (setTFloat(l[8]))
      continue;
    else
      low = QString::number(tfloat);

    // close
    QString close;
    if (setTFloat(l[2]))
      continue;
    else
      close = QString::number(tfloat);

    // volume
    QString volume = "0";
    if (l.count() == 10)
      volume = l[9];

    Setting *r = new Setting;
    r->set("Date", date, Setting::Date);
    r->set("Open", open, Setting::Float);
    r->set("High", high, Setting::Float);
    r->set("Low", low, Setting::Float);
    r->set("Close", close, Setting::Float);
    r->set("Volume", volume, Setting::Float);
    db->setRecord(r);
    delete r;
  }

  f.close();
  delete db;
}

QString YahooQuote::parseDate (QString d)
{
  QString s;

  QStringList l = QStringList::split("-", d, FALSE);
  if (l.count() != 3)
    return s;

  s = l[2];
  if (s.toInt() < 50)
    s.prepend("20");
  else
    s.prepend("19");

  while (1)
  {
    if (! l[1].compare("Jan"))
    {
      s.append("01");
      break;
    }

    if (! l[1].compare("Feb"))
    {
      s.append("02");
      break;
    }

    if (! l[1].compare("Mar"))
    {
      s.append("03");
      break;
    }

    if (! l[1].compare("Apr"))
    {
      s.append("04");
      break;
    }

    if (! l[1].compare("May"))
    {
      s.append("05");
      break;
    }

    if (! l[1].compare("Jun"))
    {
      s.append("06");
      break;
    }

    if (! l[1].compare("Jul"))
    {
      s.append("07");
      break;
    }

    if (! l[1].compare("Aug"))
    {
      s.append("08");
      break;
    }

    if (! l[1].compare("Sep"))
    {
      s.append("09");
      break;
    }

    if (! l[1].compare("Oct"))
    {
      s.append("10");
      break;
    }

    if (! l[1].compare("Nov"))
    {
      s.append("11");
      break;
    }

    if (! l[1].compare("Dec"))
    {
      s.append("12");
      break;
    }

    return s;
  }

  if (l[0].toInt() < 10)
    s.append("0");
  s.append(l[0]);
  s.append("000000");

  return s;
}

Setting * YahooQuote::getCreateDetails ()
{
  Setting *set = new Setting;
  set->set("New Symbols", "", Setting::Text);
  return set;
}

void YahooQuote::createChart (Setting *set)
{
  QStringList symbols = QStringList::split(" ", set->getData("New Symbols"), FALSE);

  QString base = dataPath;
  base.append("/Stocks");
  QDir dir(base);
  if (! dir.exists(base, TRUE))
  {
    if (! dir.mkdir(base, TRUE))
    {
      qDebug("Yahoo plugin: Unable to create directory");
      return;
    }
  }
  base.append("/");

  int loop;
  for (loop = 0; loop < (int) symbols.count(); loop++)
  {
    QString s = base;
    s.append(symbols[loop]);

    if (dir.exists(s, TRUE))
      continue;

    ChartDb *db = new ChartDb();
    db->openChart(s);

    Setting *details = db->getDetails();
    details->set("Format", "Open|High|Low|Close|Volume", Setting::None);
    details->set("Chart Type", tr("Stock"), Setting::None);
    details->set("Symbol", symbols[loop], Setting::None);
    details->set("Source", pluginName, Setting::None);
    details->set("Title", symbols[loop], Setting::Text);
    db->saveDetails();

    delete db;
  }
}

Plugin * create ()
{
  YahooQuote *o = new YahooQuote;
  return ((Plugin *) o);
}


