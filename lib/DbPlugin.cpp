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

DbPlugin::DbPlugin ()
{
  db = 0;
  barCompression = BarData::DailyBar;
  barRange = 275;
}

DbPlugin::~DbPlugin ()
{
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

BarData * DbPlugin::getHistory ()
{
  DBT key;
  DBT data;
  DBC *cursor;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));
 
  BarData *barData = new BarData;
  if (barCompression >= BarData::DailyBar)
    barData->setBarType(BarData::Daily);
  else
    barData->setBarType(BarData::Tick);
  Bar *bar = 0;
  int barCount = 0;
  BarDate prevDate;

  db->cursor(db, NULL, &cursor, 0);
  
  while (! cursor->c_get(cursor, &key, &data, DB_PREV))
  {
    if (key.size != 15)
      continue;
    
    BarDate dt;
    if (dt.setDate((char *) key.data))
      continue;

    if (barCount >= barRange)
      break;

    if (! bar)
    {
      bar = getBar((char *) key.data, (char *) data.data);
      prevDate = getPrevDate(dt);
      continue;
    }
      
    if (dt.getDateValue() < prevDate.getDateValue())
    {
      barData->prepend(bar);
      barCount++;
      
      bar = getBar((char *) key.data, (char *) data.data);
      if (barData->getBarType() == BarData::Tick)
        bar->setDate(prevDate.getDateTimeString(FALSE));
      prevDate = getPrevDate(dt);
    }
    else
    {
      Bar *tbar = getBar((char *) key.data, (char *) data.data);
    
      bar->setOpen(tbar->getOpen());
      
      if (tbar->getHigh() > bar->getHigh())
        bar->setHigh(tbar->getHigh());
	
      if (tbar->getLow() < bar->getLow())
        bar->setLow(tbar->getLow());
	
      bar->setVolume(bar->getVolume() + tbar->getVolume());
	
      delete tbar;
    }
  }
  
  cursor->c_close(cursor);
  
  if (bar)
    barData->prepend(bar);
  
  barData->createDateList();
  
  return barData;
}

BarDate DbPlugin::getPrevDate (BarDate date)
{
  BarDate dt;
  dt.setDate(date.getDateTimeString(FALSE));
  QDate dt2;
  int m = 0;
  
  switch(barCompression)
  {
    case BarData::Minute5:
      m = (int) dt.getMinute() / 5;
      m = dt.getMinute() - (m * 5);
      if (m == 0)
        dt.subMinutes(5);
      else
        dt.subMinutes(m);
      break;
    case BarData::Minute15:
      m = (int) dt.getMinute() / 15;
      m = dt.getMinute() - (m * 15);
      if (m == 0)
        dt.subMinutes(15);
      else
        dt.subMinutes(m);
      break;
    case BarData::Minute30:
      m = (int) dt.getMinute() / 30;
      m = dt.getMinute() - (m * 30);
      if (m == 0)
        dt.subMinutes(30);
      else
        dt.subMinutes(m);
      break;
    case BarData::Minute60:
      m = (int) dt.getMinute() / 60;
      m = dt.getMinute() - (m * 60);
      if (m == 0)
        dt.subMinutes(60);
      else
        dt.subMinutes(m);
      break;
    case BarData::DailyBar:
      dt.setTime(0, 0, 0);
      break;
    case BarData::WeeklyBar:
      dt2 = dt.getDate();
      dt2 = dt2.addDays(-(dt2.dayOfWeek() - 1));
      dt.setDate(dt2);
      dt.setTime(0, 0, 0);
      break;
    case BarData::MonthlyBar:
      dt.setDate(QDate(dt.getDate().year(), dt.getDate().month(), 1));
      dt.setTime(0, 0, 0);
      break;
    default:
      break;
  }
  
  return dt;
}

QStringList DbPlugin::getChartObjectsList ()
{
  QStringList l;
  QString s = getData("CHARTOBJECTS");
  if (s.length())
    l = QStringList::split(",", s, FALSE);
  return l;
}

QPtrList<Setting> DbPlugin::getChartObjects ()
{
  QPtrList<Setting> list;
  list.setAutoDelete(TRUE);

  QStringList l = getChartObjectsList();
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    Setting *set = new Setting;
    set->parse(getData(l[loop]));
    list.append(set);
  }  
  
  return list;
}

void DbPlugin::setChartObject (QString d, Setting *set)
{
  QStringList l = getChartObjectsList();
  
  if (l.findIndex(d) == -1)
  {
    l.append(d);
    setData("CHARTOBJECTS", l.join(","));
  }
  
  setData(d, set->getString());
}

void DbPlugin::deleteChartObject (QString d)
{
  QStringList l = getChartObjectsList();
  l.remove(d);
  if (l.count())
    setData("CHARTOBJECTS", l.join(","));
  else
    setData("CHARTOBJECTS", "");

  deleteData(d);
}

void DbPlugin::setBarCompression (BarData::BarCompression d)
{
  barCompression = d;
}

void DbPlugin::setBarRange (int d)
{
  barRange = d;
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

void DbPlugin::setDb (DB *d)
{
  db = d;
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

//*********************************************************
//***************** VIRTUAL OVERRIDES *********************
//*********************************************************

Bar * DbPlugin::getBar (QString, QString)
{
  return 0;
}

void DbPlugin::setBar (Bar *)
{
}

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



