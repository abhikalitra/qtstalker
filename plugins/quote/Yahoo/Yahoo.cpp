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
#include "DbPlugin.h"
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
  errorLoop = 0;
  allSymbols = FALSE;
  url.setAutoDelete(TRUE);
  currentUrl = 0;
  
  Config config;
  dataPath = config.getData(Config::DataPath) + "/Stocks/Yahoo";
  
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(downloadError()));
  
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
  
  // preload all symbols to download for default
//  loadAllSymbols();
}

Yahoo::~Yahoo ()
{
  if (op)
  {
    op->stop();
    delete op;
  }

  delete timer;
}

void Yahoo::update ()
{
  url.clear();
  data.truncate(0);
  Config config;
  errorLoop = 0;

  QDir dir = QDir::home();
  file = dir.path();
  file.append("/Qtstalker/download");
  
  int loop;
  for (loop = 0; loop < (int) symbolList.count(); loop++)
  {
    QString path = dataPath + "/";
    QFileInfo fi(symbolList[loop]);
    if (fi.extension(FALSE).length())
      path.append(fi.extension(FALSE).upper());
    else
      path.append("US");
    path.append("/");
    path.append(symbolList[loop]);
    if (! dir.exists(path, TRUE))
      continue;
      
    if (! method.compare(tr("History")))
      createHistoryUrls(symbolList[loop]);
    else
    {
      if (! method.compare(tr("Auto History")))
        createAutoHistoryUrls(path, symbolList[loop]);
      else
      {
        if (! method.compare(tr("Quote")))
          createQuoteUrls(symbolList[loop]);
	else
          createFundamentalUrls(symbolList[loop]);
      }
    }
  }

  if (! url.count())
  {
    emit done();
    emit statusLogMessage(tr("No symbols selected. Done."));
    return;
  }

  currentUrl = url.first();
  
  QTimer::singleShot(250, this, SLOT(getFile()));
}

void Yahoo::opDone (QNetworkOperation *o)
{
  if (! o)
    return;

  if (o->state() == QNetworkProtocol::StDone && o->operation() == QNetworkProtocol::OpGet)
  {
    timer->stop();
    
    if (method.contains(tr("History")))
      parseHistory();
    else
    {
      if (method.contains(tr("Quote")))
        parseQuote();
      else
        parseFundamental();
    }

    url.remove();
    if (! url.count())
    {
      emit done();
      emit statusLogMessage(tr("Done"));
      return;
    }
    else
      currentUrl = url.current();
    
    if (! currentUrl)
    {
      errorLoop++;
      
      if (errorLoop == retries)
      {
        emit done();
        emit statusLogMessage(tr("Done"));
	printErrorList();
        return;
      }
      else
	currentUrl = url.first();
    }

    data.truncate(0);
    getFile();
    return;
  }

  if (o->state() == QNetworkProtocol::StFailed)
    downloadError();
}

void Yahoo::getFile ()
{
  if (op)
  {
    op->stop();
    delete op;
  }

  timer->start(timeout * 1000, TRUE);
  
  op = new QUrlOperator(currentUrl->getData("url"));
  connect(op, SIGNAL(finished(QNetworkOperation *)), this, SLOT(opDone(QNetworkOperation *)));
  connect(op, SIGNAL(data(const QByteArray &, QNetworkOperation *)), this, SLOT(dataReady(const QByteArray &, QNetworkOperation *)));
  op->get();
  
  QString s = tr("Downloading ");
  s.append(currentUrl->getData("symbol"));
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
  s = dataPath + "/";
  QFileInfo fi(currentUrl->getData("symbol"));
  if (fi.extension(FALSE).length())
    s.append(fi.extension(FALSE).upper());
  else
    s.append("US");
  s.append("/");
  s.append(currentUrl->getData("symbol"));
  
  DbPlugin *plug = config.getDbPlugin("Stocks");
  if (! plug)
  {
    config.closePlugin("Stocks");
    f.close();
    return;
  }

  if (plug->openChart(s))
  {
    emit statusLogMessage("Could not open db.");
    f.close();
    config.closePlugin("Stocks");
    return;
  }

  // verify if this chart can be updated by this plugin
  s = plug->getHeaderField(DbPlugin::QuotePlugin);
  if (! s.length())
    plug->setHeaderField(DbPlugin::QuotePlugin, pluginName);
  else
  {
    if (s.compare(pluginName))
    {
      s = currentUrl->getData("symbol") + " - skipping update. Source does not match destination.";
      emit statusLogMessage(s);
      f.close();
      config.closePlugin("Stocks");
      return;
    }
  }
  
  s = plug->getHeaderField(DbPlugin::Symbol);
  if (! s.length())
  {
    plug->createNew();
    plug->setHeaderField(DbPlugin::Symbol, currentUrl->getData("symbol"));
    plug->setHeaderField(DbPlugin::Title, currentUrl->getData("symbol"));
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
    if (setTFloat(l[1], FALSE))
      continue;
    else
      open = QString::number(tfloat);

    // high
    QString high;
    if (setTFloat(l[2], FALSE))
      continue;
    else
      high = QString::number(tfloat);

    // low
    QString low;
    if (setTFloat(l[3], FALSE))
      continue;
    else
      low = QString::number(tfloat);

    // close
    QString close;
    if (setTFloat(l[4], FALSE))
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
	if (setTFloat(l[6], FALSE))
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
    
    s = currentUrl->getData("symbol") + " " + date + " " + open + " " + high + " " + low
        + " " + close + " " + volume;
	
    emit dataLogMessage(s);
  }

  f.close();
  config.closePlugin("Stocks");
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
  QString s = dataPath + "/";
  QFileInfo fi(currentUrl->getData("symbol"));
  if (fi.extension(FALSE).length())
    s.append(fi.extension(FALSE).upper());
  else
    s.append("US");
  s.append("/");
  s.append(currentUrl->getData("symbol"));
  
  DbPlugin *plug = config.getDbPlugin("Stocks");
  if (! plug)
  {
    config.closePlugin("Stocks");
    f.close();
    return;
  }
  
  if (plug->openChart(s))
  {
    emit statusLogMessage("Could not open db.");
    f.close();
    config.closePlugin("Stocks");
    return;
  }

  // verify if this chart can be updated by this plugin
  s = plug->getHeaderField(DbPlugin::QuotePlugin);
  if (! s.length())
    plug->setHeaderField(DbPlugin::QuotePlugin, pluginName);
  else
  {
    if (s.compare(pluginName))
    {
      s = currentUrl->getData("symbol") + " - skipping update. Source does not match destination.";
      emit statusLogMessage(s);
      f.close();
      config.closePlugin("Stocks");
      return;
    }
  }
    
  s = plug->getHeaderField(DbPlugin::Symbol);
  if (! s.length())
  {
    plug->createNew();
    plug->setHeaderField(DbPlugin::Symbol, currentUrl->getData("symbol"));
    plug->setHeaderField(DbPlugin::Title, currentUrl->getData("symbol"));
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
    if (setTFloat(l[6], FALSE))
      continue;
    else
      open = QString::number(tfloat);

    // high
    QString high;
    if (setTFloat(l[7], FALSE))
      continue;
    else
      high = QString::number(tfloat);

    // low
    QString low;
    if (setTFloat(l[8], FALSE))
      continue;
    else
      low = QString::number(tfloat);

    // close
    QString close;
    if (setTFloat(l[2], FALSE))
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
    
    s = currentUrl->getData("symbol") + " " + date + " " + open + " " + high + " " + low
        + " " + close + " " + volume;
	
    emit dataLogMessage(s);
  }

  f.close();
  config.closePlugin("Stocks");
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
  dialog->setRetries(retries);
  dialog->setTimeout(timeout);
  dialog->setAllSymbols(allSymbols);
  dialog->setList(fileList);
  
  int rc = dialog->exec();
  if (rc == QDialog::Accepted)
  {
    adjustment = dialog->getAdjustment();
    sdate = dialog->getStartDate();
    edate = dialog->getEndDate();
    method = dialog->getMethod();
    retries = dialog->getRetries();
    timeout = dialog->getTimeout();
    allSymbols = dialog->getAllSymbols();
    
    if (! allSymbols)
    {
      fileList = dialog->getList();
      int loop;
      symbolList.clear();
      for (loop = 0; loop < (int) fileList.count(); loop++)
      {
        QFileInfo fi(fileList[loop]);
        symbolList.append(fi.fileName());
      }
    }
    else
      loadAllSymbols();
    
    saveFlag = TRUE;
    saveSettings();
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
  
  s = settings.readEntry("/Retries", "3");
  retries = s.toInt();
  
  s = settings.readEntry("/Timeout", "15");
  timeout = s.toInt();
  
  s = settings.readEntry("/AllSymbols", "1");
  allSymbols = s.toInt();
  
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
  settings.writeEntry("/Retries", QString::number(retries));
  settings.writeEntry("/Timeout", QString::number(timeout));
  settings.writeEntry("/AllSymbols", QString::number(allSymbols));
  
  settings.endGroup();
}

void Yahoo::printErrorList ()
{
  for (url.first(); url.current() != 0; url.next())
  {
    QString s = tr("Unable to download ");
    s.append(url.current()->getData("symbol"));
    emit statusLogMessage(s);
  }
}	

void Yahoo::cancelUpdate ()
{
  if (op)
  {
    timer->stop();
    op->stop();
    emit done();
  }
}

void Yahoo::downloadError ()
{
  timer->stop();
   
  emit statusLogMessage(tr("Download error ") + currentUrl->getData("symbol") + tr(" skipped"));
    
  currentUrl = url.next();
  if (! url.count())
  {
    emit done();
    emit statusLogMessage(tr("Done"));
    printErrorList();
    return;
  }
    
  if (! currentUrl)
  {
    errorLoop++;
    
    if (errorLoop == retries)
    {
      emit done();
      emit statusLogMessage(tr("Done"));
      printErrorList();
      return;
    }
    else
      currentUrl = url.first();
  }
    
  data.truncate(0);
  getFile();
}

void Yahoo::parseFundamental ()
{
  if (! data.length())
    return;

  if (data.contains("data available"))
    return;
    
  QStringList l = QStringList::split("yfnc_tablehead1", data, FALSE);
  int loop;
  Setting fund;
  for (loop = 1; loop < (int) l.count(); loop++)
  {
    QString k = l[loop];
    int p = k.find(">", 0, TRUE);
    if (p == -1)
      continue;
    p++;
    k.remove(0, p);
    p = k.find("<", 0, TRUE);
    if (p == -1)
      continue;
    k.truncate(p - 1);
    
    if (k.contains("&sup"))
      k.truncate(k.length() - 6);
    if (k.contains("&amp"))
      k.remove(k.find("&", 0, TRUE), 5);
    k = k.stripWhiteSpace();
    if (! k.length())
      continue;
    
    QString d = l[loop];
    p = d.find("yfnc_tabledata1", 0, TRUE);
    if (p == -1)
      continue;
    p = d.find(">", p, TRUE);
    if (p == -1)
      continue;
    p++;
    d.remove(0, p);
    p = d.find("<", 0, TRUE);
    if (p == -1)
      continue;
    d.truncate(p);
    d = d.stripWhiteSpace();  
    if (! d.length())
      continue;

    fund.setData(k, d);
  }
      
  Config config;
  QString s = dataPath + "/";
  QFileInfo fi(currentUrl->getData("symbol"));
  if (fi.extension(FALSE).length())
    s.append(fi.extension(FALSE).upper());
  else
    s.append("US");
  s.append("/");
  s.append(currentUrl->getData("symbol"));
  
  DbPlugin *plug = config.getDbPlugin("Stocks");
  if (! plug)
  {
    config.closePlugin("Stocks");
    return;
  }
  
  if (plug->openChart(s))
  {
    emit statusLogMessage("Could not open db.");
    config.closePlugin("Stocks");
    return;
  }
  
  // verify if this chart can be updated by this plugin
  s = plug->getHeaderField(DbPlugin::QuotePlugin);
  if (! s.length())
    plug->setHeaderField(DbPlugin::QuotePlugin, pluginName);
  else
  {
    if (s.compare(pluginName))
    {
      s = currentUrl->getData("symbol") + " - skipping update. Source does not match destination.";
      emit statusLogMessage(s);
      config.closePlugin("Stocks");
      return;
    }
  }
  
  s = plug->getHeaderField(DbPlugin::Symbol);
  if (! s.length())
  {
    plug->createNew();
    plug->setHeaderField(DbPlugin::Symbol, currentUrl->getData("symbol"));
    
    QString title = currentUrl->getData("symbol");
    int p = data.find("yfnc_leftnav1", 0, TRUE);
    if (p != -1)
    {
      p = data.find(">", p, TRUE);
      if (p != -1)
      {
        int p2 = data.find("<", p, TRUE);
        if (p2 != -1)
          title = data.mid(p, p2 - p);
      }
    }
    
    plug->setHeaderField(DbPlugin::Title, title);
  }

  // include date of this update
  QDate dt = QDate::currentDate();
  fund.setData("updateDate", dt.toString("yyyy-MM-dd"));
  
  plug->setData("Fundamentals", fund.getString());
    
  s = "Updating " + currentUrl->getData("symbol");
  emit dataLogMessage(s);
  
  config.closePlugin("Stocks");
}

void Yahoo::loadAllSymbols ()
{
  symbolList.clear();
  
  QDir dir(dataPath);
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    QString s = dir.absPath() + "/" + dir[loop];
    QFileInfo fi(s);
    if (fi.isDir())
    {
      int loop2;
      QDir dir2(s);
      for (loop2 = 2; loop2 < (int) dir2.count(); loop2++)
        symbolList.append(dir2[loop2]);
    }
  }
  
  symbolList.sort();
}

void Yahoo::createHistoryUrls (QString d)
{
  if (sdate.date().daysTo(edate.date()) > 199) 
  { 
    QDateTime tsdate = sdate;
    QDateTime tedate = sdate;
	  
    while (tsdate <= edate)
    {
      tsdate = tedate;
      tedate = tsdate.addDays(199);
      if (tedate.date().dayOfWeek() == 6)
        tedate = tedate.addDays(-1);
      else
      {
        if (tedate.date().dayOfWeek() == 7)
          tedate = tedate.addDays(-2);
      }
            
      if (tedate > edate)
	tedate = edate;
	    
      QString s = "http://ichart.yahoo.com/table.csv?s=";
      s.append(d);
      s.append("&a=");
      s.append(QString::number(tsdate.date().month() - 1));
      s.append("&b=");
      s.append(tsdate.toString("dd"));
      s.append("&c=");
      s.append(tsdate.toString("yy"));
      s.append("&d=");
      s.append(QString::number(tedate.date().month() - 1));
      s.append("&e=");
      s.append(tedate.toString("dd"));
      s.append("&f=");
      s.append(tedate.toString("yy"));
      s.append("&g=d&q=q&y=0&x=.csv");
	    
      Setting *set = new Setting;
      set->setData("url", s);
      set->setData("symbol", d);
      url.append(set);
	    
      if (tedate == edate)
        break;
    }
  }
  else 
  { 
    QString s = "http://ichart.yahoo.com/table.csv?s=";
    s.append(d);
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
    
    Setting *set = new Setting;
    set->setData("url", s);
    set->setData("symbol", d);
    url.append(set);
  }
}

void Yahoo::createAutoHistoryUrls (QString path, QString d)
{
  Config config;
  DbPlugin *plug = config.getDbPlugin("Stocks");
  if (! plug)
  {
    config.closePlugin("Stocks");
    qDebug("Yahoo::createAutoHistoryUrls:could not open plugin");
    return;
  }
	
  if (plug->openChart(path))
  {
    config.closePlugin("Stocks");
    qDebug("Yahoo::createAutoHistoryUrls:could not open db");
    return;
  }
	
  // verify if this chart can be updated by this plugin
  QString s = plug->getHeaderField(DbPlugin::QuotePlugin);
  if (! s.length())
    plug->setHeaderField(DbPlugin::QuotePlugin, pluginName);
  else
  {
    if (s.compare(pluginName))
    {
      config.closePlugin("Stocks");
      qDebug("Yahoo::createAutoHistoryUrls:source not same as destination");
      return;
    }
  }
        
  edate = QDateTime::currentDateTime();
  if (edate.date().dayOfWeek() == 6)
    edate = edate.addDays(-1);
  else
  {
    if (edate.date().dayOfWeek() == 7)
      edate = edate.addDays(-2);
  }
	
  Bar *bar = plug->getLastBar();
  if (! bar)
  {
    QDateTime dt = edate;
    dt = dt.addDays(-365);
    bar = new Bar;
    bar->setDate(dt.toString("yyyyMMdd000000"));
  }

  if (bar->getDate().getDate() == edate.date())
  {
    delete bar;
    config.closePlugin("Stocks");
    qDebug("Yahoo::createAutoHistoryUrls:barDate == endDate");
    return;
  }
		
  s = "http://ichart.yahoo.com/table.csv?s=";
  s.append(d);
  s.append("&a=");
  s.append(QString::number(bar->getDate().getDate().month() - 1));
  s.append("&b=");
  s.append(bar->getDate().getDate().toString("dd"));
  s.append("&c=");
  s.append(bar->getDate().getDate().toString("yy"));
  s.append("&d=");
  s.append(QString::number(edate.date().month() - 1));
  s.append("&e=");
  s.append(edate.toString("dd"));
  s.append("&f=");
  s.append(edate.toString("yy"));
  s.append("&g=d&q=q&y=0&x=.csv");
	
  delete bar;
  config.closePlugin("Stocks");
  
  Setting *set = new Setting;
  set->setData("url", s);
  set->setData("symbol", d);
  url.append(set);
}

void Yahoo::createQuoteUrls (QString d)
{
  // multiple quotes: quote.yahoo.com/d/quotes.csv?s=IBM+RHAT+SCOX+MSFT+GOLD&f=sl1d1t1c1ohgv 
  // multiple close only: quote.yahoo.com/d/quotes.csv?s=IBM+RHAT+SCOX+MSFT+GOLD&f=sl1
  // URLPrefix = "http://quote.yahoo.com/d/quotes.csv?s=";
  // URLPostfix = "&f=snl1d1t1c1ohgv&e=.csv";	
  QString s = "http://finance.yahoo.com/d/quotes.csv?s=";
  s.append(d);
  s.append("&f=snl1d1t1c1ohgv&e=.csv");
  
  Setting *set = new Setting;
  set->setData("url", s);
  set->setData("symbol", d);
  url.append(set);
}

void Yahoo::createFundamentalUrls (QString d)
{
  QString s = "http://finance.yahoo.com/q/ks?s=" + d;
  Setting *set = new Setting;
  set->setData("url", s);
  set->setData("symbol", d);
  url.append(set);
}

//***********************************************************************
//***********************************************************************
//***********************************************************************

QuotePlugin * createQuotePlugin ()
{
  Yahoo *o = new Yahoo;
  return ((QuotePlugin *) o);
}


