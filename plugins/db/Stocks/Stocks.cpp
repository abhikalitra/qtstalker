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

#include "Stocks.h"
#include "StocksDialog.h"
#include <qtextstream.h>
#include <qfile.h>

Stocks::Stocks ()
{
  helpFile = "stocksplugin.html";
  recordSize = sizeof(StockRecord);
  memset(&record, 0, recordSize);
}

Stocks::~Stocks ()
{
}

void Stocks::dbPrefDialog ()
{
  StocksDialog *dialog = new StocksDialog(path, helpFile);
  dialog->exec();
  delete dialog;
}

void Stocks::saveDbDefaults (Setting *set)
{
  strncpy(header->symbol, set->getData("Symbol").ascii(), SSIZE);
  strncpy(header->type, (char *) "Stock", SSIZE);
  strncpy(header->title, set->getData("Title").ascii(), TITLESIZE);
  header->barType = set->getInt("BarType");
  strncpy(header->plugin, (char *) "Stocks", SSIZE);
  saveFlag = TRUE;
}

void Stocks::dump (QString d, bool f)
{
  QFile outFile(d);
  if (! outFile.open(IO_WriteOnly))
    return;
  QTextStream outStream(&outFile);
  
  if (! f)
    dumpHeader(outStream);

  fseek(db, sizeof(ChartHeader), SEEK_SET);
  while (fread(&record, recordSize, 1, db))
  {
    if (! record.state)
      continue;
  
    outStream << QString::number(record.date, 'f', 0) << ",";
    outStream << QString::number(record.open, 'f', 2) << ",";
    outStream << QString::number(record.high, 'f', 2) << ",";
    outStream << QString::number(record.low, 'f', 2) << ",";
    outStream << QString::number(record.close, 'f', 2) << ",";
    outStream << QString::number(record.volume, 'f', 0) << "\n";
  }  

  outFile.close();
}

void Stocks::deleteBar (QString d)
{
  if (! findRecord(d))
    return;
    
  fseek(db, -recordSize, SEEK_CUR);
  memset(&record, 0, recordSize);
  record.date = d.toDouble();
  fwrite(&record, recordSize, 1, db);
}

int Stocks::readRecord ()
{
  return fread(&record, recordSize, 1, db);
}

int Stocks::writeRecord ()
{
  return fwrite(&record, recordSize, 1, db);
}

bool Stocks::getRecordState ()
{
  return record.state;
}

void Stocks::fillBar (Bar *bar)
{
  bar->setDate(QString::number(record.date, 'f', 0));
  bar->setOpen(record.open);
  bar->setHigh(record.high);
  bar->setLow(record.low);
  bar->setClose(record.close);
  bar->setVolume(record.volume);
}

double Stocks::getRecordDate ()
{
  return record.date;
}

void Stocks::fillRecord (Bar *bar)
{
  record.state = TRUE;
  record.date = bar->getDate().getDateValue();
  record.open = bar->getOpen();  
  record.high = bar->getHigh();  
  record.low = bar->getLow();
  record.close = bar->getClose();  
  record.volume = bar->getVolume();  
}

void Stocks::setRecordDate (double d)
{
  record.date = d;
}

void Stocks::clearRecord ()
{
  memset(&record, 0, recordSize);
}

int Stocks::writeTempRecord ()
{
  return fwrite(&record, recordSize, 1, tdb);
}

void Stocks::setBarString (QString d)
{
  QStringList l = QStringList::split(",", d, FALSE);
  if (l.count() < 6)
    return;
  
  Bar *bar = new Bar;
  bar->setDate(l[0]);
  bar->setOpen(l[1].toDouble());
  bar->setHigh(l[2].toDouble());
  bar->setLow(l[3].toDouble());
  bar->setClose(l[4].toDouble());
  bar->setVolume(l[5].toDouble());
  
  bar->setTickFlag(header->barType);
  
  setBar(bar);
  delete bar;
}

//********************************************************************
//********************************************************************
//********************************************************************

DbPlugin * createDbPlugin ()
{
  Stocks *o = new Stocks;
  return ((DbPlugin *) o);
}
