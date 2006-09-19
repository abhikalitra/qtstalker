/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2006 Stefan S. Stratigakos
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
  
  config.getData(Config::DataPath, dataPath);
  dataPath.append("/Stocks/Yahoo");
  config.getData(Config::Home, file);
  file.append("/download");
  
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
  plug.close();
}

void Yahoo::update ()
{
  plug.close();
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
  QString s, ts2;
  QString ts = "symbol";
  currentUrl->getData(ts, ts2);
  s = tr("Downloading ") + ts2;
  emit statusLogMessage(s);

  ts = "url";
  currentUrl->getData(ts, s);
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
  QString ts = "symbol";
  QString ts2;
  errorLoop++;
  if (errorLoop == retries)
  {
    currentUrl->getData(ts, ts2);
    emit statusLogMessage(tr("Timeout: retry limit skipping") + ts2 + tr(" skipped"));
    errorList.append(ts2);
    
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
    currentUrl->getData(ts, ts2);
    emit statusLogMessage(tr("Timeout: retry ") + QString::number(errorLoop + 1) + " " + ts2);
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

  s = dataPath + "/";
  QString ts = "symbol";
  QString ts2;
  currentUrl->getData(ts, ts2);
  QFileInfo fi(ts2);
  if (fi.extension(FALSE).length())
    s.append(fi.extension(FALSE).upper());
  else
    s.append("US");
  s.append("/");
  s.append(ts2);

  if (plug.openChart(s))
  {
    emit statusLogMessage("Could not open db.");
    f.close();
    return;
  }

  // verify if this chart can be updated by this plugin
  plug.getHeaderField(DbPlugin::QuotePlugin, s);
  if (! s.length())
    plug.setHeaderField(DbPlugin::QuotePlugin, pluginName);
  else
  {
    if (s.compare(pluginName))
    {
      s = ts2 + " - skipping update. Source does not match destination.";
      emit statusLogMessage(s);
      f.close();
      plug.close();
      return;
    }
  }
  
  plug.getHeaderField(DbPlugin::Symbol, s);
  if (! s.length())
  {
    plug.createNew(DbPlugin::Stock);
    plug.setHeaderField(DbPlugin::Symbol, ts2);
    plug.setHeaderField(DbPlugin::Title, ts2);
  }

  while(stream.atEnd() == 0)
  {
    ts = stream.readLine();
    stripJunk(ts, s);

    QStringList l = QStringList::split(",", s, FALSE);
    if (l.count() < 5)
      continue;

    // date
    QString date = parseDate(l[0]);
    Bar bar;
    if (bar.setDate(date))
    {
      emit statusLogMessage("Bad date " + date);
      continue;
    }

    if (setTFloat(l[1], FALSE))
      continue;
    else
      bar.setOpen(tfloat);

    if (setTFloat(l[2], FALSE))
      continue;
    else
      bar.setHigh(tfloat);

    if (setTFloat(l[3], FALSE))
      continue;
    else
      bar.setLow(tfloat);

    if (setTFloat(l[4], FALSE))
      continue;
    else
      bar.setClose(tfloat);

    if (l.count() >= 6)
    {
      if (setTFloat(l[5], FALSE))
        continue;
      else
        bar.setVolume(tfloat);
    }

    if (bar.verify())
      continue;

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
	float factor = bar.getClose() / adjclose;
	if (factor != 1)
	{
	  bar.setHigh(bar.getHigh() / factor);
	  bar.setLow(bar.getLow() / factor);
	  bar.setOpen(bar.getOpen() / factor);
	  bar.setClose(bar.getClose() / factor);
	  bar.setVolume(bar.getVolume() * factor);
	}
      }
    }

    plug.setBar(bar);
  }

  f.close();
  plug.close();
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

  QString s = dataPath + "/";
  QString ts = "symbol";
  QString ts2;
  currentUrl->getData(ts, ts2);
  QFileInfo fi(ts2);
  if (fi.extension(FALSE).length())
    s.append(fi.extension(FALSE).upper());
  else
    s.append("US");
  s.append("/");
  s.append(ts2);
  
  if (plug.openChart(s))
  {
    emit statusLogMessage("Could not open db.");
    f.close();
    return;
  }

  // verify if this chart can be updated by this plugin
  plug.getHeaderField(DbPlugin::QuotePlugin, s);
  if (! s.length())
    plug.setHeaderField(DbPlugin::QuotePlugin, pluginName);
  else
  {
    if (s.compare(pluginName))
    {
      s = ts2 + " - skipping update. Source does not match destination.";
      emit statusLogMessage(s);
      f.close();
      plug.close();
      return;
    }
  }
    
  plug.getHeaderField(DbPlugin::Symbol, s);
  if (! s.length())
  {
    plug.createNew(DbPlugin::Stock);
    plug.setHeaderField(DbPlugin::Symbol, ts2);
    plug.setHeaderField(DbPlugin::Title, ts2);
  }

  while(stream.atEnd() == 0)
  {
    ts = stream.readLine();
    stripJunk(ts, s);

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

    Bar bar;
    if (bar.setDate(date))
    {
      emit statusLogMessage("Bad date " + date);
      continue;
    }

    if (setTFloat(l[6], FALSE))
      continue;
    else
      bar.setOpen(tfloat);

    if (setTFloat(l[7], FALSE))
      continue;
    else
      bar.setHigh(tfloat);

    if (setTFloat(l[8], FALSE))
      continue;
    else
      bar.setLow(tfloat);

    if (setTFloat(l[2], FALSE))
      continue;
    else
      bar.setClose(tfloat);

    if (l.count() == 10)
    {
      if (setTFloat(l[9], FALSE))
        continue;
      else
        bar.setVolume(tfloat);
    }

    if (bar.verify())
      continue;
      
    plug.setBar(bar);
  }

  f.close();
  plug.close();
}

QString Yahoo::parseDate (QString &d)
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
      dialog->getList(fileList);
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
      
  QString s = dataPath + "/";
  QString ts = "symbol";
  QString ts2;
  currentUrl->getData(ts, ts2);
  QFileInfo fi(ts2);
  if (fi.extension(FALSE).length())
    s.append(fi.extension(FALSE).upper());
  else
    s.append("US");
  s.append("/");
  s.append(ts2);
  
  if (plug.openChart(s))
  {
    emit statusLogMessage("Could not open db.");
    return;
  }
  
  // verify if this chart can be updated by this plugin
  plug.getHeaderField(DbPlugin::QuotePlugin, s);
  if (! s.length())
    plug.setHeaderField(DbPlugin::QuotePlugin, pluginName);
  else
  {
    if (s.compare(pluginName))
    {
      s = ts2 + " - skipping update. Source does not match destination.";
      emit statusLogMessage(s);
      plug.close();
      return;
    }
  }
  
  plug.getHeaderField(DbPlugin::Symbol, s);
  if (! s.length())
  {
    plug.createNew(DbPlugin::Stock);
    plug.setHeaderField(DbPlugin::Symbol, ts2);
    
    QString title = ts2;
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
    
    plug.setHeaderField(DbPlugin::Title, title);
  }
  else
  {
    QString s2;
    plug.getHeaderField(DbPlugin::Title, s2);
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
              plug.setHeaderField(DbPlugin::Title, s);
	  }
	}
      }
    }
  }
  
  // include date of this update
  QDate dt = QDate::currentDate();
  ts = "updateDate";
  ts2 = dt.toString("yyyy-MM-dd");
  fund.setData(ts, ts2);
  fund.getString(ts2);
  ts = "Fundamentals";
  plug.setData(ts, ts2);
    
  plug.close();
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

void Yahoo::createHistoryUrls (QString &d)
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
      QString ts = "url";
      set->setData(ts, s);
      ts = "symbol";
      set->setData(ts, d);
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
    QString ts = "url";
    set->setData(ts, s);
    ts = "symbol";
    set->setData(ts, d);
    url.append(set);
  }
}

void Yahoo::createAutoHistoryUrls (QString &path, QString &d)
{
  if (plug.openChart(path))
  {
    qDebug("Yahoo::createAutoHistoryUrls:could not open db");
    return;
  }
	
  // verify if this chart can be updated by this plugin
  QString s;
  plug.getHeaderField(DbPlugin::QuotePlugin, s);
  if (! s.length())
    plug.setHeaderField(DbPlugin::QuotePlugin, pluginName);
  else
  {
    if (s.compare(pluginName))
    {
      qDebug("Yahoo::createAutoHistoryUrls:source not same as destination");
      plug.close();
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
	
  Bar bar;
  plug.getLastBar(bar);
  if (bar.getEmptyFlag())
  {
    QDateTime dt = edate;
    dt = dt.addDays(-365);
    s = dt.toString("yyyyMMdd000000");
    bar.setDate(s);
  }

  QDateTime dt;
  bar.getDate(dt);
  if (dt.date() == edate.date())
  {
    qDebug("Yahoo::createAutoHistoryUrls:barDate == endDate");
    plug.close();
    return;
  }
		
  s = "http://ichart.yahoo.com/table.csv?s=";
  s.append(d);
  s.append("&a=");
  s.append(QString::number(dt.date().month() - 1));
  s.append("&b=");
  s.append(dt.toString("dd"));
  s.append("&c=");
  s.append(dt.toString("yy"));
  s.append("&d=");
  s.append(QString::number(edate.date().month() - 1));
  s.append("&e=");
  s.append(edate.toString("dd"));
  s.append("&f=");
  s.append(edate.toString("yy"));
  s.append("&g=d&q=q&y=0&x=.csv");
	
  plug.close();
  
  Setting *set = new Setting;
  QString ts = "url";
  set->setData(ts, s);
  ts = "symbol";
  set->setData(ts, d);
  url.append(set);
}

void Yahoo::createQuoteUrls (QString &d)
{
  // multiple quotes: quote.yahoo.com/d/quotes.csv?s=IBM+RHAT+SCOX+MSFT+GOLD&f=sl1d1t1c1ohgv 
  // multiple close only: quote.yahoo.com/d/quotes.csv?s=IBM+RHAT+SCOX+MSFT+GOLD&f=sl1
  // URLPrefix = "http://quote.yahoo.com/d/quotes.csv?s=";
  // URLPostfix = "&f=snl1d1t1c1ohgv&e=.csv";	
  QString s = "http://finance.yahoo.com/d/quotes.csv?s=";
  s.append(d);
  s.append("&f=snl1d1t1c1ohgv&e=.csv");
  
  Setting *set = new Setting;
  QString ts = "url";
  set->setData(ts, s);
  ts = "symbol";
  set->setData(ts, d);
  url.append(set);
}

void Yahoo::createFundamentalUrls (QString &d)
{
  QString s = "http://finance.yahoo.com/q/ks?s=" + d;
  Setting *set = new Setting;
  QString ts = "url";
  set->setData(ts, s);
  ts = "symbol";
  set->setData(ts, d);
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


