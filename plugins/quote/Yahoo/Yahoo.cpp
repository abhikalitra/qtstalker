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

#include "Yahoo.h"
#include "YahooDialog.h"
#include "ChartDb.h"
#include "Config.h"
#include "Bar.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qdir.h>
#include <qdatetime.h>
#include <qnetwork.h>
#include <qtimer.h>
#include <qsettings.h>

Yahoo::Yahoo ()
{
  pluginName = "Yahoo";
  op = 0;
  helpFile = "yahoo.html";
  
  sdate = QDateTime::currentDateTime();
  if (sdate.date().dayOfWeek() == 6)
    sdate = sdate.addDays(-1);
  else
  {
    if (sdate.date().dayOfWeek() == 7)
      sdate = sdate.addDays(-2);
  }
  sdate = sdate.addDays(-1);

  edate = QDateTime::currentDateTime();
  if (edate.date().dayOfWeek() == 6)
    edate = edate.addDays(-1);
  else
  {
    if (edate.date().dayOfWeek() == 7)
      edate = edate.addDays(-2);
  }
    
  qInitNetworkProtocols();
  
  loadSettings();
}

Yahoo::~Yahoo ()
{
}

void Yahoo::update ()
{
  urlList.clear();
  data.truncate(0);
  symbolLoop = 0;
  op = 0;

  QDir dir = QDir::home();
  file = dir.path();
  file.append("/Qtstalker/download");

  int loop;
  for (loop = 0; loop < (int) symbolList.count(); loop++)
  {
    QString s;
    
    if (! method.compare(tr("History")))
    {
      s = "http://ichart.yahoo.com/table.csv?s=";
      s.append(symbolList[loop]);
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
    }
    else
    {
      s = "http://finance.yahoo.com/d/quotes.csv?s=";
      s.append(symbolList[loop]);
      s.append("&f=snl1d1t1c1ohgv&e=.csv");
    }
    
    urlList.append(s);
  }

  if (! symbolList.count())
  {
    emit done();
    emit statusLogMessage(tr("No symbols selected. Done."));
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
    if (! method.compare(tr("History")))
      parseHistory();
    else
      parseQuote();
      
    symbolLoop++;

    if (symbolLoop == (int) symbolList.count())
    {
      emit done();
      emit statusLogMessage(tr("Done"));
      delete op;
      return;
    }

    data.truncate(0);
    getFile();
    return;
  }

  if (o->state() == QNetworkProtocol::StFailed)
  {
    if (symbolLoop + 1 >= (int) symbolList.count())
    {
      emit done();
      emit statusLogMessage(tr("Done"));
      delete op;
    }
    else
    {
      emit statusLogMessage(tr("Download error ") + symbolList[symbolLoop] + tr(" skipped"));
      symbolLoop++;
      data.truncate(0);
      getFile();
    }
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
  emit statusLogMessage(s);
}

void Yahoo::dataReady (const QByteArray &d, QNetworkOperation *)
{
  int loop;
  for (loop = 0; loop < (int) d.size(); loop++)
    data.append(d[loop]);
}

void Yahoo::parseHistory ()
{
  if (! data.length())
    return;

  if (data.contains("No data available"))
    return;

  if (data.contains("No Prices in this date range"))
    return;

  // strip off the header
  QString s = "Date,Open,High,Low,Close";
  int p = data.find(s, 0, TRUE);
  if (p != -1)
    data.remove(0, p + s.length());

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

  Config config;
  s = config.getData(Config::DataPath) + "/Stocks/" + symbolList[symbolLoop];
  
  ChartDb *plug = new ChartDb;
  plug->setPlugin("Stocks");
  if (plug->openChart(s))
  {
    emit statusLogMessage("Could not open db.");
    f.close();
    delete plug;
    return;
  }

  s = plug->getData("Symbol");
  if (! s.length())
  {
    Setting *set = new Setting;
    set->setData("BarType", QString::number(BarData::Daily));
    set->setData("Symbol", symbolList[symbolLoop]);
    set->setData("Title", symbolList[symbolLoop]);
    plug->saveDbDefaults(set);
    delete set;
  }

  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    s = stripJunk(s);

    QStringList l = QStringList::split(",", s, FALSE);
    if (l.count() < 5)
      continue;

    // date
    QString date = parseDate(l[0]);

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
    if (l.count() >= 6)
      volume = l[5];

    // adjusted close
    if (adjustment)
    {
      QString adjclose = "0";
      if (l.count() >= 7)
      {
	if (setTFloat(l[6]))
	  continue;
	else
	  adjclose = QString::number(tfloat);
	// apply yahoo's adjustments through all the data, not just closing price
	// i.e. adjust for stock splits and dividends
	float factor = close.toFloat() / adjclose.toFloat();
	if (factor != 1)
	{
	  high   = QString::number(high.toFloat()   / factor);
	  low    = QString::number(low.toFloat()    / factor);
	  open   = QString::number(open.toFloat()   / factor);
	  close  = QString::number(close.toFloat()  / factor);
	  volume = QString::number(volume.toFloat() * factor);
	}
      }
    }

    Bar *bar = new Bar;
    if (bar->setDate(date))
    {
      emit statusLogMessage("Bad date " + date);
      delete bar;
      continue;
    }
    bar->setOpen(open.toDouble());
    bar->setHigh(high.toDouble());
    bar->setLow(low.toDouble());
    bar->setClose(close.toDouble());
    bar->setVolume(volume.toDouble());
    plug->setBar(bar);
    delete bar;
    
    s = symbolList[symbolLoop] + " " + date + " " + open + " " + high + " " + low
        + " " + close + " " + volume;
	
    emit dataLogMessage(s);
  }

  f.close();
  delete plug;
}

void Yahoo::parseQuote ()
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

  Config config;  
  QString s = config.getData(Config::DataPath) + "/Stocks/" + symbolList[symbolLoop];
  
  ChartDb *plug = new ChartDb;
  plug->setPlugin("Stocks");
  if (plug->openChart(s))
  {
    emit statusLogMessage("Could not open db.");
    f.close();
    delete plug;
    return;
  }
  
  s = plug->getData("Symbol");
  if (! s.length())
  {
    Setting *set = new Setting;
    set->setData("BarType", QString::number(BarData::Daily));
    set->setData("Symbol", symbolList[symbolLoop]);
    set->setData("Title", symbolList[symbolLoop]);
    plug->saveDbDefaults(set);
    delete set;
  }
  
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
      
    Bar *bar = new Bar;
    if (bar->setDate(date))
    {
      emit statusLogMessage("Bad date " + date);
      delete bar;
      continue;
    }
    bar->setOpen(open.toDouble());
    bar->setHigh(high.toDouble());
    bar->setLow(low.toDouble());
    bar->setClose(close.toDouble());
    bar->setVolume(volume.toDouble());
    plug->setBar(bar);
    delete bar;
    
    s = symbolList[symbolLoop] + " " + date + " " + open + " " + high + " " + low
        + " " + close + " " + volume;
	
    emit dataLogMessage(s);
  }

  f.close();
  delete plug;
}

QString Yahoo::parseDate (QString d)
{
  QString s;

  QStringList l = QStringList::split("-", d, FALSE);
  if (l.count() != 3)
    return s;

  s = l[2];
  if (s.toInt() > 29)
    s.prepend("19");
  else
    s.prepend("20");
    
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

void Yahoo::prefDialog (QWidget *w)
{
  YahooDialog *dialog = new YahooDialog(w, helpFile);
  dialog->setCaption(tr("Yahoo Prefs"));
  dialog->setAdjustment(adjustment);
  dialog->setStartDate(sdate);
  dialog->setEndDate(edate);
  dialog->setMethod(method);
  
  int rc = dialog->exec();
  if (rc == QDialog::Accepted)
  {
    adjustment = dialog->getAdjustment();
    sdate = dialog->getStartDate();
    edate = dialog->getEndDate();
    symbolList = dialog->getList();
    method = dialog->getMethod();
    saveFlag = TRUE;
  }
  
  delete dialog;
}

void Yahoo::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/Yahoo plugin");

  QString s = settings.readEntry("/Adjustment", "0");
  adjustment = s.toInt();
  
  method = settings.readEntry("/Method", tr("History"));
  
  settings.endGroup();
}

void Yahoo::saveSettings ()
{
  if (! saveFlag)
    return;
    
  QSettings settings;
  settings.beginGroup("/Qtstalker/Yahoo plugin");
  
  settings.writeEntry("/Adjustment", QString::number(adjustment));
  settings.writeEntry("/Method", method);
  
  settings.endGroup();
}

QuotePlugin * createQuotePlugin ()
{
  Yahoo *o = new Yahoo;
  return ((QuotePlugin *) o);
}


