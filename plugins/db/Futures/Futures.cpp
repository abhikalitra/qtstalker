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

#include "Futures.h"
#include "FuturesDialog.h"
#include <qtextstream.h>
#include <qfile.h>

Futures::Futures ()
{
  helpFile = "futuresplugin.html";
  recordSize = sizeof(FuturesRecord);
  memset(&record, 0, recordSize);
}

Futures::~Futures ()
{
}

void Futures::dbPrefDialog ()
{
  FuturesDialog *dialog = new FuturesDialog(path, helpFile);
  dialog->exec();
  delete dialog;
}

void Futures::saveDbDefaults (Setting *set)
{
  strncpy(header->symbol, set->getData("Symbol").ascii(), SSIZE);
  strncpy(header->type, (char *) "Futures", SSIZE);
  strncpy(header->title, set->getData("Title").ascii(), TITLESIZE);
  header->barType = set->getInt("BarType");
  strncpy(header->plugin, (char *) "Futures", SSIZE);
  strncpy(header->futuresType, set->getData("FuturesType").ascii(), SSIZE);
  strncpy(header->futuresMonth, set->getData("FuturesMonth").ascii(), SSIZE);
  saveFlag = TRUE;
}

void Futures::dump (QString d)
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

void Futures::deleteBar (QString d)
{
  if (! findRecord(d))
    return;
    
  memset(&record, 0, recordSize);
  record.date = d.toDouble();
  fwrite(&record, recordSize, 1, db);
}

int Futures::readRecord ()
{
  return fread(&record, recordSize, 1, db);
}

int Futures::writeRecord ()
{
  return fwrite(&record, recordSize, 1, db);
}

bool Futures::getRecordState ()
{
  return record.state;
}

void Futures::fillBar (Bar *bar)
{
  bar->setDate(QString::number(record.date, 'f', 0));
  bar->setOpen(record.open);
  bar->setHigh(record.high);
  bar->setLow(record.low);
  bar->setClose(record.close);
  bar->setVolume(record.volume);
  bar->setOI(record.oi);
}

double Futures::getRecordDate ()
{
  return record.date;
}

void Futures::fillRecord (Bar *bar)
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

void Futures::setRecordDate (double d)
{
  record.date = d;
}

void Futures::clearRecord ()
{
  memset(&record, 0, recordSize);
}

int Futures::writeTempRecord ()
{
  return fwrite(&record, recordSize, 1, tdb);
}

void Futures::setBarString (QString d)
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

//********************************************************************
//********************************************************************
//********************************************************************

DbPlugin * createDbPlugin ()
{
  Futures *o = new Futures;
  return ((DbPlugin *) o);
}

