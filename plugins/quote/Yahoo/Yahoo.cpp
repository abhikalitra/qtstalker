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

#include "Yahoo.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qdir.h>
#include <qdatetime.h>
#include <qnetwork.h>
#include <qtimer.h>

Yahoo::Yahoo ()
{
  pluginName = "Yahoo";
  createFlag = TRUE;
  op = 0;
  
  QDate date = QDate::currentDate();
  if (date.dayOfWeek() == 6)
    date = date.addDays(-1);
  else
  {
    if (date.dayOfWeek() == 7)
      date = date.addDays(-2);
  }
  set("End Date", date.toString("yyyyMMdd"), Setting::Date);

  date = date.addDays(-1);
  if (date.dayOfWeek() == 6)
    date = date.addDays(-1);
  else
  {
    if (date.dayOfWeek() == 7)
      date = date.addDays(-2);
  }
  set("Start Date", date.toString("yyyyMMdd"), Setting::Date);

  about = "Downloads Yahoo data\n";
  about.append("and imports it directly into qtstalker.\n");
  
  qInitNetworkProtocols();
}

Yahoo::~Yahoo ()
{
}

void Yahoo::update ()
{
  symbolList.clear();
  urlList.clear();
  data.truncate(0);
  symbolLoop = 0;
  op = 0;

  QDir dir = QDir::home();
  file = dir.path();
  file.append("/Qtstalker/download");

  QDateTime edate = QDateTime::fromString(getDateTime("End Date"), Qt::ISODate);

  QString s = dataPath;
  s.append("/Stocks");
  dir.setPath(s);

  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    s = dataPath;
    s.append("/Stocks/");
    s.append(dir[loop]);

    ChartDb *db = new ChartDb();
    db->openChart(s);
    Setting *details = db->getDetails();

    QDateTime sdate;
    s = details->getDateTime("Last Date");
    if (s.length())
      sdate = QDateTime::fromString(getDateTime("Start Date"), Qt::ISODate);
    else
      sdate = QDateTime::fromString("1990-01-0100:00:00", Qt::ISODate);

    s = "http://chart.yahoo.com/table.csv?s=";
    s.append(dir[loop]);
    s.append("&a=");
    s.append(QString::number(sdate.date().month() - 1));
    s.append("&b=");
    s.append(sdate.toString("dd"));
    s.append("&c=");
    s.append(sdate.toString("yy"));
    s.append("&d=");
    s.append(QString::number(edate.date().month() - 1));
    s.append("&e=");
    s.append(edate.toString("dd"));
    s.append("&f=");
    s.append(edate.toString("yy"));
    s.append("&g=d&q=q&y=0&x=.csv");
    
    symbolList.append(dir[loop]);
    urlList.append(s);

    delete db;
  }

  if (! symbolList.count())
  {
    emit done();
    return;
  }

  QTimer::singleShot(250, this, SLOT(getFile()));
}

void Yahoo::opDone (QNetworkOperation *o)
{
  if (! o)
    return;

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

void Yahoo::getFile ()
{
  if (op)
    delete op;

  op = new QUrlOperator(urlList[symbolLoop]);
  connect(op, SIGNAL(finished(QNetworkOperation *)), this, SLOT(opDone(QNetworkOperation *)));
  connect(op, SIGNAL(data(const QByteArray &, QNetworkOperation *)), this, SLOT(dataReady(const QByteArray &, QNetworkOperation *)));
  op->get();
  
  QString s = tr("Downloading ");
  s.append(symbolList[symbolLoop]);
  emit message(s);
}

void Yahoo::dataReady (const QByteArray &d, QNetworkOperation *)
{
  int loop;
  for (loop = 0; loop < (int) d.size(); loop++)
    data.append(d[loop]);
}

void Yahoo::parse ()
{
  if (! data.length())
    return;

  if (data.contains("No data available"))
    return;

  if (data.contains("No Prices in this date range"))
    return;

  // strip off the header
  QString s = "Date,Open,High,Low,Close,Volume";
  int p = data.find(s, 0, TRUE);
  if (p != -1)
    data.remove(0, p + s.length());
  else
  {
    s = "Date,Open,High,Low,Close";
    int p = data.find(s, 0, TRUE);
    if (p != -1)
      data.remove(0, p + s.length());
  }

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

  s = dataPath;
  s.append("/Stocks/");
  s.append(symbolList[symbolLoop]);
  ChartDb *db = new ChartDb();
  db->openChart(s);

  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    s = stripJunk(s);

    QStringList l = QStringList::split(",", s, FALSE);
    if (l.count() < 5 || l.count() > 6)
      continue;

    // date
    QString date = parseDate(l[0]);
    if (date.length() != 14)
      continue;

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
    QString volume = "0";
    if (l.count() == 6)
      volume = l[5];

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

QString Yahoo::parseDate (QString d)
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

Setting * Yahoo::getCreateDetails ()
{
  Setting *set = new Setting;
  set->set("New Symbols", "", Setting::Text);
  return set;
}

void Yahoo::createChart (Setting *set)
{
  QStringList symbols = QStringList::split(" ", set->getData("New Symbols"), FALSE);

  QString path = createDirectory("Stocks");
  if (! path.length())
  {
    qDebug("Yahoo plugin: Unable to create directory");
    return;
  }
  path.append("/");

  int loop;
  for (loop = 0; loop < (int) symbols.count(); loop++)
  {
    QString s = path;
    s.append(symbols[loop]);
    QDir dir(s);
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
  Yahoo *o = new Yahoo;
  return ((Plugin *) o);
}


