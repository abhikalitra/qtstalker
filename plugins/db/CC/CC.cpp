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
  dialog->addIntItem(tr("Maximum Years"), tr("Details"), getData("MaxYears").toInt());
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    setData("MaxYears", QString::number(dialog->getInt(tr("Maximum Years"))));
  }
  
  delete dialog;
}

void CC::setBar (Bar *bar)
{
  QStringList l;
  l.append(QString::number(bar->getOpen()));
  l.append(QString::number(bar->getHigh()));
  l.append(QString::number(bar->getLow()));
  l.append(QString::number(bar->getClose()));
  l.append(QString::number(bar->getVolume(), 'f', 0));
  l.append(QString::number(bar->getOI(), 'f', 0));
  setData(bar->getDate().getDateTimeString(FALSE), l.join(","));
}

void CC::saveDbDefaults (Setting *set)
{
  setData("Symbol", set->getData("Symbol"));
  setData("Type", "CC");
  setData("Title", set->getData("Title") + " - Continuous Adjusted");
  setData("BarType", set->getData("BarType"));
  setData("CCType", set->getData("Symbol"));
  setData("Plugin", "CC");
  setData("MaxYears", "10");
}

void CC::update ()
{
  Config config;
  QString baseDir = config.getData(Config::DataPath) + "/Futures/" + getData("Symbol");
  QDir dir(baseDir);
  if (! dir.exists(baseDir, TRUE))
    return;
    
  int maxYears = getData("MaxYears").toInt();
  
  FuturesData fd;
  if (fd.setSymbol(getData("Symbol")))
  {
    qDebug("CC::newChart:invalid futures symbol");
    return;
  }
  
  QDateTime startDate = QDateTime::currentDateTime();
  QDateTime endDate = QDateTime::currentDateTime();
  Bar *bar = getLastBar();
  if (! bar)
    startDate = startDate.addYears(-maxYears);
  else
  {
    startDate.setDate(bar->getDate().getDate());
    delete bar;
  }

  QString currentContract;
  ChartDb *db = 0;
  Bar *fr = 0;
  Bar *sr = 0;
  Bar *pr = new Bar;
    
  while (startDate <= endDate)
  {
    QString s = fd.getCurrentContract(startDate);
    if (s.compare(currentContract))
    {
      if (db)
        delete db;
	
      currentContract = s;
      s = baseDir + "/" + currentContract;
      db = new ChartDb;
      db->setPlugin("Futures");
      db->openChart(s);
      db->setBarCompression(BarData::DailyBar);
      fr = 0;

      while (! fr)
      {
        s = startDate.toString("yyyyMMdd000000");
        QString s2 = db->getData(s);
        if (s2.length())
          fr = db->getBar(s, s2);
	
        startDate = startDate.addDays(1);
        if (startDate.date().dayOfWeek() == 6)
          startDate = startDate.addDays(2);
      }
    }
    
    s = startDate.toString("yyyyMMdd000000");
    QString s2 = db->getData(s);
    if (s2.length())
    {
      sr = db->getBar(s, s2);
      if (sr)
      {
        double c = pr->getClose() + (sr->getClose() - fr->getClose());
        double h = c + (sr->getHigh() - sr->getClose());
        double l = c - (sr->getClose() - sr->getLow());
        double o = h - (sr->getHigh() - sr->getOpen());
	
	Bar *bar = new Bar;
	bar->setDate(sr->getDate());
	bar->setOpen(o);
	bar->setHigh(h);
	bar->setLow(l);
	bar->setClose(c);
	bar->setVolume(sr->getVolume());
	bar->setOI((int) sr->getOI());
        setBar(bar);
	delete bar;
        pr->setClose(c);
	delete fr;
	fr = sr;
      }
    }
      
    startDate = startDate.addDays(1);
    if (startDate.date().dayOfWeek() == 6)
      startDate = startDate.addDays(2);
  }

  delete pr;
  if (db)
    delete db;
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

