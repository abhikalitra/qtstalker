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

#include "Yahoo.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qdir.h>
#include <qnetwork.h>

Yahoo::Yahoo ()
{
  pluginName = "Yahoo";
  version = 0.2;
  symbolLoop = 0;
  data.truncate(0);
  op = 0;

  QDateTime dt = QDateTime::currentDateTime();
  set("Start Date", dt.toString("yyyyMMdd"), Setting::Date);
  set("New Symbols", "", Setting::Text);

  about = "Downloads Yahoo data\n";
  about.append("and imports it directly into qtstalker.\n");
}

Yahoo::~Yahoo ()
{
  if (op)
    delete op;
}

void Yahoo::download ()
{
  QDir dir = QDir::home();
  file = dir.path();
  file.append("/Qtstalker/download");

  QString s = getData("Start Date");
  s.insert(4, "-");
  s.insert(7, "-");
  s.append("00:00:00");
  sdate = QDateTime::fromString(s, Qt::ISODate);

  edate = QDateTime::currentDateTime();
  if (edate.date().dayOfWeek() == 6)
    edate = edate.addDays(-1);
  if (edate.date().dayOfWeek() == 7)
    edate = edate.addDays(-2);

  symbols = QStringList::split(",", getData("New Symbols"), FALSE);
  if (symbols.count() == 0)
  {
    ChartDb *db = new ChartDb();
    db->setPath(indexPath);
    db->openChart();
    QStringList l = db->getKeyList();

    int loop;
    for (loop = 0; loop < (int) l.count(); loop++)
    {
      Setting *details = new Setting;
      details->parse(db->getData(l[loop]));
      s = details->getData("Source");
      if (! s.compare(pluginName))
        symbols.append(l[loop]);
      delete details;
    }
  }

  symbolLoop = 0;

  data.truncate(0);

  qInitNetworkProtocols();

  QTimer::singleShot(1000, this, SLOT(getFile()));
}

void Yahoo::opDone (QNetworkOperation *o)
{
  if (o->state() == QNetworkProtocol::StDone && o->operation() == QNetworkProtocol::OpGet)
  {
    parse();

    symbolLoop++;
    if (symbolLoop == (int) symbols.count())
    {
      emit done();
      delete op;
      return;
    }

    getFile();
  }
}

void Yahoo::getFile ()
{
  data.truncate(0);

  QString s = "http://chart.yahoo.com/table.csv?s=";
  s.append(symbols[symbolLoop]);
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

  if (op)
    delete op;

  op = new QUrlOperator(s);
  connect(op, SIGNAL(finished(QNetworkOperation *)), this, SLOT(opDone(QNetworkOperation *)));
  connect(op, SIGNAL(data(const QByteArray &, QNetworkOperation *)), this, SLOT(dataReady(const QByteArray &, QNetworkOperation *)));
  op->get();
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
  s.append("/");
  s.append(symbols[symbolLoop]);
  ChartDb *db = new ChartDb();
  db->setPath(s);
  db->openChart();
  
  Setting *details = db->getDetails();
  if (! details->count())
  {
    details->set("Chart Type", tr("Stock"), Setting::None);
    details->set("Symbol", symbols[symbolLoop], Setting::None);
    details->set("Source", pluginName, Setting::None);
    details->set("Title", symbols[symbolLoop], Setting::Text);
    db->setDetails(details);
  }
  delete details;

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
    r->set("Open Interest", "0", Setting::Float);
    db->setRecord(r);
    delete r;
  }

  f.close();
  delete db;
  updateChartIndex(symbols[symbolLoop]);
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

Plugin * create ()
{
  Yahoo *o = new Yahoo;
  return ((Plugin *) o);
}


