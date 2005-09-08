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

#include "NYBOT.h"
#include "PrefDialog.h"
#include "Setting.h"
#include "Bar.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qdir.h>
#include <qsettings.h>
#include <qfileinfo.h>
#include <qnetwork.h>


NYBOT::NYBOT ()
{
  pluginName = "NYBOT";
  helpFile = "nybot.html";
  cancelFlag = FALSE;
  plug = 0;
  date = QDateTime::currentDateTime();

  connect(this, SIGNAL(signalGetFileDone(bool)), this, SLOT(fileDone(bool)));
  connect(this, SIGNAL(signalTimeout()), this, SLOT(timeoutError()));

  QString plugin("Futures");  
  plug = config.getDbPlugin(plugin);
  if (! plug)
  {
    config.closePlugin(plugin);
    qDebug("NYBOT::cannot load Futures plugin");
  }

  loadSettings();
  qInitNetworkProtocols();
}

NYBOT::~NYBOT ()
{
  if (plug)
  {
    QString plugin("Futures");  
    config.closePlugin(plugin);
  }
}

void NYBOT::update ()
{
  if (! plug)
    return;
    
  plug->close();
  errorLoop = 0;  

  file = config.getData(Config::Home) + "/nybotDownload";

  // http://www.nybot.com/reports/dmrs/files/2005,05,13_ALL_futures.csv
  url = "http://www.nybot.com/reports/dmrs/files/";
  url.append(date.date().toString("yyyy,MM,dd"));
  url.append("_ALL_futures.csv");
  getFile(url);
}

void NYBOT::fileDone (bool d)
{
  if (d)
  {
    emit statusLogMessage("Network error.");
    emit done();
    return;
  }

  QFile f(file);
  if (! f.open(IO_WriteOnly))
  {
    emit statusLogMessage("Cant write to file.");
    emit done();
    return;
  }
  QTextStream stream(&f);
  
  stream << data;
  f.close();
  
  parse();
}

void NYBOT::timeoutError ()
{
  errorLoop++;
  if (errorLoop == retries)
  {
    emit statusLogMessage(tr("Timeout: retry limit"));
    emit done();
    return;
  }
  else
  {
    QString s = tr("Timeout: retry ") + QString::number(errorLoop + 1);
    emit statusLogMessage(s);
    getFile(url);
  }
}

void NYBOT::parse ()
{
  if (cancelFlag)
    return;
  
  QFile f(file);
  if (! f.open(IO_ReadOnly))
    return;
  QTextStream stream(&f);
    
  QString s = stream.readLine();
  s = stripJunk(s);
  QStringList keys = QStringList::split(",", s, FALSE);

  while(stream.atEnd() == 0)
  {
    s = stream.readLine();
    s = stripJunk(s);

    QStringList l = QStringList::split(",", s, FALSE);

    if (l.count() != keys.count())
      continue;

    Setting *data = new Setting;
    int loop2;
    for (loop2 = 0; loop2 < (int) keys.count(); loop2++)
      data->setData(keys[loop2], l[loop2]);

    // symbol
    QString symbol = data->getData("commoditySymbol");
    symbol = symbol.stripWhiteSpace();
    if (symbol.length() == 0)
    {
      delete data;
      continue;
    }

    if (! symbol.compare("CC") || ! symbol.compare("CR") || ! symbol.compare("CT") ||
        ! symbol.compare("DX") || ! symbol.compare("KC") || ! symbol.compare("OJ") ||
	! symbol.compare("SB") || ! symbol.compare("YX"))
    {
    }
    else
    {
      delete data;
      continue;
    }

    // open
    double open = 0;
    s = data->getData("dailyOpenPrice1");
    if (s.toFloat() == 0)
      s = data->getData("dailyOpenPrice2");
    if (setTFloat(s, FALSE))
    {
      delete data;
      continue;
    }
    else
      open = tfloat;

    // high
    double high = 0;
    s = data->getData("dailyHigh");
    if (setTFloat(s, FALSE))
    {
      delete data;
      continue;
    }
    else
      high = tfloat;

    // low
    double low = 0;
    s = data->getData("dailyLow");
    if (setTFloat(s, FALSE))
    {
      delete data;
      continue;
    }
    else
      low = tfloat;

    // close
    double close = 0;
    s = data->getData("settlementPrice");
    if (setTFloat(s, FALSE))
    {
      delete data;
      continue;
    }
    else
      close = tfloat;

    // volume
    double volume = 0;
    s = data->getData("tradeVolume");
    if (setTFloat(s, FALSE))
    {
      delete data;
      continue;
    }
    else
      volume = tfloat;

    // oi
    double oi = 0;
    s = data->getData("openInterest");
    if (setTFloat(s, FALSE))
    {
      delete data;
      continue;
    }
    else
      oi = tfloat;

    if (symbol.compare("CC"))
    {
      open = open / 100;
      high = high / 100;
      low = low / 100;
      close = close / 100;
    }

    // check for bad values
    if (close == 0)
    {
      delete data;
      continue;
    }

    if (open == 0 || high == 0 || low == 0 || volume == 0)
    {
      open = close;
      high = close;
      low = close;
    }

    if (open > high || open < low)
    {
      open = close;
      high = close;
      low = close;
    }

    if (close > high || close < low)
    {
      open = close;
      high = close;
      low = close;
    }

    // date
    QString date = data->getData("tradeDate");
    date.append("000000");

    //futures month
    s = data->getData("contractMonth");
    QString year = s.left(4);
    QString month = s.right(2);
    QString fmonth;

    delete data;

    switch (month.toInt())
    {
      case 1:
        fmonth = "F";
        break;
      case 2:
        fmonth = "G";
        break;
      case 3:
        fmonth = "H";
        break;
      case 4:
        fmonth = "J";
        break;
      case 5:
        fmonth = "K";
        break;
      case 6:
        fmonth = "M";
        break;
      case 7:
        fmonth = "N";
        break;
      case 8:
        fmonth = "Q";
        break;
      case 9:
        fmonth = "U";
        break;
      case 10:
        fmonth = "V";
        break;
      case 11:
        fmonth = "X";
        break;
      case 12:
        fmonth = "Z";
        break;
      default:
        break;
    }

    if (fd.setSymbol(symbol))
      continue;

    if (year.length())
    {
      symbol.append(year);

      if (fmonth.length())
        symbol.append(fmonth);
      else
        continue;
    }
    else
      continue;

    s = "Futures/" + fd.getExchange() + "/" + fd.getSymbol();
    QString path = createDirectory(s);
    if (! path.length())
    {
      emit statusLogMessage("Unable to create futures directory");
      return;
    }
	
    s = tr("Updating ") + symbol;
    emit statusLogMessage(s);

    Bar bar;
    if (bar.setDate(date))
    {
      emit statusLogMessage("Bad date " + date);
      continue;
    }

    s = path + "/" + symbol;
    if (plug->openChart(s))
    {
      emit statusLogMessage("Could not open db.");
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
        s = symbol + " - skipping update. Source does not match destination.";
        emit statusLogMessage(s);
        plug->close();
        return;
      }
    }

    plug->getHeaderField(DbPlugin::Symbol, s);
    if (! s.length())
    {
      plug->createNew();
      plug->setHeaderField(DbPlugin::Symbol, symbol);

      s = fd.getName();
      plug->setHeaderField(DbPlugin::Title, s);
	
      s = "FuturesType";
      QString s2 = fd.getSymbol();
      plug->setData(s, s2);
	
      s = "FuturesMonth";
      plug->setData(s, month);
    }

    bar.setOpen(open);
    bar.setHigh(high);
    bar.setLow(low);
    bar.setClose(close);
    bar.setVolume(volume);
    bar.setOI((int) oi);
    plug->setBar(bar);
		 
    //  emit dataLogMessage(symbol);
    plug->close();
  }

  f.close();

  emit done();
  if (cancelFlag)
  {
    cancelFlag = FALSE;
    emit statusLogMessage(tr("Update cancelled."));
  }
  else
    emit statusLogMessage(tr("Done"));
}

void NYBOT::prefDialog (QWidget *w)
{
  QString ds = tr("Date");
  QString ts = tr("Timeout");
  QString rs = tr("Retries");

  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("NYBOT Prefs"));
  QString pl = "Details";
  dialog->createPage (pl);

  dialog->addDateItem(ds, pl, date);
  dialog->addIntItem(rs, pl, retries, 0, 99);  
  dialog->addIntItem(ts, pl, timeout, 0, 99);  
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    date = dialog->getDate(ds);
    timeout = dialog->getInt(ts);
    retries = dialog->getInt(rs);
    
    saveFlag = TRUE;
    saveSettings();
  }
  
  delete dialog;
}

void NYBOT::cancelUpdate ()
{
  cancelFlag = TRUE;
}

void NYBOT::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/NYBOT plugin");
  
  QString s = settings.readEntry("/Retry", "3");
  retries = s.toInt();
  
  s = settings.readEntry("/Timeout", "15");
  timeout = s.toInt();
  
  settings.endGroup();
}

void NYBOT::saveSettings ()
{
  if (! saveFlag)
    return;

  QSettings settings;
  settings.beginGroup("/Qtstalker/NYBOT plugin");
  settings.writeEntry("/Retry", QString::number(retries));
  settings.writeEntry("/Timeout", QString::number(timeout));
  settings.endGroup();
}

//**********************************************************
//**********************************************************
//**********************************************************

QuotePlugin * createQuotePlugin ()
{
  NYBOT *o = new NYBOT;
  return ((QuotePlugin *) o);
}


