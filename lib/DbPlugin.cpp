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

void DbPlugin::close ()
{
  if (saveFlag)
  {
    rewind(db);
    fwrite(header, sizeof(ChartHeader), 1, db);
  }
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
  stream << "Symbol=" << getHeaderField(Symbol) << "\n";
  stream << "Title=" << getHeaderField(Title) << "\n";
  stream << "Type=" << getHeaderField(Type) << "\n";
  stream << "Path=" << getHeaderField(Path) << "\n";
  stream << "Records=" << QString::number(header->records) << "\n";
  stream << "CO=" << getHeaderField(CO) << "\n";
  stream << "BarType=" << getHeaderField(BarType) << "\n";
  stream << "Plugin=" << getHeaderField(Plugin) << "\n";
  stream << "FuturesType=" << getHeaderField(FuturesType) << "\n";
  stream << "FuturesMonth=" << getHeaderField(FuturesMonth) << "\n";
  stream << "bool1=" << getHeaderField(Bool1) << "\n";
  stream << "bool2=" << getHeaderField(Bool2) << "\n";
  stream << "bool3=" << getHeaderField(Bool3) << "\n";
  stream << "int1=" << getHeaderField(Int1) << "\n";
  stream << "int2=" << getHeaderField(Int2) << "\n";
  stream << "int3=" << getHeaderField(Int3) << "\n";
  stream << "double1=" << getHeaderField(Double1) << "\n";
  stream << "double2=" << getHeaderField(Double2) << "\n";
  stream << "double3=" << getHeaderField(Double3) << "\n";
  stream << "svar1=" << getHeaderField(Svar1) << "\n";
  stream << "svar2=" << getHeaderField(Svar2) << "\n";
  stream << "svar3=" << getHeaderField(Svar3) << "\n";
  stream << "mvar1=" << getHeaderField(Mvar1) << "\n";
  stream << "lvar1=" << getHeaderField(Lvar1) << "\n";
}

void DbPlugin::setHeader (Setting *set)
{
  QString s = set->getData("Symbol");
  if (s.length())
    setHeaderField(Symbol, s);
  
  s = set->getData("Title");
  if (s.length())
    setHeaderField(Title, s);
  
  s = set->getData("Type");
  if (s.length())
    setHeaderField(Type, s);
  
  s = set->getData("Path");
  if (s.length())
    setHeaderField(Path, s);
  
  s = set->getData("CO");
  if (s.length())
    setHeaderField(CO, s);
  
  s = set->getData("BarType");
  if (s.length())
    setHeaderField(BarType, s);
  
  s = set->getData("Plugin");
  if (s.length())
    setHeaderField(Plugin, s);
  
  s = set->getData("FuturesType");
  if (s.length())
    setHeaderField(FuturesType, s);
  
  s = set->getData("FuturesMonth");
  if (s.length())
    setHeaderField(FuturesMonth, s);
  
  s = set->getData("bool1");
  if (s.length())
    setHeaderField(Bool1, s);
  
  s = set->getData("bool2");
  if (s.length())
    setHeaderField(Bool2, s);
  
  s = set->getData("bool3");
  if (s.length())
    setHeaderField(Bool3, s);
  
  s = set->getData("int1");
  if (s.length())
    setHeaderField(Int1, s);
  
  s = set->getData("int2");
  if (s.length())
    setHeaderField(Int2, s);
  
  s = set->getData("int3");
  if (s.length())
    setHeaderField(Int3, s);

  s = set->getData("double1");
  if (s.length())
    setHeaderField(Double1, s);
  
  s = set->getData("double2");
  if (s.length())
    setHeaderField(Double2, s);
  
  s = set->getData("double3");
  if (s.length())
    setHeaderField(Double3, s);
  
  s = set->getData("svar1");
  if (s.length())
    setHeaderField(Svar1, s);
  
  s = set->getData("svar2");
  if (s.length())
    setHeaderField(Svar2, s);

  s = set->getData("svar3");
  if (s.length())
    setHeaderField(Svar3, s);
      
  s = set->getData("mvar1");
  if (s.length())
    setHeaderField(Mvar1, s);
  
  s = set->getData("lvar1");
  if (s.length())
    setHeaderField(Lvar1, s);
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

BarData * DbPlugin::getHistory ()
{
  barData = new BarData;
  if (barCompression >= BarData::DailyBar)
    barData->setBarType(BarData::Daily);
  else
    barData->setBarType(BarData::Tick);
  
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

void DbPlugin::setHeaderField (int k, QString d)
{
  switch (k)
  {
    case BarType:
      header->barType = d.toInt();
      break;
    case Plugin:
      strncpy(header->plugin, d.ascii(), SSIZE);
      break;
    case Symbol:
      strncpy(header->symbol, d.ascii(), SSIZE);
      break;
    case Type:
      strncpy(header->type, d.ascii(), SSIZE);
      break;
    case FuturesType:
      strncpy(header->futuresType, d.ascii(), SSSIZE);
      break;
    case FuturesMonth:
      strncpy(header->futuresMonth, d.ascii(), SSSIZE);
      break;
    case Title:
      strncpy(header->title, d.ascii(), TITLESIZE);
      break;
    case Path:
      strncpy(header->path, d.ascii(), PATHSIZE);
      break;
    case CO:
      strncpy(header->co, d.ascii(), COSIZE);
      break;
    case Svar1:
      strncpy(header->svar1, d.ascii(), SSIZE);
      break;
    case Svar2:
      strncpy(header->svar2, d.ascii(), SSIZE);
      break;
    case Svar3:
      strncpy(header->svar3, d.ascii(), SSIZE);
      break;
    case Mvar1:
      strncpy(header->mvar1, d.ascii(), MSIZE);
      break;
    case Lvar1:
      strncpy(header->lvar1, d.ascii(), LSIZE);
      break;
    case Bool1:
      header->bool1 = d.toInt();
      break;
    case Bool2:
      header->bool2 = d.toInt();
      break;
    case Bool3:
      header->bool3 = d.toInt();
      break;
    case Int1:
      header->int1 = d.toInt();
      break;
    case Int2:
      header->int2 = d.toInt();
      break;
    case Int3:
      header->int3 = d.toInt();
      break;
    case Double1:
      header->double1 = d.toDouble();
      break;
    case Double2:
      header->double2 = d.toDouble();
      break;
    case Double3:
      header->double3 = d.toDouble();
      break;
    default:
      break;
  }
  
  saveFlag = TRUE;    
}

QString DbPlugin::getHeaderField (int k)
{
  QString s;
  
  switch (k)
  {
    case BarType:
      s = QString::number(header->barType);
      break;
    case Plugin:
      s= header->plugin;
      break;
    case Symbol:
      s = header->symbol;
      break;
    case Type:
      s = header->type;
      break;
    case FuturesType:
      s = header->futuresType;
      break;
    case FuturesMonth:
      s = header->futuresMonth;
      break;
    case Title:
      s = header->title;
      break;
    case Path:
      s = header->path;
      break;
    case CO:
      s = header->co;
      break;
    case Svar1:
      s = header->svar1;
      break;
    case Svar2:
      s = header->svar2;
      break;
    case Svar3:
      s = header->svar3;
      break;
    case Mvar1:
      s = header->mvar1;
      break;
    case Lvar1:
      s = header->lvar1;
      break;
    case Bool1:
      s = QString::number(header->bool1);
      break;
    case Bool2:
      s = QString::number(header->bool2);
      break;
    case Bool3:
      s = QString::number(header->bool3);
      break;
    case Int1:
      s = QString::number(header->int1);
      break;
    case Int2:
      s = QString::number(header->int2);
      break;
    case Int3:
      s = QString::number(header->int3);
      break;
    case Double1:
      s = QString::number(header->double1);
      break;
    case Double2:
      s = QString::number(header->double2);
      break;
    case Double3:
      s = QString::number(header->double3);
      break;
    default:
      break;
  }
  
  return s;
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

void DbPlugin::dump (QString, bool)
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

