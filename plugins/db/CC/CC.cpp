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
#include <qobject.h>

CC::CC ()
{
  helpFile = "cc.html";
  recordSize = sizeof(CCRecord);
  memset(&record, 0, recordSize);
}

CC::~CC ()
{
}

BarData * CC::getHistory ()
{
  update();
  return DbPlugin::getHistory();
}

void CC::dbPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("CC Prefs"));
  dialog->createPage (QObject::tr("Details"));
  dialog->setHelpFile (helpFile);
  dialog->addIntItem(QObject::tr("Maximum Years"), QObject::tr("Details"), header->int1);
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    header->int1 = dialog->getInt(QObject::tr("Maximum Years"));
    saveFlag = TRUE;
  }
  
  delete dialog;
}

void CC::saveDbDefaults (Setting *set)
{
  strncpy(header->symbol, set->getData("Symbol").ascii(), SSIZE);
  strncpy(header->type, (char *) "CC", SSIZE);
  
  QString s = set->getData("Title") + " - Continuous Adjusted";
  strncpy(header->title, s.ascii(), TITLESIZE);
  
  header->barType = set->getInt("BarType");
  strncpy(header->plugin, (char *) "CC", SSIZE);
  header->int1 = 10; // MAXYEARS
  
  saveFlag = TRUE;
}

void CC::update ()
{
  Config config;
  QString baseDir = config.getData(Config::DataPath) + "/Futures/" + header->symbol;
  QDir dir(baseDir);
  if (! dir.exists(baseDir, TRUE))
    return;
    
  int maxYears = header->int1;
  
  FuturesData fd;
  if (fd.setSymbol(header->symbol))
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
  ChartDb *tdb = 0;
  Bar *fr = 0;
  Bar *sr = 0;
  Bar *pr = new Bar;

  while (startDate <= endDate)
  {
    QString s = fd.getCurrentContract(startDate);
    if (s.compare(currentContract))
    {
      if (tdb)
        delete tdb;
	
      QString s2 = baseDir + "/" + s;
      if (! dir.exists(s2))
      {
        startDate = startDate.addDays(1);
        if (startDate.date().dayOfWeek() == 6)
          startDate = startDate.addDays(2);
        continue;
      }
	
      currentContract = s;
      
      tdb = new ChartDb;
      tdb->setPlugin("Futures");
      tdb->openChart(s2);
      tdb->setBarCompression(BarData::DailyBar);
      fr = 0;

      while (! fr)
      {
        s = startDate.toString("yyyyMMdd000000");
	Bar *tbar = tdb->getBar(s);
        if (tbar)
          fr = tbar;
	
        startDate = startDate.addDays(1);
        if (startDate.date().dayOfWeek() == 6)
          startDate = startDate.addDays(2);
      }
    }
    
    s = startDate.toString("yyyyMMdd000000");
    Bar *tbar = tdb->getBar(s);
    if (tbar)
    {
      sr = tbar;
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
  if (tdb)
    delete tdb;
}

QString CC::createNew ()
{
  FuturesData fd;
  bool ok = FALSE;
  QString symbol = QInputDialog::getItem(QObject::tr("New CC"),
                                         QObject::tr("Select futures symbol to create."),
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
                               QObject::tr("Qtstalker: Error"),
			       QObject::tr("Could not create ~/Qtstalker/data/CC directory."));
      return QString();
    }
  }
  
  s.append("/" + symbol);
  if (dir.exists(s))
  {
    QMessageBox::information(0,
                             QObject::tr("Qtstalker: Error"),
			     QObject::tr("This CC already exists."));
    return QString();
  }
  
  return s;
}

void CC::dump (QString d)
{
  QFile outFile(d);
  if (! outFile.open(IO_WriteOnly))
    return;
  QTextStream outStream(&outFile);
  
  dumpHeader(outStream);

  fseek(db, sizeof(ChartHeader), SEEK_SET);
  while (fread(&record, recordSize, 1, db))
  {
    if (! record.state)
      continue;
  
    outStream << QString::number(record.date, 'f', 0) << ",";
    outStream << QString::number(record.open, 'g', 4) << ",";
    outStream << QString::number(record.high, 'g', 4) << ",";
    outStream << QString::number(record.low, 'g', 4) << ",";
    outStream << QString::number(record.close, 'g', 4) << ",";
    outStream << QString::number(record.volume, 'f', 0) << ",";
    outStream << QString::number(record.oi) << "\n";
  }  

  outFile.close();
}

void CC::deleteBar (QString d)
{
  if (! findRecord(d))
    return;
    
  fseek(db, -recordSize, SEEK_CUR);
  memset(&record, 0, recordSize);
  record.date = d.toDouble();
  fwrite(&record, recordSize, 1, db);
}

int CC::readRecord ()
{
  return fread(&record, recordSize, 1, db);
}

int CC::writeRecord ()
{
  return fwrite(&record, recordSize, 1, db);
}

bool CC::getRecordState ()
{
  return record.state;
}

void CC::fillBar (Bar *bar)
{
  bar->setDate(QString::number(record.date, 'f', 0));
  bar->setOpen(record.open);
  bar->setHigh(record.high);
  bar->setLow(record.low);
  bar->setClose(record.close);
  bar->setVolume(record.volume);
  bar->setOI(record.oi);
}

double CC::getRecordDate ()
{
  return record.date;
}

void CC::fillRecord (Bar *bar)
{
  record.state = TRUE;
  record.date = bar->getDate().getDateValue();
  record.open = bar->getOpen();  
  record.high = bar->getHigh();  
  record.low = bar->getLow();
  record.close = bar->getClose();  
  record.volume = bar->getVolume();  
  record.oi = (int) bar->getOI();  
}

void CC::setRecordDate (double d)
{
  record.date = d;
}

void CC::clearRecord ()
{
  memset(&record, 0, recordSize);
}

int CC::writeTempRecord ()
{
  return fwrite(&record, recordSize, 1, tdb);
}

void CC::setBarString (QString d)
{
  QStringList l = QStringList::split(",", d, FALSE);
  if (l.count() < 7)
    return;
  
  Bar *bar = new Bar;
  bar->setDate(l[0]);
  bar->setOpen(l[1].toDouble());
  bar->setHigh(l[2].toDouble());
  bar->setLow(l[3].toDouble());
  bar->setClose(l[4].toDouble());
  bar->setVolume(l[5].toDouble());
  bar->setOI(l[6].toInt());
  
  setBar(bar);
  delete bar;
}

//***********************************************************
//***********************************************************
//***********************************************************

DbPlugin * createDbPlugin ()
{
  CC *o = new CC;
  return ((DbPlugin *) o);
}

