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
#include <qfile.h>

DbPlugin::DbPlugin ()
{
  barCompression = BarData::DailyBar;
  barRange = 275;
  db = 0;
}

DbPlugin::~DbPlugin ()
{
  if (db)
    db->close(db, 0);
}

int DbPlugin::openChart (QString d)
{
  if (db)
  {
    qDebug("DbPlugin::openChart: db already open");
    return TRUE;
  }

  bool flag = FALSE;  
  QDir dir(d);
  if (! dir.exists())
    flag = TRUE;
  
  int rc = db_create(&db, NULL, 0);
  if (rc)
  {
    qDebug("DbPlugin::openChart: %s", db_strerror(rc));
    return TRUE;
  }
  
  rc = db->open(db, NULL, (char *) d.latin1(), NULL, DB_BTREE, DB_CREATE, 0664);
  if (rc)
  {
    qDebug("DbPlugin::openChart: %s", db_strerror(rc));
    return TRUE;
  }

  if (flag)
    setHeaderField(Path, d.ascii());
  
  return FALSE;
}

void DbPlugin::close ()
{
  if (db)
  {
    db->close(db, 0);
    db = 0;
  }
}

QString DbPlugin::getData (QString k)
{
  DBT key;
  DBT data;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  key.data = (char *) k.latin1();
  key.size = k.length() + 1;

  QString s;
  if (db->get(db, NULL, &key, &data, 0) == 0)
    s = (char *) data.data;

  return s;
}

void DbPlugin::setData (QString k, QString d)
{
  DBT key;
  DBT data;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  key.data = (char *) k.latin1();
  key.size = k.length() + 1;

  data.data = (char *) d.latin1();
  data.size = d.length() + 1;

  db->put(db, NULL, &key, &data, 0);
}

void DbPlugin::deleteData (QString k)
{
  DBT key;
  memset(&key, 0, sizeof(DBT));

  key.data = (char *) k.latin1();
  key.size = k.length() + 1;

  db->del(db, NULL, &key, 0);
}

void DbPlugin::setBarCompression (BarData::BarCompression d)
{
  barCompression = d;
}

void DbPlugin::setBarRange (int d)
{
  barRange = d;
}

QString DbPlugin::getHelpFile ()
{
  return helpFile;
}

QStringList DbPlugin::getChartObjectsList ()
{
  QStringList l = QStringList::split(",", getHeaderField(CO), FALSE);
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

  QStringList l = QStringList::split(",", getHeaderField(CO), FALSE);
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
  QStringList l = QStringList::split(",", getHeaderField(CO), FALSE);
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
    setHeaderField(CO, l.join(","));
}

void DbPlugin::deleteChartObject (QString d)
{
  QStringList l = QStringList::split(",", getHeaderField(CO), FALSE);
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
    setHeaderField(CO, "");
    return;
  }
  
  setHeaderField(CO, l.join(","));
}

void DbPlugin::dump (QString d, bool f)
{
  QFile outFile(d);
  if (! outFile.open(IO_WriteOnly))
    return;
  QTextStream outStream(&outFile);
  
  DBT key;
  DBT data;
  DBC *cursor;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  db->cursor(db, NULL, &cursor, 0);

  while (! cursor->c_get(cursor, &key, &data, DB_NEXT))
  {
    if (f)
    {
      if (key.size != 15)
        continue;
    
      BarDate dt;
      if (dt.setDate((char *) key.data))
        continue;
  
      outStream << (char *) key.data << "," << (char *) data.data << "\n";
    }
    else
      outStream << (char *) key.data << "=" << (char *) data.data << "\n";
  }

  cursor->c_close(cursor);

  outFile.close();
}

Bar * DbPlugin::getFirstBar ()
{
  DBT key;
  DBT data;
  DBC *cursor;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));
  Bar *bar = 0;

  db->cursor(db, NULL, &cursor, 0);
  while (! cursor->c_get(cursor, &key, &data, DB_NEXT))
  {
    if (key.size != 15)
      continue;
    
    BarDate dt;
    if (dt.setDate((char *) key.data))
      continue;

    bar = getBar((char *) key.data, (char *) data.data);
    
    break;
  }
  cursor->c_close(cursor);
  
  return bar;
}

Bar * DbPlugin::getLastBar ()
{
  DBT key;
  DBT data;
  DBC *cursor;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));
  Bar *bar = 0;

  db->cursor(db, NULL, &cursor, 0);
  while (! cursor->c_get(cursor, &key, &data, DB_PREV))
  {
    if (key.size != 15)
      continue;
    
    BarDate dt;
    if (dt.setDate((char *) key.data))
      continue;

    bar = getBar((char *) key.data, (char *) data.data);
    
    break;
  }
  cursor->c_close(cursor);

  return bar;
}

Bar * DbPlugin::getBar (QString k)
{
  QString d = getData(k);
  if (d.length())
    return getBar(k, d);
  else
    return 0;
}

BarData * DbPlugin::getHistory ()
{
  barData = new BarData;
  if (barCompression >= BarData::DailyBar)
    barData->setBarType(BarData::Daily);
  else
    barData->setBarType(BarData::Tick);

  int barType = getHeaderField(BarType).toInt();
    
  switch(barCompression)
  {
    case BarData::Minute5:
      if (barType)
        getTickHistory(5);
      break;
    case BarData::Minute15:
      if (barType)
        getTickHistory(15);
      break;
    case BarData::Minute30:
      if (barType)
        getTickHistory(30);
      break;
    case BarData::Minute60:
      if (barType)
        getTickHistory(60);
      break;
    case BarData::DailyBar:
      if (barType)
        getDailyTickHistory();
      else
        getDailyHistory();
      break;
    case BarData::WeeklyBar:
      if (! barType)
        getWeeklyHistory();
      break;
    case BarData::MonthlyBar:
      if (! barType)
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
  DBT key;
  DBT data;
  DBC *cursor;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  int barType = getHeaderField(BarType).toInt();
  
  db->cursor(db, NULL, &cursor, 0);
  
  while (! cursor->c_get(cursor, &key, &data, DB_PREV))
  {
    if (barData->count() >= barRange)
      break;
      
    if (key.size != 15)
      continue;
    
    BarDate dt;
    if (dt.setDate((char *) key.data))
      continue;
      
    Bar *bar = getBar((char *) key.data, (char *) data.data);
    bar->setTickFlag(barType);
    barData->prepend(bar);
  }
  
  cursor->c_close(cursor);
}
  
void DbPlugin::getDailyTickHistory ()
{
  DBT key;
  DBT data;
  DBC *cursor;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  Bar *tbar = 0;
  Bar *bar = 0;
    
  db->cursor(db, NULL, &cursor, 0);
  
  while (! cursor->c_get(cursor, &key, &data, DB_PREV))
  {
    if (barData->count() >= barRange)
      break;
      
    if (key.size != 15)
      continue;
    
    BarDate dt;
    if (dt.setDate((char *) key.data))
      continue;
    
    tbar = getBar((char *) key.data, (char *) data.data);
    
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

    delete tbar;
  }
  
  if (bar->count())
    barData->prepend(bar);
  else
    delete bar;
    
  cursor->c_close(cursor);
}

void DbPlugin::getWeeklyHistory ()
{
  DBT key;
  DBT data;
  DBC *cursor;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  Bar *bar = new Bar;
  int week = -1;
  int year = 0;
  int tyear = 0;
    
  db->cursor(db, NULL, &cursor, 0);
  
  while (! cursor->c_get(cursor, &key, &data, DB_PREV))
  {
    if (barData->count() >= barRange)
      break;
      
    if (key.size != 15)
      continue;
    
    BarDate dt;
    if (dt.setDate((char *) key.data))
      continue;
      
    int tweek = dt.getDate().weekNumber(&tyear);
    if (tweek != week)
    {
      week = tweek;
      year = tyear;
      if (! bar->count())
        delete bar;
      else
        barData->prepend(bar);
	
      bar = getBar((char *) key.data, (char *) data.data);
    }
    else
    {
      if (tyear != year)
      {
        week = tweek;
        year = tyear;
        if (! bar->count())
          delete bar;
        else
          barData->prepend(bar);
	
        bar = getBar((char *) key.data, (char *) data.data);
      }
    }      

    Bar *tbar = getBar((char *) key.data, (char *) data.data);
    if (tbar->getHigh() > bar->getHigh())
      bar->setHigh(tbar->getHigh());
    if (tbar->getLow() < bar->getLow())
      bar->setLow(tbar->getLow());
    bar->setOpen(tbar->getOpen());
    bar->setVolume(bar->getVolume() + tbar->getVolume());
    delete tbar;
  }

  db->cursor(db, NULL, &cursor, 0);

  if (bar->count())
    barData->prepend(bar);
  else
    delete bar;
}

void DbPlugin::getMonthlyHistory ()
{
  DBT key;
  DBT data;
  DBC *cursor;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  Bar *bar = new Bar;
  int month = -1;
  int year = 0;
    
  db->cursor(db, NULL, &cursor, 0);
  
  while (! cursor->c_get(cursor, &key, &data, DB_PREV))
  {
    if (barData->count() >= barRange)
      break;
      
    if (key.size != 15)
      continue;
    
    BarDate dt;
    if (dt.setDate((char *) key.data))
      continue;
      
    int tmonth = dt.getDate().month();
    int tyear = dt.getDate().year();
    if (tmonth != month)
    {
      month = tmonth;
      year = tyear;
      if (! bar->count())
        delete bar;
      else
        barData->prepend(bar);
	
      bar = getBar((char *) key.data, (char *) data.data);
    }
    else
    {
      if (tyear != year)
      {
        month = tmonth;
        year = tyear;
        if (! bar->count())
          delete bar;
        else
          barData->prepend(bar);
	
        bar = getBar((char *) key.data, (char *) data.data);
      }
    }      

    Bar *tbar = getBar((char *) key.data, (char *) data.data);
    if (tbar->getHigh() > bar->getHigh())
      bar->setHigh(tbar->getHigh());
    if (tbar->getLow() < bar->getLow())
      bar->setLow(tbar->getLow());
    bar->setOpen(tbar->getOpen());
    bar->setVolume(bar->getVolume() + tbar->getVolume());
    delete tbar;
  }

  db->cursor(db, NULL, &cursor, 0);

  if (bar->count())
    barData->prepend(bar);
  else
    delete bar;
}

void DbPlugin::getTickHistory (int mins)
{
  DBT key;
  DBT data;
  DBC *cursor;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  Bar *lbar = getLastBar();
  if (! lbar)
    return;
    
  BarDate ed;
  ed.setDate(lbar->getDate().getDate());
  ed.setTime(lbar->getDate().getTime());
  int t = (int) ed.getTime().minute() / mins;
  t++;
  ed.addSecs(((t * mins) - ed.getTime().minute()) * 60);
  delete lbar;
  
  BarDate sd;
  sd.setDate(ed.getDateTimeString(FALSE));
  sd.addSecs(-(mins * 60));
  
  int barType = getHeaderField(BarType).toInt();  
  Bar *tbar = new Bar;
  Bar *bar = new Bar;
  bar->setTickFlag(barType);
  bar->setDate(QString::number(ed.getDateValue(), 'f', 0));

  db->cursor(db, NULL, &cursor, 0);
  
  while (! cursor->c_get(cursor, &key, &data, DB_PREV))
  {
    if (barData->count() >= barRange)
      break;
      
    if (key.size != 15)
      continue;
    
    BarDate dt;
    if (dt.setDate((char *) key.data))
      continue;
    
    if (dt.getDateValue() < ed.getDateValue() && dt.getDateValue() >= sd.getDateValue())
    {
      delete tbar;
      tbar = getBar((char *) key.data, (char *) data.data);
	
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
    else
    {
      if (bar->getOpen())
      {
        barData->prepend(bar);
        bar = new Bar;
      }
      
      ed.addSecs(-(mins * 60));
      sd.addSecs(-(mins * 60));
	
      bar->setTickFlag(barType);
      bar->setDate(QString::number(ed.getDateValue(), 'f', 0));
      
      delete tbar;
      tbar = getBar((char *) key.data, (char *) data.data);
	
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

  db->cursor(db, NULL, &cursor, 0);
    
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
      setData("BarType", d);
      break;
    case Plugin:
      setData("Plugin", d);
      break;
    case Symbol:
      setData("Symbol", d);
      break;
    case Type:
      setData("Type", d);
      break;
    case Title:
      setData("Title", d);
      break;
    case Path:
      setData("Path", d);
      break;
    case CO:
      setData("CO", d);
      break;
    default:
      break;
  }
}

QString DbPlugin::getHeaderField (int k)
{
  QString s;
  
  switch (k)
  {
    case BarType:
      s = getData("BarType");
      break;
    case Plugin:
      s = getData("Plugin");
      break;
    case Symbol:
      s = getData("Symbol");
      break;
    case Type:
      s = getData("Type");
      break;
    case Title:
      s = getData("Title");
      break;
    case Path:
      s = getData("Path");
      break;
    case CO:
      s = getData("CO");
      break;
    default:
      break;
  }
  
  return s;
}

//*********************************************************
//***************** VIRTUAL OVERRIDES *********************
//*********************************************************

void DbPlugin::createNew ()
{
}

void DbPlugin::dbPrefDialog ()
{
}

Bar * DbPlugin::getBar (QString, QString)
{
  return 0;
}

void DbPlugin::setBar (Bar *)
{
}
