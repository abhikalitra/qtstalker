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

#include "Spread.h"
#include "PrefDialog.h"
#include "ChartDb.h"
#include "Config.h"
#include "Bar.h"
#include <qdir.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qobject.h>
#include <qtextstream.h>
#include <qfile.h>

Spread::Spread ()
{
  data.setAutoDelete(TRUE);
  helpFile = "spread.html";
  recordSize = sizeof(SpreadRecord);
  memset(&record, 0, recordSize);
}

Spread::~Spread ()
{
}

BarData * Spread::getHistory ()
{
  updateSpread();
  return DbPlugin::getHistory();
}

void Spread::dbPrefDialog ()
{
  Config config;
  QStringList l;
  l.append(QObject::tr("Subtract"));
  l.append(QObject::tr("Divide"));
  
  QString s = config.getData(Config::DataPath);
  
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("Spread Prefs"));
  dialog->createPage (QObject::tr("Details"));
  dialog->setHelpFile (helpFile);
  dialog->addSymbolItem(QObject::tr("First Symbol"), QObject::tr("Details"), s, header->mvar1); // First Symbol
  dialog->addSymbolItem(QObject::tr("Second Symbol"), QObject::tr("Details"), s, header->lvar1); // Second Symbol
  dialog->addComboItem(QObject::tr("Method"), QObject::tr("Details"), l, header->svar1); // Method
  dialog->addCheckItem(QObject::tr("Rebuild"), QObject::tr("Details"), header->bool1); // Rebuild
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    QString s = dialog->getSymbol(QObject::tr("First Symbol"));
    if (s.length())
      strncpy(header->mvar1, s.ascii(), MSIZE);
      
    s = dialog->getSymbol(QObject::tr("Second Symbol"));
    if (s.length())
      strncpy(header->lvar1, s.ascii(), MSIZE);
      
    strncpy(header->svar1, dialog->getCombo(QObject::tr("Method")), SSIZE);
    header->bool1 = dialog->getCheck(QObject::tr("Rebuild"));

    saveFlag = TRUE;
  }
  
  delete dialog;
}

void Spread::updateSpread ()
{
  data.clear();
  fdate = 99999999999999.0;
  
  QString fs = header->mvar1;
  if (! fs.length())
    return;
    
  QString ss = header->lvar1;
  if (! ss.length())
    return;
  
  QString meth = header->svar1;

  loadData(fs, meth);

  loadData(ss, meth);

  Bar *r = data.find(QString::number(fdate, 'f', 0));
  if (r)
  {
    setBar(r);
    
    if (r->getData("Count") != 2)
      deleteBar(QString::number(fdate, 'f', 0));
  }
  
  QDictIterator<Bar> it(data);
  for (; it.current(); ++it)
  {
    r = it.current();
    if (r->getData("Count") == 2)
    {
      Bar *bar = new Bar;
      bar->setDate(r->getDate());
      bar->setOpen(r->getClose());
      bar->setHigh(r->getClose());
      bar->setLow(r->getClose());
      bar->setClose(r->getClose());
      setBar(bar);
      delete bar;
    }
  }

  data.clear();
}

void Spread::loadData (QString symbol, QString method)
{
  ChartDb *db = new ChartDb;
  if (db->openChart(symbol))
  {
    qDebug("Spread::loadData: can't open db");
    delete db;
    return;
  }
  
  db->setBarCompression(BarData::DailyBar);
  db->setBarRange(99999999);
  
  bool rebuild = header->bool1;
  if (! rebuild)
  {
    Bar *bar = getLastBar();
    if (bar)
    {
      QDate d = QDate::currentDate();
      db->setBarRange(bar->getDate().getDate().daysTo(d));
      delete bar;
    }
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
      r->setClose(recordList->getClose(loop));
      r->setData("Count", 1);
      data.insert(r->getDate().getDateTimeString(FALSE), r);
      
      if (r->getDate().getDateValue() < fdate)
        fdate = r->getDate().getDateValue();
    }
    else
    {
      if (! method.compare(QObject::tr("Subtract")))
        r->setClose(r->getClose() - recordList->getClose(loop));

      if (! method.compare(QObject::tr("Divide")))
        r->setClose(r->getClose() / recordList->getClose(loop));
      
      r->setData("Count", 2);
    }
  }

  delete db;
}

QString Spread::createNew ()
{
  bool ok = FALSE;
  QString spread = QInputDialog::getText(QObject::tr("New Spread"),
                                         QObject::tr("Enter symbol name for the new Spread"),
					 QLineEdit::Normal,
					 QString::null,
					 &ok,
					 0);
  if (! spread.length() || ok == FALSE)
    return QString();

  QDir dir;
  Config config;
  QString s = config.getData(Config::DataPath) + "/Spread";
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
    {
      QMessageBox::information(0,
                               QObject::tr("Qtstalker: Error"),
			       QObject::tr("Could not create ~/Qtstalker/data/Spread directory."));
      return QString();
    }
  }
  
  s.append("/" + spread);
  if (dir.exists(s))
  {
    QMessageBox::information(0,
                             QObject::tr("Qtstalker: Error"),
			     QObject::tr("This Spread already exists."));
    return QString();
  }

  return s;  
}

void Spread::saveDbDefaults (Setting *set)
{
  strncpy(header->symbol, set->getData("Symbol").ascii(), SSIZE);
  strncpy(header->type, (char *) "Spread", SSIZE);
  strncpy(header->title, set->getData("Title").ascii(), TITLESIZE);
  header->barType = set->getData("BarType").toInt();
  strncpy(header->plugin, (char *) "Spread", SSIZE);
  saveFlag = TRUE;
}

void Spread::dump (QString d)
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

void Spread::deleteBar (QString d)
{
  if (! findRecord(d))
    return;
    
  fseek(db, -recordSize, SEEK_CUR);
  memset(&record, 0, recordSize);
  record.date = d.toDouble();
  fwrite(&record, recordSize, 1, db);
}

int Spread::readRecord ()
{
  return fread(&record, recordSize, 1, db);
}

int Spread::writeRecord ()
{
  return fwrite(&record, recordSize, 1, db);
}

bool Spread::getRecordState ()
{
  return record.state;
}

void Spread::fillBar (Bar *bar)
{
  bar->setDate(QString::number(record.date, 'f', 0));
  bar->setOpen(record.open);
  bar->setHigh(record.high);
  bar->setLow(record.low);
  bar->setClose(record.close);
}

double Spread::getRecordDate ()
{
  return record.date;
}

void Spread::fillRecord (Bar *bar)
{
  record.state = TRUE;
  record.date = bar->getDate().getDateValue();
  record.open = bar->getOpen();  
  record.high = bar->getHigh();  
  record.low = bar->getLow();
  record.close = bar->getClose();  
}

void Spread::setRecordDate (double d)
{
  record.date = d;
}

void Spread::clearRecord ()
{
  memset(&record, 0, recordSize);
}

int Spread::writeTempRecord ()
{
  return fwrite(&record, recordSize, 1, tdb);
}

void Spread::setBarString (QString d)
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
  Spread *o = new Spread;
  return ((DbPlugin *) o);
}

