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
#include <qsettings.h>

Yahoo::Yahoo ()
{
  pluginName = "Yahoo";
  helpFile = "yahoo.html";
  allSymbols = FALSE;
  url.setAutoDelete(TRUE);
  currentUrl = 0;
  
  Config config;
  dataPath = config.getData(Config::DataPath) + "/Stocks/Yahoo";
  file = config.getData(Config::Home) + "/download";
  
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
  
  connect(this, SIGNAL(signalGetFileDone(bool)), this, SLOT(fileDone(bool)));
  connect(this, SIGNAL(signalTimeout()), this, SLOT(timeoutError()));
  
  // preload all symbols to download for default
//  loadAllSymbols();
}

Yahoo::~Yahoo ()
{
}

void Yahoo::update ()
{
  Config config;
  errorLoop = 0;
  url.clear();
  errorList.clear();

  QDir dir;
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
      
    if (! method.compare("History"))
      createHistoryUrls(symbolList[loop]);
    else
    {
      if (! method.compare("Auto History"))
        createAutoHistoryUrls(path, symbolList[loop]);
      else
      {
        if (! method.compare("Quote"))
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

  QTimer::singleShot(250, this, SLOT(startDownload()));
}

void Yahoo::startDownload ()
{
  QString s = tr("Downloading ") + currentUrl->getData("symbol");
  emit statusLogMessage(s);

  s = currentUrl->getData("url");
  getFile(s);
}

void Yahoo::fileDone (bool d)
{
  if (d)
  {
//    emit statusLogMessage(tr("Network error aborting"));
//    emit statusLogMessage(tr("Done"));
//    emit done();
//    return;
  }
  else
  {
    if (method.contains("History"))
      parseHistory();
    else
    {
      if (method.contains("Quote"))
        parseQuote();
      else
        parseFundamental();
    }
  }

  currentUrl = url.next();
  if (! currentUrl)
  {
    emit done();
    emit statusLogMessage(tr("Done"));
    printErrorList();
    return;
  }

  errorLoop = 0;
  startDownload();    
}

void Yahoo::timeoutError ()
{
  errorLoop++;
  if (errorLoop == retries)
  {
    emit statusLogMessage(tr("Timeout: retry limit skipping") + currentUrl->getData("symbol") + tr(" skipped"));
    errorList.append(currentUrl->getData("symbol"));
    
    errorLoop = 0;
    currentUrl = url.next();
    if (! currentUrl)
    {
      emit done();
      emit statusLogMessage(tr("Done"));
      printErrorList();
      return;
    }

    startDownload();
  }
  else
  {
    QString s = tr("Timeout: retry ") + QString::number(errorLoop + 1) + " " + currentUrl->getData("symbol");
    emit statusLogMessage(s);
    startDownload();
  }
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

  QString plugin("Stocks");  
  DbPlugin *plug = config.getDbPlugin(plugin);
  if (! plug)
  {
    config.closePlugin(plugin);
    f.close();
    return;
  }

  if (plug->openChart(s))
  {
    emit statusLogMessage("Could not open db.");
    f.close();
    config.closePlugin(plugin);
    return;
  }

  // verify if this chart can be updated by this plugin
  plug->getHeaderField(DbPlugin::QuotePlugin, s);
  if (! s.length())
    plug->setHeaderField(DbPlugin::QuotePlugin, pluginName);
  else
  {
    if (s.compare(pluginName))
    {
      s = currentUrl->getData("symbol") + " - skipping update. Source does not match destination.";
      emit statusLogMessage(s);
      f.close();
      config.closePlugin(plugin);
      return;
    }
  }
  
  plug->getHeaderField(DbPlugin::Symbol, s);
  if (! s.length())
  {
    plug->createNew();
    s = currentUrl->getData("symbol");
    plug->setHeaderField(DbPlugin::Symbol, s);
    plug->setHeaderField(DbPlugin::Title, s);
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
    double open = 0;
    if (setTFloat(l[1], FALSE))
      continue;
    else
      open = tfloat;

    // high
    double high = 0;
    if (setTFloat(l[2], FALSE))
      continue;
    else
      high = tfloat;

    // low
    double low = 0;
    if (setTFloat(l[3], FALSE))
      continue;
    else
      low = tfloat;

    // close
    double close = 0;
    if (setTFloat(l[4], FALSE))
      continue;
    else
      close = tfloat;

    // volume
    double volume = 0;
    if (l.count() >= 6)
    {
      if (setTFloat(l[5], FALSE))
        continue;
      else
        volume = tfloat;
    }

    // adjusted close
    if (adjustment)
    {
      double adjclose = 0;
      if (l.count() >= 7)
      {
	if (setTFloat(l[6], FALSE))
	  continue;
	else
	  adjclose = tfloat;
	// apply yahoo's adjustments through all the data, not just closing price
	// i.e. adjust for stock splits and dividends
	float factor = close / adjclose;
	if (factor != 1)
	{
	  high = high / factor;
	  low = low / factor;
	  open = open / factor;
	  close = close / factor;
	  volume = volume * factor;
	}
      }
    }

    Bar bar;
    if (bar.setDate(date))
    {
      emit statusLogMessage("Bad date " + date);
      continue;
    }
    bar.setOpen(open);
    bar.setHigh(high);
    bar.setLow(low);
    bar.setClose(close);
    bar.setVolume(volume);
    plug->setBar(bar);
    
//    s = currentUrl->getData("symbol") + " " + date + " " + open + " " + high + " " + low
//        + " " + close + " " + volume;
	
//    emit dataLogMessage(s);
  }

  f.close();
  config.closePlugin(plugin);
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
  
  QString plugin("Stocks");
  DbPlugin *plug = config.getDbPlugin(plugin);
  if (! plug)
  {
    config.closePlugin(plugin);
    f.close();
    return;
  }
  
  if (plug->openChart(s))
  {
    emit statusLogMessage("Could not open db.");
    f.close();
    config.closePlugin(plugin);
    return;
  }

  // verify if this chart can be updated by this plugin
  plug->getHeaderField(DbPlugin::QuotePlugin, s);
  if (! s.length())
    plug->setHeaderField(DbPlugin::QuotePlugin, pluginName);
  else
  {
    if (s.compare(pluginName))
    {
      s = currentUrl->getData("symbol") + " - skipping update. Source does not match destination.";
      emit statusLogMessage(s);
      f.close();
      config.closePlugin(plugin);
      return;
    }
  }
    
  plug->getHeaderField(DbPlugin::Symbol, s);
  if (! s.length())
  {
    plug->createNew();
    s = currentUrl->getData("symbol");
    plug->setHeaderField(DbPlugin::Symbol, s);
    plug->setHeaderField(DbPlugin::Title, s);
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
    double open = 0;
    if (setTFloat(l[6], FALSE))
      continue;
    else
      open = tfloat;

    // high
    double high = 0;
    if (setTFloat(l[7], FALSE))
      continue;
    else
      high = tfloat;

    // low
    double low = 0;
    if (setTFloat(l[8], FALSE))
      continue;
    else
      low = tfloat;

    // close
    double close = 0;
    if (setTFloat(l[2], FALSE))
      continue;
    else
      close = tfloat;

    // volume
    double volume = 0;
    if (l.count() == 10)
    {
      if (setTFloat(l[9], FALSE))
        continue;
      else
        volume = tfloat;
    }
      
    Bar bar;
    if (bar.setDate(date))
    {
      emit statusLogMessage("Bad date " + date);
      continue;
    }
    bar.setOpen(open);
    bar.setHigh(high);
    bar.setLow(low);
    bar.setClose(close);
    bar.setVolume(volume);
    plug->setBar(bar);
    
//    s = currentUrl->getData("symbol") + " " + date + " " + open + " " + high + " " + low
//        + " " + close + " " + volume;
	
//    emit dataLogMessage(s);
  }

  f.close();
  config.closePlugin(plugin);
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
  
  method = settings.readEntry("/Method", "History");
  
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
  int loop;
  for (loop = 0; loop < (int) errorList.count(); loop++)
  {
    QString s = tr("Unable to download ") + errorList[loop];
    emit statusLogMessage(s);
  }
}	

void Yahoo::cancelUpdate ()
{
  if (op)
  {
    timer->stop();
    op->stop();
  }
  
  emit done();
  emit statusLogMessage(tr("Canceled"));
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
  
  QString plugin("Stocks");
  DbPlugin *plug = config.getDbPlugin(plugin);
  if (! plug)
  {
    config.closePlugin(plugin);
    return;
  }
  
  if (plug->openChart(s))
  {
    emit statusLogMessage("Could not open db.");
    config.closePlugin(plugin);
    return;
  }
  
  // verify if this chart can be updated by this plugin
  plug->getHeaderField(DbPlugin::QuotePlugin, s);
  if (! s.length())
    plug->setHeaderField(DbPlugin::QuotePlugin, pluginName);
  else
  {
    if (s.compare(pluginName))
    {
      s = currentUrl->getData("symbol") + " - skipping update. Source does not match destination.";
      emit statusLogMessage(s);
      config.closePlugin(plugin);
      return;
    }
  }
  
  plug->getHeaderField(DbPlugin::Symbol, s);
  if (! s.length())
  {
    plug->createNew();
    s = currentUrl->getData("symbol");
    plug->setHeaderField(DbPlugin::Symbol, s);
    
    QString title = currentUrl->getData("symbol");
    int p = data.find("yfnc_leftnav1", 0, TRUE);
    if (p != -1)
    {
      p = data.find("b>", p, TRUE);
      if (p != -1)
      {
        p = p + 2;
        int p2 = data.find("<", p, TRUE);
        if (p2 != -1)
	{
	  s = data.mid(p, p2 - p);
	  if (s.length())
            title = s;
	}
      }
    }
    
    plug->setHeaderField(DbPlugin::Title, title);
  }
  else
  {
    QString s2;
    plug->getHeaderField(DbPlugin::Title, s2);
    if (! s.compare(s2))
    {
      int p = data.find("yfnc_leftnav1", 0, TRUE);
      if (p != -1)
      {
        p = data.find("b>", p, TRUE);
        if (p != -1)
        {
          p = p + 2;
          int p2 = data.find("<", p, TRUE);
          if (p2 != -1)
	  {
            s = data.mid(p, p2 - p);
	    if (s.length())
              plug->setHeaderField(DbPlugin::Title, s);
	  }
	}
      }
    }
  }
  
  // include date of this update
  QDate dt = QDate::currentDate();
  fund.setData("updateDate", dt.toString("yyyy-MM-dd"));
  QString s2;
  fund.getString(s2);
  s = "Fundamentals";
  plug->setData(s, s2);
    
  s = "Updating " + currentUrl->getData("symbol");
  emit dataLogMessage(s);
  
  config.closePlugin(plugin);
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
  QString plugin("Stocks");
  DbPlugin *plug = config.getDbPlugin(plugin);
  if (! plug)
  {
    config.closePlugin(plugin);
    qDebug("Yahoo::createAutoHistoryUrls:could not open plugin");
    return;
  }
	
  if (plug->openChart(path))
  {
    config.closePlugin(plugin);
    qDebug("Yahoo::createAutoHistoryUrls:could not open db");
    return;
  }
	
  // verify if this chart can be updated by this plugin
  QString s;
  plug->getHeaderField(DbPlugin::QuotePlugin, s);
  if (! s.length())
    plug->setHeaderField(DbPlugin::QuotePlugin, pluginName);
  else
  {
    if (s.compare(pluginName))
    {
      config.closePlugin(plugin);
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
    s = dt.toString("yyyyMMdd000000");
    bar->setDate(s);
  }

  if (bar->getDate().getDate() == edate.date())
  {
    delete bar;
    config.closePlugin(plugin);
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
  config.closePlugin(plugin);
  
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


