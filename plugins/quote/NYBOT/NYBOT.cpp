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
#include "DbPlugin.h"
#include "PrefDialog.h"
#include "Setting.h"
#include "Bar.h"
#include "Config.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qdir.h>
#include <qsettings.h>

NYBOT::NYBOT ()
{
  pluginName = "NYBOT";
  helpFile = "nybot.html";
  fd = new FuturesData;
  lastPath = QDir::homeDirPath();
  cancelFlag = FALSE;
  
  loadSettings();
}

NYBOT::~NYBOT ()
{
  delete fd;
}

void NYBOT::update ()
{
  QTimer::singleShot(250, this, SLOT(parse()));
}

void NYBOT::parse ()
{
  int loop;
  for (loop = 0; loop < (int) list.count(); loop++)
  {
    if (cancelFlag)
      break;
  
    QFile f(list[loop]);
    if (! f.open(IO_ReadOnly))
      continue;
    QTextStream stream(&f);
    
    QString s = stream.readLine();
    s = stripJunk(s);
    QStringList keys = QStringList::split(",", s, FALSE);

    while(stream.atEnd() == 0)
    {
      QString s = stream.readLine();
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

      if (fd->setSymbol(symbol))
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

      s = "Futures/" + fd->getExchange() + "/" + fd->getSymbol();
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
      
      Config config;
      QString plugin("Futures");
      DbPlugin *db = config.getDbPlugin(plugin);
      if (! db)
      {
        config.closePlugin(plugin);
        continue;
      }
      
      s = path + "/" + symbol;
      if (db->openChart(s))
      {
        emit statusLogMessage("Could not open db.");
        config.closePlugin(plugin);
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
          s = symbol + " - skipping update. Source does not match destination.";
          emit statusLogMessage(s);
          config.closePlugin(plugin);
          return;
        }
      }
      
      db->getHeaderField(DbPlugin::Symbol, s);
      if (! s.length())
      {
        db->createNew();
        db->setHeaderField(DbPlugin::Symbol, symbol);
	
	s = fd->getName();
        db->setHeaderField(DbPlugin::Title, s);
	
	s = "FuturesType";
	QString s2 = fd->getSymbol();
        db->setData(s, s2);
	
	s = "FuturesMonth";
        db->setData(s, month);
      }
      
      bar.setOpen(open);
      bar.setHigh(high);
      bar.setLow(low);
      bar.setClose(close);
      bar.setVolume(volume);
      bar.setOI((int) oi);
      db->setBar(bar);
		 
//      emit dataLogMessage(symbol);
      config.closePlugin(plugin);
    }

    f.close();
  }

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
  PrefDialog *dialog = new PrefDialog(w);
  dialog->setCaption(tr("NYBOT Prefs"));
  QString s = tr("Details");
  dialog->createPage (s);
  dialog->setHelpFile(helpFile);
  QString s2 = tr("File Input");
  dialog->addFileItem(s2, s, list, lastPath);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    s = tr("File Input");
    dialog->getFile(s, list);
    
    saveFlag = TRUE;
    
    if (list.count())
    {
      QFileInfo fi(list[0]);
      lastPath = fi.dirPath(TRUE);
      saveSettings();
    }
  }
  
  delete dialog;
}

void NYBOT::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/NYBOT plugin");
  lastPath = settings.readEntry("/lastPath", QDir::homeDirPath());
  settings.endGroup();
}

void NYBOT::saveSettings ()
{
  if (! saveFlag)
    return;

  QSettings settings;
  settings.beginGroup("/Qtstalker/NYBOT plugin");
  settings.writeEntry("/lastPath", lastPath);
  settings.endGroup();
}

void NYBOT::cancelUpdate ()
{
  cancelFlag = TRUE;
}

//**********************************************************
//**********************************************************
//**********************************************************

QuotePlugin * createQuotePlugin ()
{
  NYBOT *o = new NYBOT;
  return ((QuotePlugin *) o);
}


