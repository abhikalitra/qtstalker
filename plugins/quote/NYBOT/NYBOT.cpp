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
#include "ChartDb.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qdir.h>

NYBOT::NYBOT ()
{
  pluginName = "NYBOT";
  fd = new FuturesData;
  createFlag = FALSE;

  set(tr("Input"), "", Setting::FileList);

  about = "Imports quotes downloaded previously from the NYBOT site.\n";
  about.append("NOTE: Only quotes downloaded manually can be imported.\n");
  about.append("      The quotes are created dynamically, no static content\n");
  about.append("      for scripted downloads. Oh well, still better then nothing.");
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
  QStringList list = getList(tr("Input"));

  int loop;
  for (loop = 0; loop < (int) list.count(); loop++)
  {
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
        data->set(keys[loop2], l[loop2], Setting::None);

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
        qDebug("NYBOT plugin: Unable to create futures directory");
        return;
      }

      s = "Futures/";
      s.append(fd->getSymbol());
      path = createDirectory(s);
      if (! path.length())
      {
        qDebug("NYBOT plugin: Unable to create directory");
        return;
      }

      s = tr("Updating ");
      s.append(fd->getSymbol());
      emit message(s);

      Setting *r = new Setting;
      r->set("Date", date, Setting::Date);
      r->set("Open", open, Setting::Float);
      r->set("High", high, Setting::Float);
      r->set("Low", low, Setting::Float);
      r->set("Close", close, Setting::Float);
      r->set("Volume", volume, Setting::Float);
      r->set("Open Interest", oi, Setting::Float);

      s = path;
      s.append("/");
      s.append(symbol);
      ChartDb *db = new ChartDb();
      db->openChart(s);

      Setting *details = db->getDetails();
      if (! details->count())
      {
        details->set("Format", "Open|High|Low|Close|Volume|Open Interest", Setting::None);
        details->set("Chart Type", tr("Futures"), Setting::None);
        details->set("Symbol", symbol, Setting::None);
        details->set("Source", pluginName, Setting::None);
        details->set("Futures Month", month, Setting::None);
        details->set("Futures Type", fd->getSymbol(), Setting::None);
        details->set("Title", fd->getName(), Setting::Text);
        db->setFormat();
      }
      db->setRecord(r);
      delete db;
      delete r;
    }

    f.close();
  }

  emit done();
}

Plugin * create ()
{
  NYBOT *o = new NYBOT;
  return ((Plugin *) o);
}


