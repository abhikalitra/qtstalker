/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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

#include "CC.h"
#include "ChartDb.h"
#include "FuturesData.h"
#include "PrefDialog.h"
#include "Config.h"
#include <qstring.h>
#include <qdir.h>
#include <qinputdialog.h>
#include <qmessagebox.h>

CC::CC ()
{
}

CC::~CC ()
{
}

BarData * CC::getHistory ()
{
  update();
  return DbPlugin::getHistory();
}

Bar * CC::getBar (QString k, QString d)
{
  QStringList l = QStringList::split(",", d, FALSE);
  Bar *bar = new Bar;
  bar->setDate(k);
  bar->setData("Open", l[0].toDouble());
  bar->setData("High", l[1].toDouble());
  bar->setData("Low", l[2].toDouble());
  bar->setData("Close", l[3].toDouble());
  bar->setData("Volume", l[4].toDouble());
  bar->setData("OI", l[5].toDouble());
  return bar;
}

void CC::dbPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(tr("CC Prefs"));
  dialog->createPage (tr("Details"));
  dialog->addIntItem(tr("Rollover"), tr("Details"), getData("Rollover").toInt());
  dialog->addIntItem(tr("Maximum Years"), tr("Details"), getData("MaxYears").toInt());
  dialog->addCheckItem(tr("Rebuild"), tr("Details"), getData("Rebuild").toInt());
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    setData("Rollover", QString::number(dialog->getInt(tr("Rollover"))));
    setData("MaxYears", QString::number(dialog->getInt(tr("Maximum Years"))));
    setData("Rebuild", QString::number(dialog->getCheck(tr("Rebuild"))));
  }
  
  delete dialog;
}

void CC::setBar (BarDate date, double open, double high, double low,
                 double close, double volume, double oi)
{
  QStringList l;
  l.append(QString::number(open));
  l.append(QString::number(high));
  l.append(QString::number(low));
  l.append(QString::number(close));
  l.append(QString::number(volume, 'f', 0));
  l.append(QString::number(oi, 'f', 0));
  setData(date.getDateTimeString(FALSE), l.join(","));
}

void CC::saveDbDefaults (BarData::BarType barType, QString symbol, QString name, QString,
                         QString, QString, QString)
{
  setData("Symbol", symbol);
  setData("Type", "CC");
  setData("Title", name + " - Continuous Adjusted");
  setData("BarType", QString::number(barType));
  setData("CCType", symbol);
  setData("Plugin", "CC");
  setData("Rollover", "20");
  setData("MaxYears", "10");
}

void CC::update ()
{
  Config config;
  QString s = config.getData(Config::DataPath) + "/Futures/" + getData("Symbol");
  QDir dir(s);
  if (! dir.exists(s, TRUE))
    return;
    
  int rollover = getData("Rollover").toInt();
  int maxYears = getData("MaxYears").toInt();

  QDate tdate = QDate::currentDate();
  int years = tdate.year() - maxYears;

  FuturesData fd;
  if (fd.setSymbol(getData("Symbol")))
  {
    qDebug("CC::newChart:invalid futures symbol");
    return;
  }
  QString currentContract = fd.getContract();

  Bar *pr = new Bar;
  
  BarDate startDate;
  
  bool flag = FALSE;
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    s = dir[loop];
    s.truncate(s.length() - 1);
    if (s.right(4).toInt() < years)
      continue;

    if (! dir[loop].compare(currentContract))
      flag = TRUE;

    s = dir.absPath();
    s.append("/");
    s.append(dir[loop]);

    ChartDb *tdb = new ChartDb();
    tdb->openChart(s);
    
    Bar *bar = tdb->getFirstBar();
    if (! bar)
    {
      qDebug("CC::newChart:no first bar %s", dir[loop].latin1());
      delete tdb;
      continue;
    }
    if (! startDate.getDate().isValid())
      startDate = bar->getDate();
    delete bar;

    tdb->setBarCompression(BarData::DailyBar);
    tdb->setBarRange(260);
    BarData *recordList = tdb->getHistory();

    int loop2;
    int count = recordList->count() - rollover;
    if (flag)
      count = recordList->count();
    if (count < 1)
    {
      delete recordList;
      delete tdb;
      continue;
    }

    for (loop2 = 1; loop2 < count; loop2++)
    {
      if (recordList->getDate(loop2).getDate() < startDate.getDate())
        continue;
	
      if (! pr->getDate().getDate().isValid())
        pr->setClose(recordList->getClose(loop2 - 1));

      double c = pr->getClose() + (recordList->getClose(loop2) - recordList->getClose(loop2 - 1));
      double h = c + (recordList->getHigh(loop2) - recordList->getClose(loop2));
      double l = c - (recordList->getClose(loop2) - recordList->getLow(loop2));
      double o = h - (recordList->getHigh(loop2) - recordList->getOpen(loop2));

      delete pr;

      pr = new Bar;
      pr->setDate(recordList->getDate(loop2));
      pr->setOpen(o);
      pr->setHigh(h);
      pr->setLow(l);
      pr->setClose(c);
      pr->setVolume(recordList->getVolume(loop2));
      pr->setOI((int) recordList->getOI(loop2));
      
      setBar(pr->getDate(), pr->getOpen(), pr->getHigh(), pr->getLow(), pr->getClose(),
             pr->getVolume(), pr->getOI());
      
      startDate = recordList->getDate(loop2);
    }

    delete recordList;
    delete tdb;

    if (flag)
      break;
  }

  delete pr;
}

QString CC::createNew ()
{
  FuturesData fd;
  bool ok = FALSE;
  QString symbol = QInputDialog::getItem(tr("New CC"),
                                         tr("Select futures symbol to create."),
					 fd.getSymbolList(),
					 0,
					 FALSE,
					 &ok,
					 0);
  if (! symbol.length() || ok == FALSE)
    return QString();

  QDir dir;
  Config config;
  QString s = config.getData(Config::DataPath) + "/CC";
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
    {
      QMessageBox::information(0,
                               tr("Qtstalker: Error"),
			       tr("Could not create ~/Qtstalker/data/CC directory."));
      return QString();
    }
  }
  
  s.append("/" + symbol);
  if (dir.exists(s))
  {
    QMessageBox::information(0,
                             tr("Qtstalker: Error"),
			     tr("This CC already exists."));
    return QString();
  }
  
  return s;
}

Plugin * create ()
{
  CC *o = new CC;
  return ((Plugin *) o);
}

