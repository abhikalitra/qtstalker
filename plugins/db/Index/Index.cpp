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

#include "Index.h"
#include "IndexDialog.h"
#include "Config.h"
#include "ChartDb.h"
#include "Bar.h"
#include <qdir.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qobject.h>
#include <qtextstream.h>
#include <qfile.h>

Index::Index ()
{
  data.setAutoDelete(TRUE);
  helpFile = "indexes.html";
  recordSize = sizeof(IndexRecord);
  memset(&record, 0, recordSize);
}

Index::~Index ()
{
}

BarData * Index::getHistory ()
{
  updateIndex();
  return DbPlugin::getHistory();
}

void Index::dbPrefDialog ()
{
  IndexDialog *dialog = new IndexDialog(helpFile);
  dialog->setList(header->lvar1);
  dialog->setRebuild(header->bool1);
  dialog->setName(header->symbol);
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    QString s = dialog->getList();
    if (s.length() < LSIZE)
      strncpy(header->lvar1, s.ascii(), LSIZE);
    else
    {
      QMessageBox::warning(0,
                           QObject::tr("Qtstalker: Error"),
			   QObject::tr("Too many Index items. Out of strorage room.\nRemove item(s) and try again."));
      delete dialog;
      return;		
    }
    
    header->bool1 = dialog->getRebuild();
    
    saveFlag = TRUE;
    updateIndex();
  }
  
  delete dialog;
}

void Index::updateIndex ()
{
  data.clear();
  fdate = 99999999999999.0;
  ldate = 0;
  
  QStringList l = QStringList::split(":", header->lvar1, FALSE);
  if (! l.count())
    return;
    
  int loop;
  for (loop = 0; loop < (int) l.count(); loop = loop + 2)
  {
    QString symbol = l[loop];
    float weight = l[loop + 1].toFloat();
    if (weight == 0)
      weight = 1;

    loadData(symbol, weight);
  }

  Bar *r = data.find(QString::number(fdate, 'f', 0));
  if (r)
    setBar(r);
  
  r = data.find(QString::number(ldate, 'f', 0));
  if (r)
    setBar(r);
  
  int count = l.count() / 2;
  QDictIterator<Bar> it(data);
  for (; it.current(); ++it)
  {
    r = it.current();
    if (r->getData("Count") == count)
    {
      r->setOpen(r->getOpen() / count);
      r->setHigh(r->getHigh() / count);
      r->setLow(r->getLow() / count);
      r->setClose(r->getClose() / count);
      
      if (r->getOpen() > r->getHigh())
	r->setHigh(r->getOpen());
      if (r->getOpen() < r->getLow())
	r->setLow(r->getOpen());

      if (r->getClose() > r->getHigh())
	r->setHigh(r->getClose());
      if (r->getClose() < r->getLow())
	r->setLow(r->getClose());
	
      setBar(r);
    }
  }

  data.clear();
}

void Index::loadData (QString symbol, float weight)
{
  ChartDb *db = new ChartDb;
  if (db->openChart(symbol))
  {
    qDebug("Index::loadData: can't open db");
    delete db;
    return;
  }
  
  db->setBarCompression(BarData::DailyBar);
  
  bool rebuild = header->bool1;
  if (rebuild)
    db->setBarRange(99999999);
  else
  {
    Bar *bar = getLastBar();
    if (bar)
    {
      QDate d = QDate::currentDate();
      db->setBarRange(bar->getDate().getDate().daysTo(d));
      delete bar;
    }
    else
      db->setBarRange(99999999);
  }

  BarData *recordList = db->getHistory();

  int loop;
  for (loop = 0; loop < (int) recordList->count(); loop++)
  {
    Bar *r = data.find(recordList->getDate(loop).getDateTimeString(FALSE));
    if (! r)
    {
      r = new Bar;
      r->setDate(recordList->getDate(loop));
      r->setOpen(recordList->getOpen(loop) * weight);
      r->setHigh(recordList->getHigh(loop) * weight);
      r->setLow(recordList->getLow(loop) * weight);
      r->setClose(recordList->getClose(loop) * weight);
      r->setData("Count", 1);
      data.insert(r->getDate().getDateTimeString(FALSE), r);
      
      if (r->getDate().getDateValue() > ldate)
        ldate = r->getDate().getDateValue();
	
      if (r->getDate().getDateValue() < fdate)
        fdate = r->getDate().getDateValue();
    }
    else
    {
      r->setOpen(r->getOpen() + (recordList->getOpen(loop) * weight));
      r->setHigh(r->getHigh() + (recordList->getHigh(loop) * weight));
      r->setLow(r->getLow() + (recordList->getLow(loop) * weight));
      r->setClose(r->getClose() + (recordList->getClose(loop) * weight));
      r->setData("Count", r->getData("Count") + 1);
    }
  }

  delete db;
}

QString Index::createNew ()
{
  bool ok = FALSE;
  QString index = QInputDialog::getText(QObject::tr("New Index"),
                                        QObject::tr("Enter symbol name for the new Index"),
					QLineEdit::Normal,
					QString::null,
					&ok,
					0);
  if (! index.length() || ok == FALSE)
    return QString();

  QDir dir;
  Config config;
  QString s = config.getData(Config::DataPath) + "/Index";
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
    {
      QMessageBox::information(0,
                               QObject::tr("Qtstalker: Error"),
			       QObject::tr("Could not create ~/Qtstalker/data/Index directory."));
      return QString();
    }
  }
  
  s.append("/" + index);
  if (dir.exists(s))
  {
    QMessageBox::information(0,
                             QObject::tr("Qtstalker: Error"),
			     QObject::tr("This Index already exists."));
    return QString();
  }
  
  return s;
}

void Index::saveDbDefaults (Setting *set)
{
  strncpy(header->symbol, set->getData("Symbol").ascii(), SSIZE);
  strncpy(header->type, (char *) "Index", SSIZE);
  strncpy(header->title, set->getData("Title").ascii(), TITLESIZE);
  header->barType = set->getData("BarType").toInt();
  strncpy(header->plugin, (char *) "Index", SSIZE);
  saveFlag = TRUE;
}

void Index::dump (QString d)
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
    outStream << QString::number(record.close, 'g', 4) << "\n";
  }  

  outFile.close();
}

void Index::deleteBar (QString d)
{
  if (! findRecord(d))
    return;
    
  fseek(db, -recordSize, SEEK_CUR);
  memset(&record, 0, recordSize);
  record.date = d.toDouble();
  fwrite(&record, recordSize, 1, db);
}

int Index::readRecord ()
{
  return fread(&record, recordSize, 1, db);
}

int Index::writeRecord ()
{
  return fwrite(&record, recordSize, 1, db);
}

bool Index::getRecordState ()
{
  return record.state;
}

void Index::fillBar (Bar *bar)
{
  bar->setDate(QString::number(record.date, 'f', 0));
  bar->setOpen(record.open);
  bar->setHigh(record.high);
  bar->setLow(record.low);
  bar->setClose(record.close);
}

double Index::getRecordDate ()
{
  return record.date;
}

void Index::fillRecord (Bar *bar)
{
  record.state = TRUE;
  record.date = bar->getDate().getDateValue();
  record.open = bar->getOpen();  
  record.high = bar->getHigh();  
  record.low = bar->getLow();
  record.close = bar->getClose();  
}

void Index::setRecordDate (double d)
{
  record.date = d;
}

void Index::clearRecord ()
{
  memset(&record, 0, recordSize);
}

int Index::writeTempRecord ()
{
  return fwrite(&record, recordSize, 1, tdb);
}

void Index::setBarString (QString d)
{
  QStringList l = QStringList::split(",", d, FALSE);
  if (l.count() < 5)
    return;
  
  Bar *bar = new Bar;
  bar->setDate(l[0]);
  bar->setOpen(l[1].toDouble());
  bar->setHigh(l[2].toDouble());
  bar->setLow(l[3].toDouble());
  bar->setClose(l[4].toDouble());
  
  setBar(bar);
  delete bar;
}

//********************************************************************
//********************************************************************
//********************************************************************

DbPlugin * createDbPlugin ()
{
  Index *o = new Index;
  return ((DbPlugin *) o);
}

