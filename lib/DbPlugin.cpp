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

#include "DbPlugin.h"
#include <qobject.h>
#include <qdict.h>
#include <qdatetime.h>
#include <qmessagebox.h>
#include <qfileinfo.h>
#include <qdir.h>

DbPlugin::DbPlugin ()
{
  barCompression = BarData::DailyBar;
  barRange = 275;
  db = 0;
  header = 0;
  saveFlag = FALSE;
}

DbPlugin::~DbPlugin ()
{
}

void DbPlugin::setBarCompression (BarData::BarCompression d)
{
  barCompression = d;
}

void DbPlugin::setBarRange (int d)
{
  barRange = d;
}

void DbPlugin::setDb (FILE *d, ChartHeader *ch)
{
  db = d;
  header = ch;
}

void DbPlugin::setDbPath (QString d)
{
  path = d;
}

QString DbPlugin::getPluginName ()
{
  return pluginName;
}

QString DbPlugin::getHelpFile ()
{
  return helpFile;
}

void DbPlugin::close ()
{
  if (saveFlag)
  {
    rewind(db);
    fwrite(header, sizeof(ChartHeader), 1, db);
  }
  
  fclose(db);
}

QStringList DbPlugin::getChartObjectsList ()
{
  QStringList l = QStringList::split(",", header->co, FALSE);
  QStringList l2;
  int loop;
  Setting ts;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    ts.parse(l[loop]);
    l2.append(ts.getData("Name"));
  }
    
  return l2;
}

QPtrList<Setting> DbPlugin::getChartObjects ()
{
  QPtrList<Setting> list;
  list.setAutoDelete(TRUE);

  QStringList l = QStringList::split(",", header->co, FALSE);
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    Setting *set = new Setting;
    set->parse(l[loop]);
    list.append(set);
  }  
  
  return list;
}

void DbPlugin::setChartObject (QString d, Setting *set)
{
  QStringList l = QStringList::split(",", header->co, FALSE);
  int loop;
  Setting ts;
  bool flag= FALSE;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    ts.parse(l[loop]);
    if (! ts.getData("Name").compare(d))
    {
      l[loop] = set->getString();
      flag = TRUE;
      break;
    }
  }
  
  if (! flag)
  {
    l.append(set->getString());
    flag = TRUE;
  }
  
  if (flag)
  {
    QString s = l.join(",");
    if ((s.length() -1) < COSIZE)
    {
      strncpy(header->co, s.ascii(), COSIZE);
      saveFlag = TRUE;
    }
    else
      QMessageBox::warning(0,
                           QObject::tr("Qtstalker: Error"),
                           QObject::tr("Chart Object storage is full. Save failed"),
			   QMessageBox::Ok,
			   QMessageBox::NoButton,
			   QMessageBox::NoButton);
  }
}

void DbPlugin::deleteChartObject (QString d)
{
  QStringList l = QStringList::split(",", header->co, FALSE);
  Setting ts;
  for (QStringList::Iterator it = l.begin(); it != l.end(); ++it)
  {
    ts.parse(*it);
    if (! ts.getData("Name").compare(d))
    {
      l.remove(it);
      break;
    }
  }
  
  if (! l.count())
  {
    strncpy(header->co, "", COSIZE);
    saveFlag = TRUE;
    return;
  }
  
  QString s = l.join(",");
  if ((s.length() -1) < COSIZE)
  {
    strncpy(header->co, s.ascii(), COSIZE);
    saveFlag = TRUE;
  }
  else
    QMessageBox::warning(0,
                         QObject::tr("Qtstalker: Error"),
			 QObject::tr("Chart Object storage is full. Save failed"),
		         QMessageBox::Ok,
			 QMessageBox::NoButton,
			 QMessageBox::NoButton);
			 
}

void DbPlugin::dumpHeader (QTextStream &stream)
{
  stream << "FirstDate=" << QString::number(header->firstDate, 'f', 0) << "\n";
  stream << "LastDate=" << QString::number(header->lastDate, 'f', 0) << "\n";
  stream << "Symbol=" << header->symbol << "\n";
  stream << "Title=" << header->title << "\n";
  stream << "Type=" << header->type << "\n";
  stream << "Path=" << header->path << "\n";
  stream << "Records=" << QString::number(header->records) << "\n";
  stream << "CO=" << header->co << "\n";
  stream << "BarType=" << QString::number(header->barType) << "\n";
  stream << "Plugin=" << header->plugin << "\n";
  stream << "FuturesType=" << header->futuresType << "\n";
  stream << "FuturesMonth=" << header->futuresMonth << "\n";
  stream << "bool1=" << QString::number(header->bool1) << "\n";
  stream << "bool2=" << QString::number(header->bool2) << "\n";
  stream << "bool3=" << QString::number(header->bool3) << "\n";
  stream << "int1=" << QString::number(header->int1) << "\n";
  stream << "int2=" << QString::number(header->int2) << "\n";
  stream << "int3=" << QString::number(header->int3) << "\n";
  stream << "double1=" << QString::number(header->double1) << "\n";
  stream << "double2=" << QString::number(header->double2) << "\n";
  stream << "double3=" << QString::number(header->double3) << "\n";
  stream << "svar1=" << header->svar1 << "\n";
  stream << "svar2=" << header->svar2 << "\n";
  stream << "svar3=" << header->svar3 << "\n";
  stream << "mvar1=" << header->mvar1 << "\n";
  stream << "lvar1=" << header->lvar1 << "\n";
}

void DbPlugin::setHeader (Setting *set)
{
  QString s = set->getData("Symbol");
  if (s.length())
    strncpy(header->symbol, s.ascii(), SSIZE);
  
  s = set->getData("Title");
  if (s.length())
    strncpy(header->title, s.ascii(), TITLESIZE);
  
  s = set->getData("Type");
  if (s.length())
    strncpy(header->type, s.ascii(), SSIZE);
  
  s = set->getData("Path");
  if (s.length())
    strncpy(header->path, s.ascii(), PATHSIZE);
  
  s = set->getData("CO");
  if (s.length())
    strncpy(header->co, s.ascii(), COSIZE);
  
  s = set->getData("BarType");
  if (s.length())
    header->barType = s.toInt();
  
  s = set->getData("Plugin");
  if (s.length())
    strncpy(header->plugin, s.ascii(), SSIZE);
  
  s = set->getData("FuturesType");
  if (s.length())
    strncpy(header->futuresType, s.ascii(), SSSIZE);
  
  s = set->getData("FuturesMonth");
  if (s.length())
    strncpy(header->futuresMonth, s.ascii(), SSSIZE);
  
  s = set->getData("bool1");
  if (s.length())
    header->bool1 = s.toInt();
  
  s = set->getData("bool2");
  if (s.length())
    header->bool2 = s.toInt();
  
  s = set->getData("bool3");
  if (s.length())
    header->bool3 = s.toInt();
  
  s = set->getData("int1");
  if (s.length())
    header->int1 = s.toInt();
  
  s = set->getData("int2");
  if (s.length())
    header->int2 = s.toInt();
  
  s = set->getData("int3");
  if (s.length())
    header->int3 = s.toInt();

  s = set->getData("double1");
  if (s.length())
    header->double1 = s.toDouble();
  
  s = set->getData("double2");
  if (s.length())
    header->double2 = s.toDouble();
  
  s = set->getData("double3");
  if (s.length())
    header->double3 = s.toDouble();
  
  s = set->getData("svar1");
  if (s.length())
    strncpy(header->svar1, s.ascii(), SSIZE);
  
  s = set->getData("svar2");
  if (s.length())
    strncpy(header->svar2, s.ascii(), SSIZE);

  s = set->getData("svar3");
  if (s.length())
    strncpy(header->svar3, s.ascii(), SSIZE);
      
  s = set->getData("mvar1");
  if (s.length())
    strncpy(header->mvar1, s.ascii(), MSIZE);
  
  s = set->getData("lvar1");
  if (s.length())
    strncpy(header->lvar1, s.ascii(), LSIZE);
    
  saveFlag = TRUE;    
}

bool DbPlugin::findRecord (QString d)
{
  if (d.toDouble() < header->firstDate)
    return FALSE;
  else
  {
    if (d.toDouble() > header->lastDate)
      return FALSE;
  }

  BarDate dt;
  dt.setDate(QString::number(header->lastDate, 'f', 0));
  BarDate dt2;
  dt2.setDate(d);
    
  if (header->barType)
  {
    int min = dt2.getDateTime().secsTo(dt.getDateTime());
    min = (int) min / 60;
    min++;
    fseek(db, -(recordSize * min), SEEK_END);
  }
  else
  {
    int days = dt2.getDate().daysTo(dt.getDate());
    days++;
    fseek(db, -(recordSize * days), SEEK_END);
  }
  
  readRecord();
  
  if (getRecordDate() != d.toDouble())
    return FALSE;
    
  return TRUE;
}

Bar * DbPlugin::getBar (QString d)
{
  if (! findRecord(d))
    return 0;
  
  Bar *bar = new Bar;
  fillBar(bar);
  bar->setTickFlag(header->barType);
  return bar;
}

Bar * DbPlugin::getLastBar ()
{
  Bar *bar = 0;
  if (! header->records)
    return bar;
    
  fseek(db, -recordSize, SEEK_END);
  bool flag = FALSE;
  while(ftell(db) >= (long) sizeof(ChartHeader))
  {
    readRecord();
    if (! getRecordState())
    {
      fseek(db, -recordSize * 2, SEEK_CUR);
      continue;
    }
    else
    {
      flag = TRUE;
      break;
    }
  }
  
  if (! flag)
    return bar;
  
  bar = new Bar;
  fillBar(bar);
  bar->setTickFlag(header->barType);
  return bar;
}

void DbPlugin::setBar (Bar *bar)
{
  if (header->barType)
  {
    if (! bar->getTickFlag())
      return;
  }
  else
  {
    if (bar->getTickFlag())
      return;
  }
   
  // if this is the first record
  if (! header->records)
  {
    setFirstRecord(bar);
    return;  
  }
  
  // if we need to extend the db with more dates
  if (bar->getDate().getDateValue() > header->lastDate)
  {
    setAppendRecord(bar);
    return;
  }

  // if we need to prepend the db with more dates
  if (bar->getDate().getDateValue() < header->firstDate)
  {
    setInsertRecord(bar);
    return;
  }

  // we are overwriting a record here
  if (! findRecord(bar->getDate().getDateTimeString(FALSE)))
    return;
  
  fillRecord(bar);
  fseek(db, -recordSize, SEEK_CUR);
  writeRecord();
}

void DbPlugin::setFirstRecord (Bar *bar)
{
  fseek(db, sizeof(ChartHeader), SEEK_SET);
  fillRecord(bar);
  writeRecord();
  header->lastDate = bar->getDate().getDateValue();
  header->firstDate = bar->getDate().getDateValue();
  header->records++;
  saveFlag = TRUE;
}

void DbPlugin::setAppendRecord (Bar *bar)
{
  fseek(db, 0, SEEK_END);
    
  BarDate dt;
  dt.setDate(QString::number(header->lastDate, 'f', 0));
  if (header->barType)
    dt.addSecs(60);
  else
    dt.addDays(1);

  clearRecord();
        
  while (dt.getDateValue() < bar->getDate().getDateValue())
  {
    setRecordDate(dt.getDateValue());
    writeRecord();
    header->records++;
    if (header->barType)
      dt.addSecs(60);
    else
      dt.addDays(1);
  }
    
  fillRecord(bar);
  writeRecord();  
  header->lastDate = bar->getDate().getDateValue();
  header->records++;
  saveFlag = TRUE;
}

void DbPlugin::setInsertRecord (Bar *bar)
{
  // open up the tmp db
  QFileInfo fi(header->path);
  QString s = fi.dirPath(TRUE);
  s.append("/tmp");
  tdb = fopen(s.ascii(), "a+");
  fwrite(header, sizeof(ChartHeader), 1, tdb);
    
  // go back a year to give us alot of extra room
  BarDate dt;
  dt.setDate(bar->getDate().getDate());
  if (header->barType)
    dt.addSecs(-86400);
  else
    dt.addDays(-365);
    
  header->firstDate = dt.getDateValue();
  header->records = 0;

  clearRecord();
        
  // write the empty records
  while (dt.getDateValue() < bar->getDate().getDateValue())
  {
    setRecordDate(dt.getDateValue());
    writeTempRecord();
    header->records++;
    if (header->barType)
      dt.addSecs(60);
    else
      dt.addDays(1);
  }
    
  // save the new record
  fillRecord(bar);
  writeTempRecord();
  header->records++;
    
  // copy the old records into the tmp db
  fseek(db, sizeof(ChartHeader), SEEK_SET);
  while (readRecord())
  {
    writeTempRecord();
    header->records++;
  }      
  fclose(db);
  fclose(tdb);
    
  QDir dir;
  dir.remove(header->path);
  dir.rename(s, header->path);
  
  db = fopen(header->path, "r+");
    
  saveFlag = TRUE;
}

void DbPlugin::setSymbol (QString d)
{
  strncpy(header->symbol, d.ascii(), SSIZE);
  saveFlag = TRUE;
}

void DbPlugin::setTitle (QString d)
{
  strncpy(header->title, d.ascii(), TITLESIZE);
  saveFlag = TRUE;
}

void DbPlugin::setType (QString d)
{
  strncpy(header->type, d.ascii(), SSIZE);
  saveFlag = TRUE;
}

void DbPlugin::setFuturesType (QString d)
{
  strncpy(header->futuresType, d.ascii(), SSSIZE);
  saveFlag = TRUE;
}

void DbPlugin::setHeaderCO (QString d)
{
  strncpy(header->co, d.ascii(), COSIZE);
  saveFlag = TRUE;
}

void DbPlugin::setFuturesMonth (QString d)
{
  strncpy(header->futuresMonth, d.ascii(), SSSIZE);
  saveFlag = TRUE;
}

BarData * DbPlugin::getHistory ()
{
  barData = new BarData;
  if (barCompression >= BarData::DailyBar)
    barData->setBarType(BarData::Daily);
  else
    barData->setBarType(BarData::Tick);
  
  QTime timer;
  timer.start();
  
  if (fseek(db, -recordSize, SEEK_END) == -1)
  {
    qDebug("DbPlugin::getHistory:seek end failed");
    return barData;
  }

  if (ftell(db) < (long) sizeof(ChartHeader))
    return barData;
  
  switch(barCompression)
  {
    case BarData::Minute5:
      if (header->barType)
        getTickHistory(5);
      break;
    case BarData::Minute15:
      if (header->barType)
        getTickHistory(15);
      break;
    case BarData::Minute30:
      if (header->barType)
        getTickHistory(30);
      break;
    case BarData::Minute60:
      if (header->barType)
        getTickHistory(60);
      break;
    case BarData::DailyBar:
      if (header->barType)
        getDailyTickHistory();
      else
        getDailyHistory();
      break;
    case BarData::WeeklyBar:
      if (! header->barType)
        getWeeklyHistory();
      break;
    case BarData::MonthlyBar:
      if (! header->barType)
        getMonthlyHistory();
      break;
    default:
      break;
  }
  
  qDebug("Chart load: %i", timer.elapsed());
  
  barData->createDateList();
  
  return barData;
}

void DbPlugin::getDailyHistory ()
{
  while(ftell(db) >= (long) sizeof(ChartHeader))
  {
    if (barData->count() >= barRange)
      break;
      
    if (! readRecord())
      break;
      
    if (! getRecordState())
    {
      if (fseek(db, -recordSize * 2, SEEK_CUR) == -1)
        break;
      continue;
    }
    
    Bar *bar = new Bar;
    fillBar(bar);
    bar->setTickFlag(header->barType);
    
    barData->prepend(bar);
    
    if (fseek(db, -recordSize * 2, SEEK_CUR) == -1)
      break;
  }
}
  
void DbPlugin::getDailyTickHistory ()
{
  Bar *tbar = new Bar;
  Bar *bar = 0;
  while(ftell(db) >= (long) sizeof(ChartHeader))
  {
    readRecord();
    
    if (! getRecordState())
    {
      fseek(db, -(recordSize * 2), SEEK_CUR);
      continue;
    }
    
    fillBar(tbar);
    
    if (bar)
    {
      if (bar->getDate().getDate().day() == tbar->getDate().getDate().day())
      {
        bar->setOpen(tbar->getOpen());
        if (tbar->getHigh() > bar->getHigh())
          bar->setHigh(tbar->getHigh());
        if (tbar->getLow() < bar->getLow())
          bar->setLow(tbar->getLow());
        bar->setVolume(bar->getVolume() + tbar->getVolume());
      }
      else
      {
        barData->prepend(bar);
        bar = new Bar;
	
	BarDate dt;
	dt.setDate(tbar->getDate().getDate());
	dt.setTime(QTime(0, 0, 0, 0));
        bar->setDate(dt);
        bar->setOpen(tbar->getOpen());
        bar->setHigh(tbar->getHigh());
        bar->setLow(tbar->getLow());
        bar->setClose(tbar->getClose());
        bar->setVolume(tbar->getVolume());
      }
    }
    else
    {
      bar = new Bar;
	
      BarDate dt;
      dt.setDate(tbar->getDate().getDate());
      dt.setTime(QTime(0, 0, 0, 0));
      bar->setDate(dt);
      bar->setOpen(tbar->getOpen());
      bar->setHigh(tbar->getHigh());
      bar->setLow(tbar->getLow());
      bar->setClose(tbar->getClose());
      bar->setVolume(tbar->getVolume());
    }

    if (barData->count() >= barRange)
      break;
      
    fseek(db, -(recordSize * 2), SEEK_CUR);
  }
  
  delete tbar;
  
  if (bar->getOpen())
    barData->prepend(bar);
  else
    delete bar;
}

void DbPlugin::getWeeklyHistory ()
{
  Bar *tbar = new Bar;
  BarDate dt;
  dt.setDate(QString::number(header->lastDate, 'f', 0));
  int day = dt.getDate().dayOfWeek();
  day--;
  dt.addDays(-day);

  while(ftell(db) >= (long) sizeof(ChartHeader))
  {
    if (! findRecord(dt.getDateTimeString(FALSE)))
      break;

    if (barData->count() >= barRange)
      break;
      
    int loop;
    Bar *bar = new Bar;
    bar->setTickFlag(header->barType);
    
    for (loop = 0; loop < 6; loop++)
    {
      if (! getRecordState())
      {
        readRecord();
	continue;
      }
      
      fillBar(tbar);
      
      if (! bar->getOpen())
      {
        bar->setDate(tbar->getDate());
        bar->setOpen(tbar->getOpen());
        bar->setHigh(tbar->getHigh());
        bar->setLow(tbar->getLow());
        bar->setClose(tbar->getClose());
        bar->setVolume(tbar->getVolume());
      }
      else
      {
        bar->setDate(tbar->getDate());
        if (tbar->getHigh() > bar->getHigh())
          bar->setHigh(tbar->getHigh());
        if (tbar->getLow() < bar->getLow())
          bar->setLow(tbar->getLow());
        bar->setClose(tbar->getClose());
        bar->setVolume(bar->getVolume() + tbar->getVolume());
      }
      
      readRecord();
    }

    if (bar->getOpen())
      barData->prepend(bar);
    else
      delete bar;
      
    dt.addDays(-7);
  }
  
  delete tbar;
}

void DbPlugin::getMonthlyHistory ()
{
  Bar *tbar = new Bar;
  BarDate dt;
  dt.setDate(QString::number(header->lastDate, 'f', 0));
  int day = dt.getDate().day();
  day--;
  dt.addDays(-day);

  while(ftell(db) >= (long) sizeof(ChartHeader))
  {
    if (! findRecord(dt.getDateTimeString(FALSE)))
      break;

    if (barData->count() >= barRange)
      break;
      
    int loop;
    Bar *bar = new Bar;
    bar->setTickFlag(header->barType);
    
    for (loop = 0; loop < dt.getDate().daysInMonth() + 1; loop++)
    {
      if (! getRecordState())
      {
        readRecord();
	continue;
      }
      
      fillBar(tbar);
      
      if (! bar->getOpen())
      {
        bar->setDate(tbar->getDate());
        bar->setOpen(tbar->getOpen());
        bar->setHigh(tbar->getHigh());
        bar->setLow(tbar->getLow());
        bar->setClose(tbar->getClose());
        bar->setVolume(tbar->getVolume());
      }
      else
      {
        bar->setDate(tbar->getDate());
        if (tbar->getHigh() > bar->getHigh())
          bar->setHigh(tbar->getHigh());
        if (tbar->getLow() < bar->getLow())
          bar->setLow(tbar->getLow());
        bar->setClose(tbar->getClose());
        bar->setVolume(bar->getVolume() + tbar->getVolume());
      }
      
      readRecord();
    }

    if (bar->getOpen())
      barData->prepend(bar);
    else
      delete bar;

    QDate td = dt.getDate();      
    td = td.addMonths(-1);
    td = td.addDays(-(td.day() - 1));
    dt.setDate(td);
  }
  
  delete tbar;
}

void DbPlugin::getTickHistory (int mins)
{
  BarDate ed;
  ed.setDate(QString::number(header->lastDate, 'f', 0));
  int t = (int) ed.getTime().minute() / mins;
  t++;
  ed.addSecs(((t * mins) - ed.getTime().minute()) * 60);
  
  BarDate sd;
  sd.setDate(ed.getDateTimeString(FALSE));
  sd.addSecs(-(mins * 60));
  
  Bar *tbar = new Bar;
  Bar *bar = new Bar;
  bar->setTickFlag(header->barType);
  bar->setDate(QString::number(ed.getDateValue(), 'f', 0));

  while(ftell(db) >= (long) sizeof(ChartHeader))
  {
    readRecord();
    
    if (getRecordDate() < ed.getDateValue() && getRecordDate() >= sd.getDateValue())
    {
      if (getRecordState())
      {
        fillBar(tbar);
	
        if (bar->getOpen())
        {
          bar->setOpen(tbar->getOpen());
          if (tbar->getHigh() > bar->getHigh())
            bar->setHigh(tbar->getHigh());
          if (tbar->getLow() < bar->getLow())
            bar->setLow(tbar->getLow());
          bar->setVolume(bar->getVolume() + tbar->getVolume());
	}
	else
	{
          bar->setOpen(tbar->getOpen());
          bar->setHigh(tbar->getHigh());
          bar->setLow(tbar->getLow());
          bar->setClose(tbar->getClose());
          bar->setVolume(tbar->getVolume());
	}
      }
    }
    else
    {
      if (bar->getOpen())
      {
        barData->prepend(bar);
        bar = new Bar;
      }
      
      ed.addSecs(-(mins * 60));
      sd.addSecs(-(mins * 60));
	
      bar->setTickFlag(header->barType);
      bar->setDate(QString::number(ed.getDateValue(), 'f', 0));
      
      if (getRecordState())
      {
        fillBar(tbar);
	
        if (bar->getOpen())
        {
          bar->setOpen(tbar->getOpen());
          if (tbar->getHigh() > bar->getHigh())
            bar->setHigh(tbar->getHigh());
          if (tbar->getLow() < bar->getLow())
            bar->setLow(tbar->getLow());
          bar->setVolume(bar->getVolume() + tbar->getVolume());
	}
	else
	{
          bar->setOpen(tbar->getOpen());
          bar->setHigh(tbar->getHigh());
          bar->setLow(tbar->getLow());
          bar->setClose(tbar->getClose());
          bar->setVolume(tbar->getVolume());
	}
      }
    }

    if (barData->count() >= barRange)
      break;
      
    fseek(db, -(recordSize * 2), SEEK_CUR);
  }
  
  delete tbar;
  
  if (bar->getOpen())
    barData->prepend(bar);
  else
    delete bar;
}

//*********************************************************
//***************** VIRTUAL OVERRIDES *********************
//*********************************************************

QString DbPlugin::createNew ()
{
  return QString();
}

void DbPlugin::saveDbDefaults (Setting *)
{
}

void DbPlugin::dbPrefDialog ()
{
}

void DbPlugin::dump (QString)
{
}

void DbPlugin::deleteBar (QString)
{
}

int DbPlugin::readRecord ()
{
  return 0;
}

bool DbPlugin::getRecordState ()
{
  return FALSE;
}

void DbPlugin::fillBar (Bar *)
{
}

double DbPlugin::getRecordDate ()
{
  return 0;
}

int DbPlugin::writeRecord ()
{
  return 0;
}

void DbPlugin::fillRecord (Bar *)
{
}

void DbPlugin::setRecordDate (double)
{
}

void DbPlugin::clearRecord ()
{
}

int DbPlugin::writeTempRecord ()
{
  return 0;
}

void DbPlugin::setBarString (QString)
{
}

