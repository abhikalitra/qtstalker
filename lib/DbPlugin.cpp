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

int DbPlugin::openChart (QString &d)
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
    setHeaderField(Path, d);
  
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

void DbPlugin::getData (QString &k, QString &d)
{
  DBT key;
  DBT data;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  key.data = (char *) k.latin1();
  key.size = k.length() + 1;

  if (db->get(db, NULL, &key, &data, 0) == 0)
    d = (char *) data.data;
  else
    d.truncate(0);
}

void DbPlugin::setData (QString &k, QString &d)
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

void DbPlugin::deleteData (QString &k)
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

void DbPlugin::getHelpFile (QString &d)
{
  d = helpFile;
}

void DbPlugin::getChartObjectsList (QStringList &d)
{
  QString s;
  getHeaderField(CO, s);
  QStringList l = QStringList::split(",", s, FALSE);
  int loop;
  Setting ts;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    ts.parse(l[loop]);
    d.append(ts.getData("Name"));
  }
}

void DbPlugin::getChartObjects (QStringList &d)
{
  QString s;
  getHeaderField(CO, s);
  d = QStringList::split(",", s, FALSE);
}

void DbPlugin::setChartObject (QString &d, Setting &set)
{
  QString s;
  getHeaderField(CO, s);
  QStringList l = QStringList::split(",", s, FALSE);
  
  int loop;
  Setting ts;
  bool flag= FALSE;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    ts.parse(l[loop]);
    if (! ts.getData("Name").compare(d))
    {
      set.getString(l[loop]);
      flag = TRUE;
      break;
    }
  }
  
  if (! flag)
  {
    set.getString(s);
    l.append(s);
    flag = TRUE;
  }
  
  if (flag)
  {
    s = l.join(",");
    setHeaderField(CO, s);
  }
}

void DbPlugin::deleteChartObject (QString &d)
{
  QString s;
  getHeaderField(CO, s);
  QStringList l = QStringList::split(",", s, FALSE);
  
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
    s = "";
    setHeaderField(CO, s);
    return;
  }

  s = l.join(",");
  setHeaderField(CO, s);
}

void DbPlugin::dump (QString &d, bool f)
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
      QString s = (char *) key.data;
      if (dt.setDate(s))
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
    QString k = (char *) key.data;
    if (dt.setDate(k))
      continue;

    QString d = (char *) data.data;
    bar = getBar(k, d);
   
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
    QString k = (char *) key.data;
    if (dt.setDate(k))
      continue;

    QString d = (char *) data.data;
    bar = getBar(k, d);
    
    break;
  }
  cursor->c_close(cursor);

  return bar;
}

Bar * DbPlugin::getBar (QString &k)
{
  QString d;
  getData(k, d);
  if (d.length())
    return getBar(k, d);
  else
    return 0;
}

void DbPlugin::getHistory (BarData *bd)
{
  barData = bd;
  QString s;
  getHeaderField(BarType, s);
  int barType = s.toInt();
  barData->setBarType((BarData::BarType) barType);  
    
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
}

void DbPlugin::getDailyHistory ()
{
  DBT key;
  DBT data;
  DBC *cursor;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  QString s;
  getHeaderField(BarType, s);
  int barType = s.toInt();
  
  db->cursor(db, NULL, &cursor, 0);
  
  while (! cursor->c_get(cursor, &key, &data, DB_PREV))
  {
    if (barData->count() >= barRange)
      break;
      
    if (key.size != 15)
      continue;
    
    BarDate dt;
    QString k = (char *) key.data;
    if (dt.setDate(k))
      continue;

    QString d = (char *) data.data;
    Bar *bar = getBar(k, d);
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
    QString k = (char *) key.data;
    if (dt.setDate(k))
      continue;
    
    QString d = (char *) data.data;
    tbar = getBar(k, d);
    
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
	QDate t = tbar->getDate().getDate();
	dt.setDate(t);
	QTime t2(0, 0, 0, 0);
	dt.setTime(t2);
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
      QDate t = tbar->getDate().getDate();
      dt.setDate(t);
      QTime t2(0, 0, 0, 0);
      dt.setTime(t2);
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
    QString k = (char *) key.data;
    if (dt.setDate(k))
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
	
      QString d = (char *) data.data;
      bar = getBar(k, d);
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
	
        QString k = (char *) key.data;
        QString d = (char *) data.data;
        bar = getBar(k, d);
      }
    }      

    QString d = (char *) data.data;
    Bar *tbar = getBar(k, d);
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
    QString k = (char *) key.data;
    if (dt.setDate(k))
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
	
      QString d = (char *) data.data;
      bar = getBar(k, d);
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
	
        QString k = (char *) key.data;
        QString d = (char *) data.data;
        bar = getBar(k, d);
      }
    }      

    QString d = (char *) data.data;
    Bar *tbar = getBar(k, d);
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
  QDate tdt = lbar->getDate().getDate();
  ed.setDate(tdt);
  QTime tt = lbar->getDate().getTime();
  ed.setTime(tt);
  int t = (int) ed.getTime().minute() / mins;
  t++;
  ed.addSecs(((t * mins) - ed.getTime().minute()) * 60);
  delete lbar;
  
  BarDate sd;
  QString s;
  ed.getDateTimeString(FALSE, s);
  sd.setDate(s);
  sd.addSecs(-(mins * 60));
  
  getHeaderField(BarType, s);
  int barType = s.toInt();
  
  Bar *tbar = new Bar;
  Bar *bar = new Bar;
  bar->setTickFlag(barType);
  s = QString::number(ed.getDateValue(), 'f', 0);
  bar->setDate(s);

  db->cursor(db, NULL, &cursor, 0);
  
  while (! cursor->c_get(cursor, &key, &data, DB_PREV))
  {
    if (barData->count() >= barRange)
      break;
      
    if (key.size != 15)
      continue;
    
    BarDate dt;
    QString k = (char *) key.data;
    if (dt.setDate(k))
      continue;

    if (dt.getDateValue() < ed.getDateValue() && dt.getDateValue() >= sd.getDateValue())
    {
      delete tbar;
      QString d = (char *) data.data;
      tbar = getBar(k, d);
	
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
      
      while (1)
      {
        ed.addSecs(-(mins * 60));
        sd.addSecs(-(mins * 60));
	if (dt.getDateValue() < ed.getDateValue() && dt.getDateValue() >= sd.getDateValue())
	  break;
      }
	
      bar->setTickFlag(barType);
      s = QString::number(ed.getDateValue(), 'f', 0);
      bar->setDate(s);
      
      delete tbar;
      QString d = (char *) data.data;
      tbar = getBar(k, d);
	
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

void DbPlugin::setHeaderField (int i, QString &d)
{
  QString s = d;
  QString k;
  if (! s.length())
    s = "";
    
  switch (i)
  {
    case BarType:
      k = "BarType";
      break;
    case Plugin:
      k = "Plugin";
      break;
    case Symbol:
      k = "Symbol";
      break;
    case Type:
      k = "Type";
      break;
    case Title:
      k = "Title";
      break;
    case Path:
      k = "Path";
      break;
    case CO:
      k = "CO";
      break;
    case LocalIndicators:
      k = "LocalIndicators";
      break;
    case QuotePlugin:
      k = "QuotePlugin";
      break;
    default:
      break;
  }

  if (k.length())  
    setData(k, s);
}

void DbPlugin::getHeaderField (int i, QString &d)
{
  QString k;
  
  switch (i)
  {
    case BarType:
      k = "BarType";
      break;
    case Plugin:
      k = "Plugin";
      break;
    case Symbol:
      k = "Symbol";
      break;
    case Type:
      k = "Type";
      break;
    case Title:
      k = "Title";
      break;
    case Path:
      k = "Path";
      break;
    case CO:
      k = "CO";
      break;
    case LocalIndicators:
      k = "LocalIndicators";
      break;
    case QuotePlugin:
      k = "QuotePlugin";
      break;
    default:
      break;
  }
  
  if (k.length())
    getData(k, d);
}

void DbPlugin::deleteIndicator (QString &d)
{
  QString s;
  getHeaderField(LocalIndicators, s);
  QStringList l = QStringList::split(",", s, FALSE);
  
  Setting set;
  for (QStringList::Iterator it = l.begin(); it != l.end(); ++it )
  {
    set.parse(*it);
    if (! set.getData("Name").compare(d))
    {
      l.remove(*it);
      break;
    }
  }
  
  if (l.count())
  {
    s = l.join(",");
    setHeaderField(LocalIndicators, s);
  }
  else
  {
    s = "";
    setHeaderField(LocalIndicators, s);
  }
}

void DbPlugin::addIndicator (QString &d)
{
  QString s;
  getHeaderField(LocalIndicators, s);
  QStringList l = QStringList::split(",", s, FALSE);
  
  l.append(d);
  s = l.join(",");
  setHeaderField(LocalIndicators, s);
}

void DbPlugin::setIndicator (QString &k, QString &d)
{
  QString s;
  getHeaderField(LocalIndicators, s);
  QStringList l = QStringList::split(",", s, FALSE);
  
  Setting set;
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    set.parse(l[loop]);
    if (! set.getData("Name").compare(k))
    {
      l[loop] = d;
      break;
    }
  }
  
  s = l.join(",");
  setHeaderField(LocalIndicators, s);
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

Bar * DbPlugin::getBar (QString &, QString &)
{
  return 0;
}

void DbPlugin::setBar (Bar &)
{
}
