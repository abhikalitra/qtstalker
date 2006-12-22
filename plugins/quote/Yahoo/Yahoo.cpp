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
#include "Bar.h"
#include "DBIndexItem.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qdir.h>
#include <qdatetime.h>
#include <qnetwork.h>
#include <qsettings.h>
#include "DbPlugin.h"
#include "../../../pics/newchart.xpm"
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qframe.h>

Yahoo::Yahoo ()
{
  pluginName = "Yahoo";
  helpFile = "yahoo.html";
  url.setAutoDelete(TRUE);
  currentUrl = 0;
  
  config.getData(Config::DataPath, dataPath);
  dataPath.append("/Stocks/Yahoo");

  config.getData(Config::Home, file);
  file.append("/download");
  
  qInitNetworkProtocols();

  buildGui();
  
  loadSettings();
  
  connect(this, SIGNAL(signalGetFileDone(bool)), this, SLOT(fileDone(bool)));
  connect(this, SIGNAL(signalTimeout()), this, SLOT(timeoutError()));
  
  resize(400, 450);
}

Yahoo::~Yahoo ()
{
  plug.close();
  saveSettings();
}

void Yahoo::buildGui ()
{
  setCaption(tr("Yahoo Quotes"));

  config.getData(Config::DataPath, dataPath);
  dataPath.append("/Stocks/Yahoo");

  QString s = "new";
  QString s2 = tr("New Symbol");
  toolbar->addButton(s, newchart, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(newStock()));
  
  QLabel *label = new QLabel(tr("Method"), baseWidget);
  grid->addWidget(label, 0, 0);
  
  method = new QComboBox(baseWidget);
  method->insertItem("History", 0);
  method->insertItem("Auto History", 1);
  method->insertItem("Quote", 2);
  method->insertItem("Fundamental", 3);
  QObject::connect(method, SIGNAL(activated(int)), this, SLOT(methodChanged(int)));
  grid->addWidget(method, 0, 1);
  
  label = new QLabel(tr("Start Date"), baseWidget);
  grid->addWidget(label, 1, 0);

  sdate = new QDateEdit(QDate::currentDate(), baseWidget);
  sdate->setAutoAdvance(TRUE);
  sdate->setOrder(QDateEdit::YMD);
  grid->addWidget(sdate, 1, 1);

  label = new QLabel(tr("End Date"), baseWidget);
  grid->addWidget(label, 2, 0);
  
  edate = new QDateEdit(QDate::currentDate(), baseWidget);
  edate->setAutoAdvance(TRUE);
  edate->setOrder(QDateEdit::YMD);
  grid->addWidget(edate, 2, 1);

  QDate dt = QDate::currentDate();
  if (dt.dayOfWeek() == 6)
    dt = dt.addDays(-1);
  else
  {
    if (dt.dayOfWeek() == 7)
      dt = dt.addDays(-2);
  }
  edate->setDate(dt);
  dt = dt.addDays(-1);
  sdate->setDate(dt);
  
  adjustment = new QCheckBox(tr("Adjustment"), baseWidget);
  grid->addWidget(adjustment, 3, 0);
  
  allSymbols = new QCheckBox(tr("All Symbols"), baseWidget);
  connect(allSymbols, SIGNAL(toggled(bool)), this, SLOT(allSymbolsChecked(bool)));
  grid->addWidget(allSymbols, 4, 0);

  QStringList l;  
  list = new FileButton(baseWidget, l, dataPath);
  grid->addWidget(list, 4, 1);
}

void Yahoo::update ()
{
  plug.close();
  errorLoop = 0;
  url.clear();
  errorList.clear();

  if (allSymbols->isChecked())
    allSymbolsChecked(TRUE);
  else
    allSymbolsChecked(FALSE);

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
      
    if (! method->currentText().compare("History"))
      createHistoryUrls(symbolList[loop]);
    else
    {
      if (! method->currentText().compare("Auto History"))
        createAutoHistoryUrls(path, symbolList[loop]);
      else
      {
        if (! method->currentText().compare("Quote"))
          createQuoteUrls(symbolList[loop]);
	else
          createFundamentalUrls(symbolList[loop]);
      }
    }
  }

  if (! url.count())
  {
    downloadComplete();
    QString ss(tr("No symbols selected. Done"));
    printStatusLogMessage(ss);
    return;
  }
  
  currentUrl = url.first();

  startDownload();
}

void Yahoo::startDownload ()
{
  QString s, ts2;
  QString ts = "symbol";
  currentUrl->getData(ts, ts2);
  s = tr("Downloading") + " " + ts2;
//  printStatusLogMessage(s);

  ts = "url";
  currentUrl->getData(ts, s);
  getFile(s);
}

void Yahoo::fileDone (bool d)
{
  if (d)
  {
//    QString ss(tr("Network error aborting"));
//    printStatusLogMessage(ss);
//    printStatusLogMessage(stringDone);
//    downloadComplete();
//    return;
  }
  else
  {
    if (method->currentText().contains("History"))
      parseHistory();
    else
    {
      if (method->currentText().contains("Quote"))
        parseQuote();
      else
        parseFundamental();
    }
  }

  currentUrl = url.next();
  if (! currentUrl)
  {
    downloadComplete();
    printStatusLogMessage(stringDone);
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
  if (errorLoop == retrySpin->value())
  {
    currentUrl->getData(ts, ts2);
    QString ss = tr("Timeout: retry limit skipping") + " " + ts2 + " " + tr("skipped");
    printStatusLogMessage(ss);
    errorList.append(ts2);
    
    errorLoop = 0;
    currentUrl = url.next();
    if (! currentUrl)
    {
      downloadComplete();
      printStatusLogMessage(stringDone);
      printErrorList();
      return;
    }

    startDownload();
  }
  else
  {
    currentUrl->getData(ts, ts2);
    QString ss = tr("Timeout: retry") + " " + QString::number(errorLoop + 1) + " " + ts2;
    printStatusLogMessage(ss);
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

  if (plug.open(s, chartIndex))
  {
    QString ss(tr("Could not open db"));
    printStatusLogMessage(ss);
    f.close();
    return;
  }

  QString fn = ts2;

  // verify if this chart can be updated by this plugin
  DBIndexItem item;
  chartIndex->getIndexItem(fn, item);
  item.getQuotePlugin(s);
  if (! s.length())
  {
    item.setQuotePlugin(pluginName);
    chartIndex->setIndexItem(fn, item);
  }
  else
  {
    if (s.compare(pluginName))
    {
      s = ts2 + " - " + tr("skipping update. Source does not match destination");
      printStatusLogMessage(s);
      f.close();
      plug.close();
      return;
    }
  }

  item.getSymbol(s);  
  if (! s.length())
  {
    plug.createNewStock();
    item.setSymbol(ts2);
    item.setTitle(ts2);
    chartIndex->setIndexItem(fn, item);
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
      QString ss = tr("Bad date") + " " + date;
      printStatusLogMessage(ss);
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

    emit signalWakeup();
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
  
  if (plug.open(s, chartIndex))
  {
    QString ss(tr("Could not open db"));
    printStatusLogMessage(ss);
    f.close();
    return;
  }

  QString fn = ts2;

  // verify if this chart can be updated by this plugin
  DBIndexItem item;
  chartIndex->getIndexItem(fn, item);
  item.getQuotePlugin(s);
  if (! s.length())
  {
    item.setQuotePlugin(pluginName);
    chartIndex->setIndexItem(fn, item);
  }
  else
  {
    if (s.compare(pluginName))
    {
      s = ts2 + " - " + tr("skipping update. Source does not match destination");
      printStatusLogMessage(s);
      f.close();
      plug.close();
      return;
    }
  }

  item.getSymbol(s);    
  if (! s.length())
  {
    plug.createNewStock();
    item.setSymbol(ts2);
    item.setTitle(ts2);
    chartIndex->setIndexItem(fn, item);
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
      QString ss = tr("Bad date") + " " + date;
      printStatusLogMessage(ss);
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

    emit signalWakeup();
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

void Yahoo::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/Yahoo plugin");

  QString s = settings.readEntry("/Adjustment", "0");
  adjustment->setChecked(s.toInt());
  
  s = settings.readEntry("/Method", "History");
  setMethod(s);
  
  s = settings.readEntry("/Retries", "3");
  retrySpin->setValue(s.toInt());
  
  s = settings.readEntry("/Timeout", "15");
  timeoutSpin->setValue(s.toInt());
  
  s = settings.readEntry("/AllSymbols", "1");
  allSymbols->setChecked(s.toInt());
  allSymbolsChecked(s.toInt());
  
  settings.endGroup();
}

void Yahoo::saveSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/Yahoo plugin");
  
  settings.writeEntry("/Adjustment", QString::number(adjustment->isChecked()));
  settings.writeEntry("/Method", method->currentText());
  settings.writeEntry("/Retries", retrySpin->text());
  settings.writeEntry("/Timeout", timeoutSpin->text());
  settings.writeEntry("/AllSymbols", QString::number(allSymbols->isChecked()));

  settings.endGroup();
}

void Yahoo::printErrorList ()
{
  int loop;
  for (loop = 0; loop < (int) errorList.count(); loop++)
  {
    QString s = tr("Unable to download") + " " + errorList[loop];
    printStatusLogMessage(s);
  }
}	

void Yahoo::cancelUpdate ()
{
  if (op)
  {
    timer->stop();
    op->stop();
  }
  
  downloadComplete();
  printStatusLogMessage(stringCanceled);
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
    k.truncate(p);
    
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
  
  if (plug.open(s, chartIndex))
  {
    QString ss(tr("Could not open db"));
    printStatusLogMessage(ss);
    return;
  }

  QString fn = ts2;
  
  // verify if this chart can be updated by this plugin
  DBIndexItem item;
  chartIndex->getIndexItem(fn, item);
  item.getQuotePlugin(s);
  if (! s.length())
  {
    item.setQuotePlugin(pluginName);
    chartIndex->setIndexItem(fn, item);
  }
  else
  {
    if (s.compare(pluginName))
    {
      s = ts2 + " - " + tr("skipping update. Source does not match destination");
      printStatusLogMessage(s);
      plug.close();
      return;
    }
  }

  item.getSymbol(s);  
  if (! s.length())
  {
    plug.createNewStock();
    item.setSymbol(ts2);
    
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

    item.setTitle(title);
    chartIndex->setIndexItem(fn, item);
  }
  else
  {
    QString s2;
    item.getTitle(s2);
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
            {
              item.setTitle(s);
              chartIndex->setIndexItem(fn, item);
            }
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
  chartIndex->setFundamentals(fn, ts2);
    
  plug.close();

  emit signalWakeup();
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
  if (sdate->date().daysTo(edate->date()) > 199) 
  { 
    QDate tsdate = sdate->date();
    QDate tedate = sdate->date();
	  
    while (tsdate <= edate->date())
    {
      tsdate = tedate;
      tedate = tsdate.addDays(199);
      if (tedate.dayOfWeek() == 6)
        tedate = tedate.addDays(-1);
      else
      {
        if (tedate.dayOfWeek() == 7)
          tedate = tedate.addDays(-2);
      }
            
      if (tedate > edate->date())
	tedate = edate->date();
	    
      QString s = "http://ichart.yahoo.com/table.csv?s=";
      s.append(d);
      s.append("&a=");
      s.append(QString::number(tsdate.month() - 1));
      s.append("&b=");
      s.append(tsdate.toString("dd"));
      s.append("&c=");
      s.append(tsdate.toString("yy"));
      s.append("&d=");
      s.append(QString::number(tedate.month() - 1));
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
	    
      if (tedate == edate->date())
        break;
    }
  }
  else 
  { 
    QString s = "http://ichart.yahoo.com/table.csv?s=";
    s.append(d);
    s.append("&a=");
    s.append(QString::number(sdate->date().month() - 1));
    s.append("&b=");
    s.append(sdate->date().toString("dd"));
    s.append("&c=");
    s.append(sdate->date().toString("yy"));
    s.append("&d=");
    s.append(QString::number(edate->date().month() - 1));
    s.append("&e=");
    s.append(edate->date().toString("dd"));
    s.append("&f=");
    s.append(edate->date().toString("yy"));
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
  if (plug.open(path, chartIndex))
  {
    qDebug("Yahoo::createAutoHistoryUrls:could not open db");
    return;
  }

  QFileInfo fi(path);
  QString fn = fi.fileName();

  // verify if this chart can be updated by this plugin
  QString s;
  DBIndexItem item;
  chartIndex->getIndexItem(fn, item);
  item.getQuotePlugin(s);
  if (! s.length())
  {
    item.setQuotePlugin(pluginName);
    chartIndex->setIndexItem(fn, item);
  }
  else
  {
    if (s.compare(pluginName))
    {
      qDebug("Yahoo::createAutoHistoryUrls:source not same as destination");
      plug.close();
      return;
    }
  }

  QDate edt = QDate::currentDate();        
  if (edt.dayOfWeek() == 6)
    edt = edt.addDays(-1);
  else
  {
    if (edt.dayOfWeek() == 7)
      edt = edt.addDays(-2);
  }
	
  Bar bar;
  plug.getLastBar(bar);
  if (bar.getEmptyFlag())
  {
    QDate dt = edt;
    dt = dt.addDays(-365);
    s = dt.toString("yyyyMMdd000000");
    bar.setDate(s);
  }

  QDateTime dt;
  bar.getDate(dt);
  if (dt.date() == edt)
  {
//    qDebug("Yahoo::createAutoHistoryUrls:barDate == endDate");
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
  s.append(QString::number(edt.month() - 1));
  s.append("&e=");
  s.append(edt.toString("dd"));
  s.append("&f=");
  s.append(edt.toString("yy"));
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

void Yahoo::newStock ()
{
  bool ok = FALSE;
  QString symbols = QInputDialog::getText(tr("New Yahoo Symbols"),
                                          tr("Enter symbols to add. Note: separate symbols with a space"),
				          QLineEdit::Normal,
				          QString::null,
				          &ok,
				          this);
  if (! symbols.length())
    return;

  QStringList l = QStringList::split(" ", symbols, FALSE);
  
  QString s;
  config.getData(Config::DataPath, s);
  s.append("/Stocks");
  QDir dir(s);
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s))
    {
      qDebug("YahooDialog::newStock: Unable to create %s directory", s.latin1());
      return;
    }
  }
  
  if (! dir.exists(dataPath))
  {
    if (! dir.mkdir(dataPath))
    {
      qDebug("YahooDialog::newStock: Unable to create %s directory", dataPath.latin1());
      return;
    }
  }
  
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QString s = dataPath + "/";
    QFileInfo fi(l[loop]);
    if (fi.extension(FALSE).length())
    {
      s.append(fi.extension(FALSE).upper());
      if (! dir.exists(s, TRUE))
      {
        if (! dir.mkdir(s))
	{
          qDebug("YahooDialog::newStock: Unable to create %s directory", s.latin1());
          continue;
	}
      }
    }
    else
    {
      s.append("US");
      if (! dir.exists(s, TRUE))
      {
        if (! dir.mkdir(s))
	{
          qDebug("YahooDialog::newStock: Unable to create %s directory", s.latin1());
          continue;
	}
      }
    }
    s.append("/");
    s.append(l[loop]);
    if (dir.exists(s, TRUE))
      continue;

    if (plug.open(s, chartIndex))
    {
      qDebug("YahooDialog::newStock: could not open db %s", s.latin1());
      plug.close();
      continue;
    }
          
    plug.createNewStock();
    plug.close();
  }
}

void Yahoo::setMethod (QString d)
{
  if (! d.compare("History"))
  {
    method->setCurrentItem(0);
    methodChanged(0);
  }
  else
  {
    if (! d.compare("Auto History"))
    {
      method->setCurrentItem(1);
      methodChanged(1);
    }
    else
    {
      if (! d.compare("Quote"))
      {
        method->setCurrentItem(2);
        methodChanged(2);
      }
      else
      {
        method->setCurrentItem(3);
        methodChanged(3);
      }
    }
  }
}

void Yahoo::methodChanged (int d)
{
  switch (d)
  {
    case 0:
      adjustment->setEnabled(TRUE);
      sdate->setEnabled(TRUE);
      edate->setEnabled(TRUE);
      break;
    case 1:
      adjustment->setEnabled(TRUE);
      sdate->setEnabled(FALSE);
      edate->setEnabled(FALSE);
      break;
    case 2:
      adjustment->setEnabled(FALSE);
      sdate->setEnabled(FALSE);
      edate->setEnabled(FALSE);
      break;
    case 3:
      adjustment->setEnabled(FALSE);
      sdate->setEnabled(FALSE);
      edate->setEnabled(FALSE);
      break;
    default:
      break;
  }
}

void Yahoo::allSymbolsChecked (bool d)
{
  if (d)
  {
    list->setEnabled(FALSE);
    loadAllSymbols();
  }
  else
  {
    list->setEnabled(TRUE);
    list->getFile(fileList);
    int loop;
    symbolList.clear();
    for (loop = 0; loop < (int) fileList.count(); loop++)
    {
      QFileInfo fi(fileList[loop]);
      symbolList.append(fi.fileName());
    }
  }
}

//***********************************************************************
//***********************************************************************
//***********************************************************************

QuotePlugin * createQuotePlugin ()
{
  Yahoo *o = new Yahoo;
  return ((QuotePlugin *) o);
}

