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

#include "CME.h"
#include "DbPlugin.h"
#include "PrefDialog.h"
#include "Bar.h"
#include "Config.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qnetwork.h>
#include <qdir.h>
#include <qstringlist.h>
#include <stdlib.h>
#include <qsettings.h>
#include <qfileinfo.h>


CME::CME ()
{
  pluginName = "CME";
  symbolCombo = 0;
  helpFile = "cme.html";
  downloadIndex = 0;
  cancelFlag = FALSE;
  
  symbolList = fd.getSymbolList("CME");
  symbolList.sort();
  
  connect(this, SIGNAL(signalCopyFileDone(QString)), this, SLOT(fileDone(QString)));
  connect(this, SIGNAL(signalTimeout()), this, SLOT(timeoutError()));
  
  Config config;
  file = config.getData(Config::Home) + "/download";
  
  loadSettings();
  qInitNetworkProtocols();
}

CME::~CME ()
{
}

void CME::update ()
{
  urlList.clear();
  symbolLoop = 0;
  errorLoop = 0;

  if (! method.compare("Today"))
  {
    urlList.append("ftp://ftp.cme.com//pub/settle/stlags");
    urlList.append("ftp://ftp.cme.com//pub/settle/stlcur");
    urlList.append("ftp://ftp.cme.com//pub/settle/stleqt");
    urlList.append("ftp://ftp.cme.com//pub/settle/stlint");
  }
  else
  {
    downloadIndex = 0;
    Config config;
    QString s = config.getData(Config::Home);
  
    // remove any old files
    QDir dir(s);
    int loop;
    for (loop = 2; loop < (int) dir.count(); loop++)
    {
      QString t = dir.absPath() + "/" + dir[loop];
      QFileInfo fi(t);
      if (fi.isDir())
        continue;
      
      if (! fi.extension(TRUE).compare("zip"))
      {
        dir.remove(fi.absFilePath());
        continue;
      }
    
      if (! fi.extension(TRUE).compare("eod"))
        dir.remove(fi.absFilePath());
    }

    file2 = s;
    s.append("/download.zip");
    file = s;

    s = "ftp://ftp.cme.com//pub/hist_eod/";
    s.append(downloadSymbolList[downloadIndex].lower());
    s.append("ytd.zip");
    urlList.append(s);
  }
  
  QTimer::singleShot(250, this, SLOT(startDownload()));
}

void CME::startDownload ()
{
  QString s = tr("Downloading ");
  s.append(urlList[symbolLoop]);
  emit statusLogMessage(s);

  copyFile(urlList[symbolLoop], file);
}

void CME::fileDone (QString d)
{
  if (d.length())
  {
    qDebug(d.latin1());
    emit statusLogMessage(d);
    emit statusLogMessage(tr("Done"));
    emit done();
    return;
  }

  if (! method.compare("Today"))
  {
    parseToday();
    
    symbolLoop++;
    
    if (symbolLoop >= (int) urlList.count())
    {
      emit statusLogMessage(tr("Done"));
      emit done();
      return;
    }

    startDownload();    
  }
  else
  {
    parseHistory();
    emit statusLogMessage(tr("Done"));
    emit done();
  }
}

void CME::timeoutError ()
{
  errorLoop++;
  if (errorLoop == retries)
  {
    emit statusLogMessage(tr("Timeout: retry limit skipping file"));
  
    errorLoop = 0;
    
    if (! method.compare("Today"))
    {
      symbolLoop++;
      if (symbolLoop >= (int) urlList.count())
      {
        emit statusLogMessage(tr("Done"));
        emit done();
        return;
      }

      startDownload();
    }
  }
  else
  {
    QString s = tr("Timeout: retry ") + QString::number(errorLoop + 1);
    emit statusLogMessage(s);
    startDownload();
  }
}

void CME::parseToday ()
{
  QFile f(file);
  if (! f.open(IO_ReadOnly))
    return;
  QTextStream stream(&f);

  QString s = stream.readLine();
  s = s.stripWhiteSpace();
  QString s2 = "PRICES AS OF ";
  int p = s.contains(s2);
  s.remove(0, p + s2.length() - 1);
  QString date = s.left(8);
  QStringList dl = QStringList::split("/", date, FALSE);
  date = "20";
  date.append(dl[2]);
  date.append(dl[0]);
  date.append(dl[1]);

  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    s = s.stripWhiteSpace();

    if (s.contains("FC FEEDER CATTLE"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("FC");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("LB LUMBER FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("LB");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("LC LIVE CATTLE FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("LC");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("LH LEAN HOG FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("LN");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("PB FROZEN PORK BELLY FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("PB");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("AD AUSTRALIAN DOLLAR FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("AD");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("NB BRITISH POUND FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("NB");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("CD CANADIAN DOLLAR FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("CD");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("EC EURO FX FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("EC");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("JY JAPANESE YEN FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("JY");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("SF SWISS FRANC FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("SF");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("ES E-MINI S&P 500 FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("ES");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("GI GSCI FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("GI");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("ND NASDAQ-100 FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("ND");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("NQ E-MINI NASDAQ-100 FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("NQ");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("SP S&P 500 FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("SP");
	saveTodayData(l);
      }

      continue;
    }

    if (s.contains("ED EURODOLLAR FUTURES"))
    {
      while(stream.atEnd() == 0)
      {
        s = stream.readLine();
        s = s.stripWhiteSpace();

	if (s.contains("TOTAL"))
	  break;

	QStringList l = QStringList::split(" ", s, FALSE);
	if (l.count() != 11)
	  continue;

	l.prepend(date);
	l.prepend("ED");
	saveTodayData(l);
      }
    }
  }

  f.close();
}

void CME::parseHistory ()
{
  QString s2 = file2;
  s2.append("/");
  s2.append(downloadSymbolList[downloadIndex].lower());
  s2.append("ytd.eod");
  QDir dir(s2);
  dir.remove(s2);

  QString s = "unzip ";
  s.append(file);
  s.append(" -d ");
  s.append(file2);
  if (system(s))
    return;

  QFile f(s2);
  if (! f.open(IO_ReadOnly))
  {
    emit statusLogMessage(tr("could not open parse history file"));
    return;
  }
  QTextStream stream(&f);
  
  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    s = s.stripWhiteSpace();
    if (! s.length())
      continue;

    Setting *set = new Setting;

    // date
    s2 = s.mid(31, 6);
    s2.prepend("20");
    s2.append("000000");
    set->setData("Date", s2);

    // csymbol
    s2 = s.mid(37, 2);
    set->setData("CSymbol", s2);

    // symbol
    s2 = s.mid(37, 2);
    s2.append("20");
    s2.append(s.mid(41, 2));

    QString month = s.mid(43, 2);
    switch (month.toInt())
    {
      case 1:
        month = "F";
        break;
      case 2:
        month = "G";
        break;
      case 3:
        month = "H";
        break;
      case 4:
        month = "J";
        break;
      case 5:
        month = "K";
        break;
      case 6:
        month = "M";
        break;
      case 7:
        month = "N";
        break;
      case 8:
        month = "Q";
        break;
      case 9:
        month = "U";
        break;
      case 10:
        month = "V";
        break;
      case 11:
        month = "X";
        break;
      default:
        month = "Z";
        break;
    }
    s2.append(month);
    set->setData("Symbol", s2);

    set->setData("Month", month);

    QString dec = s.mid(30, 1);
    
    // fix decimal for JY
    s2 = set->getData("CSymbol");
    if (! s2.compare("JY"))
      dec = QString::number(dec.toInt() - 2);

    // open
    s2 = s.mid(53, 9);
    s2 = s2.insert(s2.length() - dec.toInt(), ".");
    set->setData("Open", s2);

    // high
    s2 = s.mid(73, 9);
    s2 = s2.insert(s2.length() - dec.toInt(), ".");
    set->setData("High", s2);

    // low
    s2 = s.mid(83, 9);
    s2 = s2.insert(s2.length() - dec.toInt(), ".");
    set->setData("Low", s2);

    // close
    s2 = s.mid(113, 9);
    s2 = s2.insert(s2.length() - dec.toInt(), ".");
    set->setData("Close", s2);

    // volume
    s2 = s.mid(122, 7);
    set->setData("Volume", s2);

    // oi
    s2 = s.mid(136, 7);
    set->setData("OI", s2);

    parse(set);

    delete set;
  }

  f.close();
}

void CME::saveTodayData (QStringList l)
{
  Setting *set = new Setting();

  QString symbol = l[0];
  symbol.append("20");
  QString s = l[2];
  s.remove(0, 3);
  symbol.append(s);

  s = l[2];
  s.truncate(3);
  QString month;
  while (1)
  {
    if (! s.compare("JAN"))
    {
      month = "F";
      break;
    }

    if (! s.compare("FEB"))
    {
      month = "G";
      break;
    }

    if (! s.compare("MAR"))
    {
      month = "H";
      break;
    }

    if (! s.compare("APR"))
    {
      month = "J";
      break;
    }

    if (! s.compare("MAY"))
    {
      month = "K";
      break;
    }

    if (! s.compare("JUN"))
    {
      month = "M";
      break;
    }

    if (! s.compare("JLY"))
    {
      month = "N";
      break;
    }

    if (! s.compare("AUG"))
    {
      month = "Q";
      break;
    }

    if (! s.compare("SEP"))
    {
      month = "U";
      break;
    }

    if (! s.compare("OCT"))
    {
      month = "V";
      break;
    }

    if (! s.compare("NOV"))
    {
      month = "X";
      break;
    }

    month = "Z";

    break;
  }
  symbol.append(month);

  set->setData("CSymbol", l[0]);
  set->setData("Symbol", symbol);

  s = l[1];
  s.append("000000");
  set->setData("Date", s);

  set->setData("Month", month);
  
  QString open = l[3];
  QString high = l[4];
  QString low = l[5];
  QString close = l[7];
  QString volume = l[9];
  QString oi = l[12];

  s = l[0];
  if (! s.compare("JY"))
  {
    setTFloat(open, FALSE);
    open = QString::number(tfloat);
    if (open.toFloat() > 9999)
      open.insert(1, ".");
    else
      open.prepend(".");

    setTFloat(high, FALSE);
    high = QString::number(tfloat);
    if (high.toFloat() > 9999)
      high.insert(1, ".");
    else
      high.prepend(".");

    setTFloat(low, FALSE);
    low = QString::number(tfloat);
    if (low.toFloat() > 9999)
      low.insert(1, ".");
    else
      low.prepend(".");

    setTFloat(close, FALSE);
    close = QString::number(tfloat);
    if (close.toFloat() > 9999)
      close.insert(1, ".");
    else
      close.prepend(".");
  }

  set->setData("Open", open);
  set->setData("High", high);
  set->setData("Low", low);
  set->setData("Close", close);
  set->setData("Volume", volume);
  set->setData("OI", oi);

  parse(set);

  delete set;
}

void CME::parse (Setting *data)
{
  if (fd.setSymbol(data->getData("CSymbol")))
    return;

  // open
  QString open;
  if (setTFloat(data->getData("Open"), FALSE))
    return;
  else
    open = QString::number(tfloat);

  // high
  QString high;
  if (setTFloat(data->getData("High"), FALSE))
    return;
  else
    high = QString::number(tfloat);

  // low
  QString low;
  if (setTFloat(data->getData("Low"), FALSE))
    return;
  else
    low = QString::number(tfloat);

  // close
  QString close;
  if (setTFloat(data->getData("Close"), FALSE))
    return;
  else
    close = QString::number(tfloat);

  // volume
  QString volume;
  if (setTFloat(data->getData("Volume"), FALSE))
    return;
  else
    volume = QString::number(tfloat);

  // oi
  QString oi;
  if (setTFloat(data->getData("OI"), FALSE))
    return;
  else
    oi = QString::number(tfloat);

  // check for bad values
  if (close.toFloat() == 0)
    return;
  if (open.toFloat() == 0)
    open = close;
  if (high.toFloat() == 0)
    high = close;
  if (low.toFloat() == 0)
    low = close;

  QString s = "Futures/CME/" + fd.getSymbol();
  QString path = createDirectory(s);
  if (! path.length())
  {
    emit statusLogMessage(tr("Unable to create futures directory"));
    return;
  }

  s = tr("Updating ") + data->getData("Symbol");
  emit statusLogMessage(s);

  Config config;
  DbPlugin *db = config.getDbPlugin("Futures");
  if (! db)
  {
    config.closePlugin("Futures");
    return;
  }
  
  s = path + "/" + data->getData("Symbol");
  if (db->openChart(s))
  {
    emit statusLogMessage(tr("Could not open db."));
    config.closePlugin("Futures");
    return;
  }

  // verify if this chart can be updated by this plugin
  db->getHeaderField(DbPlugin::QuotePlugin, s);
  if (! s.length())
    db->setHeaderField(DbPlugin::QuotePlugin, pluginName);
  else
  {
    if (s.compare(pluginName))
    {
      s = data->getData("Symbol") + tr(" - skipping update. Source does not match destination.");
      emit statusLogMessage(s);
      config.closePlugin("Futures");
      return;
    }
  }
      
  db->getHeaderField(DbPlugin::Symbol, s);
  if (! s.length())
  {
    db->createNew();
    
    s = data->getData("Symbol");
    db->setHeaderField(DbPlugin::Symbol, s);
    
    s = fd.getName();
    db->setHeaderField(DbPlugin::Title, s);
    
    QString s2 = fd.getSymbol();
    s = "FuturesType";
    db->setData(s, s2);
    
    s2 = data->getData("Month");
    s = "FuturesMonth";
    db->setData(s, s2);
  }
  
  Bar bar;
  s = data->getData("Date");
  if (bar.setDate(s))
  {
    emit statusLogMessage("Bad date " + data->getData("Date"));
    config.closePlugin("Futures");
    return;
  }
  bar.setOpen(open.toDouble());
  bar.setHigh(high.toDouble());
  bar.setLow(low.toDouble());
  bar.setClose(close.toDouble());
  bar.setVolume(volume.toDouble());
  bar.setOI(oi.toInt());
  db->setBar(bar);
	     
  config.closePlugin("Futures");

//  emit dataLogMessage(data->getData("Symbol"));
}

void CME::cancelUpdate ()
{
  if (op)
  {
    timer->stop();
    op->stop();
  }
  
  emit done();
  emit statusLogMessage(tr("Canceled"));
}

void CME::prefDialog (QWidget *w)
{
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("CME Prefs"));
  dialog->createPage (tr("Details"));
  dialog->setHelpFile(helpFile);

  QStringList l2;
  l2.append("Today");
  l2.append("History");
  dialog->addComboItem(tr("Method"), tr("Details"), l2, method);
  connect(dialog->getComboWidget(tr("Method")),
          SIGNAL(activated(const QString &)),
	  this,
	  SLOT(methodChanged(const QString &)));
    
  dialog->addComboItem(tr("Symbol"), tr("Details"), symbolList, currentSymbol);
  symbolCombo = dialog->getComboWidget(tr("Symbol"));
  methodChanged (method);

  dialog->addIntItem(tr("Retry"), tr("Details"), retries, 0, 99);  
  dialog->addIntItem(tr("Timeout"), tr("Details"), timeout, 0, 99);  
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    downloadSymbolList = dialog->getCombo(tr("Symbol"));
    currentSymbol = dialog->getCombo(tr("Symbol"));
    timeout = dialog->getInt(tr("Timeout"));
    retries = dialog->getInt(tr("Retry"));
    
    saveFlag = TRUE;
    saveSettings();
  }
  
  delete dialog;
}

void CME::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/CME plugin");

  method = settings.readEntry("/Method", "Today");
  currentSymbol = settings.readEntry("/Symbol", "AD");
  
  QString s = settings.readEntry("/Retry", "3");
  retries = s.toInt();
  
  s = settings.readEntry("/Timeout", "15");
  timeout = s.toInt();
  
  settings.endGroup();
}

void CME::saveSettings ()
{
  if (! saveFlag)
    return;

  QSettings settings;
  settings.beginGroup("/Qtstalker/CME plugin");
  
  settings.writeEntry("/Method", method);
  settings.writeEntry("/Symbol", currentSymbol);
  settings.writeEntry("/Retry", QString::number(retries));
  settings.writeEntry("/Timeout", QString::number(timeout));
  
  settings.endGroup();
}

void CME::methodChanged (const QString & d)
{
  method = d;
  
  if (! method.compare("Today"))
    symbolCombo->setEnabled(FALSE);
  else
    symbolCombo->setEnabled(TRUE);
}

//*************************************************************
//*************************************************************
//*************************************************************

QuotePlugin * createQuotePlugin ()
{
  CME *o = new CME;
  return ((QuotePlugin *) o);
}

