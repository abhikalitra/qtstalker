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
      QString open = data->getData("dailyOpenPrice1");
      if (open.toFloat() == 0)
        open = data->getData("dailyOpenPrice2");
      if (setTFloat(open))
      {
        delete data;
        continue;
      }
      else
        open = QString::number(tfloat);

      // high
      QString high;
      if (setTFloat(data->getData("dailyHigh")))
      {
        delete data;
        continue;
      }
      else
        high = QString::number(tfloat);

      // low
      QString low;
      if (setTFloat(data->getData("dailyLow")))
      {
        delete data;
        continue;
      }
      else
        low = QString::number(tfloat);

      // close
      QString close;
      if (setTFloat(data->getData("settlementPrice")))
      {
        delete data;
        continue;
      }
      else
        close = QString::number(tfloat);

      // volume
      QString volume;
      if (setTFloat(data->getData("tradeVolume")))
      {
        delete data;
        continue;
      }
      else
        volume = QString::number(tfloat);

      // oi
      QString oi;
      if (setTFloat(data->getData("openInterest")))
      {
        delete data;
        continue;
      }
      else
        oi = QString::number(tfloat);

      if (symbol.compare("CC"))
      {
        open = QString::number(open.toFloat() / 100);
        high = QString::number(high.toFloat() / 100);
        low = QString::number(low.toFloat() / 100);
        close = QString::number(close.toFloat() / 100);
      }

      // check for bad values
      if (close.toFloat() == 0)
      {
        delete data;
        continue;
      }

      if (open.toFloat() == 0 || high.toFloat() == 0 || low.toFloat() == 0 || volume.toFloat() == 0)
      {
        open = close;
	high = close;
	low = close;
      }

      if (open.toFloat() > high.toFloat() || open.toFloat() < low.toFloat())
      {
        open = close;
	high = close;
	low = close;
      }

      if (close.toFloat() > high.toFloat() || close.toFloat() < low.toFloat())
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

      QString path = createDirectory("Futures");
      if (! path.length())
      {
        emit statusLogMessage(tr("Unable to create futures directory"));
        return;
      }

      s = "Futures/";
      s.append(fd->getSymbol());
      path = createDirectory(s);
      if (! path.length())
      {
        emit statusLogMessage(tr("Unable to create directory"));
        return;
      }

      s = tr("Updating ") + symbol;
      emit statusLogMessage(s);
      
      Bar *bar = new Bar;
      if (bar->setDate(date))
      {
        emit statusLogMessage("Bad date " + date);
	delete bar;
        continue;
      }
      
      Config config;
      DbPlugin *db = config.getDbPlugin("Futures");
      if (! db)
      {
        config.closePlugin("Futures");
	delete bar;
        continue;
      }
      s = path;
      s.append("/");
      s.append(symbol);
      if (db->openChart(s))
      {
        emit statusLogMessage("Could not open db.");
        config.closePlugin("Futures");
	delete bar;
        return;
      }

      s = db->getHeaderField(DbPlugin::Symbol);
      if (! s.length())
      {
        db->createNew();
        db->setHeaderField(DbPlugin::Symbol, symbol);
        db->setHeaderField(DbPlugin::Title, fd->getName());
        db->setData("FuturesType", fd->getSymbol());
        db->setData("FuturesMonth", month);
      }
      
      bar->setOpen(open.toDouble());
      bar->setHigh(high.toDouble());
      bar->setLow(low.toDouble());
      bar->setClose(close.toDouble());
      bar->setVolume(volume.toDouble());
      bar->setOI(oi.toInt());
      db->setBar(bar);
      delete bar;
		 
//      emit dataLogMessage(symbol);
      config.closePlugin("Futures");
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
  dialog->createPage (tr("Details"));
  dialog->setHelpFile(helpFile);
  dialog->addFileItem(tr("File Input"), tr("Details"), list, lastPath);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    list = dialog->getFile(tr("File Input"));
    
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


