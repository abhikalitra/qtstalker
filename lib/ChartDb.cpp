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

#include "ChartDb.h"
#include <qobject.h>
#include <qdict.h>
#include <qdatetime.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qdir.h>

ChartDb::ChartDb ()
{
  db = 0;
  barCompression = Daily;
  barRange = 275;
}

ChartDb::~ChartDb ()
{
  if (db)
    db->close(db, 0);
}

int ChartDb::openChart (QString path)
{
  if (db_open((char *) path.latin1(), DB_BTREE, DB_CREATE, 0664, NULL, NULL, &db) != 0)
    return TRUE;
  else
    return FALSE;
}

QString ChartDb::getData (QString k)
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

void ChartDb::setData (QString k, QString d)
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

void ChartDb::deleteData (QString k)
{
  DBT key;
  memset(&key, 0, sizeof(DBT));

  key.data = (char *) k.latin1();
  key.size = k.length() + 1;

  db->del(db, NULL, &key, 0);
}

BarData * ChartDb::getHistory ()
{
  DBT key;
  DBT data;
  DBC *cursor;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));
  
  BarData *barData = new BarData;
  QString s = getDetail(ChartDb::BarType);
  barData->setBarType((BarData::BarType) s.toInt());
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
	
      bar->setOI(bar->getOI() + tbar->getOI());
	
      delete tbar;
    }
  }
  
  if (bar)
    barData->prepend(bar);
  
  cursor->c_close(cursor);
  
  barData->createDateList();
  
  return barData;
}

BarDate ChartDb::getPrevDate (BarDate date)
{
  BarDate dt;
  dt.setDate(date.getDateTimeString(FALSE));
  QDate dt2;
  
  switch(barCompression)
  {
    case Minute5:
      dt.setTime(date.getHour(), 0, 0);
      while (dt.getDateValue() < date.getDateValue())
        dt.addMinutes(5);
      dt.subMinutes(5);
      break;
    case Minute15:
      dt.setTime(date.getHour(), 0, 0);
      while (dt.getDateValue() < date.getDateValue())
        dt.addMinutes(15);
      dt.subMinutes(15);
      break;
    case Minute30:
      if (dt.getMinute() < 30)
        dt.setTime(dt.getHour(), 0, 0);
      else
        dt.setTime(dt.getHour(), 30, 0);
      break;
    case Minute60:
      dt.setTime(dt.getHour(), 0, 0);
      break;
    case Daily:
      dt.setTime(0, 0, 0);
      break;
    case Weekly:
      dt2 = dt.getDate();
      dt2 = dt2.addDays(-(dt2.dayOfWeek() - 1));
      dt.setDate(dt2);
      dt.setTime(0, 0, 0);
      break;
    case Monthly:
      dt.setDate(QDate(dt.getDate().year(), dt.getDate().month(), 1));
      dt.setTime(0, 0, 0);
      break;
    default:
      break;
  }
  
  return dt;
}

Bar * ChartDb::getBar (QString k, QString d)
{
  QStringList l = QStringList::split(",", d, FALSE);

  Bar *bar = new Bar;
  bar->setDate(k);
  bar->setOpen(l[0].toDouble());
  bar->setHigh(l[1].toDouble());
  bar->setLow(l[2].toDouble());
  bar->setClose(l[3].toDouble());
  bar->setVolume(l[4].toDouble());
  bar->setOI(l[5].toInt());
  
  return bar;
}

void ChartDb::setBar (Bar *bar)
{
  QString k = bar->getDate().getDateTimeString(FALSE);
  QStringList l;
  l.append(QString::number(bar->getOpen()));
  l.append(QString::number(bar->getHigh()));
  l.append(QString::number(bar->getLow()));
  l.append(QString::number(bar->getClose()));
  l.append(QString::number(bar->getVolume(), 'f', 0));
  l.append(QString::number(bar->getOI()));
  QString d = l.join(",");  
  
  setData(k, d);
}

QStringList ChartDb::getChartObjectsList ()
{
  QStringList l;
  QString s = getData("CHARTOBJECTS");
  if (s.length())
    l = QStringList::split(",", s, FALSE);
  return l;
}

QList<Setting> *ChartDb::getChartObjects ()
{
  QList<Setting> *list = new QList<Setting>;
  list->setAutoDelete(TRUE);

  QStringList l = getChartObjectsList();
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    Setting *set = new Setting;
    set->parse(getData(l[loop]));
    list->append(set);
  }  
  
  return list;
}

void ChartDb::setChartObject (QString d, Setting *set)
{
  QStringList l = getChartObjectsList();
  
  if (l.findIndex(d) == -1)
  {
    l.append(d);
    setData("CHARTOBJECTS", l.join(","));
  }
  
  setData(d, set->getString());
}

void ChartDb::deleteChartObject (QString d)
{
  QStringList l = getChartObjectsList();
  l.remove(d);
  if (l.count())
    setData("CHARTOBJECTS", l.join(","));
  else
    setData("CHARTOBJECTS", "");

  deleteData(d);
}

void ChartDb::dump (QString d)
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
    outStream << (char *) key.data << "=" << (char *) data.data << "\n";

  cursor->c_close(cursor);
  outFile.close();
}

QStringList ChartDb::getBarCompressionList ()
{
  QStringList l;
  l.append(QObject::tr("5 Minute"));
  l.append(QObject::tr("15 Minute"));
  l.append(QObject::tr("30 Minute"));
  l.append(QObject::tr("60 Minute"));
  l.append(QObject::tr("Daily"));
  l.append(QObject::tr("Weekly"));
  l.append(QObject::tr("Monthly"));
  return l;  
}

void ChartDb::setBarCompression (ChartDb::BarCompression d)
{
  barCompression = d;
}

ChartDb::BarCompression ChartDb::getBarCompression ()
{
  return barCompression;
}

void ChartDb::setBarRange (int d)
{
  barRange = d;
}

int ChartDb::getBarRange ()
{
  return barRange;
}

Bar * ChartDb::getLastBar ()
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

Bar * ChartDb::getFirstBar ()
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

void ChartDb::setDetail (Detail k, QString d)
{
  switch (k)
  {
    case Symbol:
      setData("Symbol", d);
      break;
    case Title:
      setData("Title", d);
      break;
    case Type:
      setData("Type", d);
      break;
    case FuturesType:
      setData("FuturesType", d);
      break;
    case FuturesMonth:
      setData("FuturesMonth", d);
      break;
    case BarType:
      setData("BarType", d);
      break;
    default:
      break;
  }
}

QString ChartDb::getDetail (Detail k)
{
  QString s;
  
  switch (k)
  {
    case Symbol:
      s = getData("Symbol");
      break;
    case Title:
      s = getData("Title");
      break;
    case Type:
      s = getData("Type");
      break;
    case FuturesType:
      s = getData("FuturesType");
      break;
    case FuturesMonth:
      s = getData("FuturesMonth");
      break;
    case BarType:
      s = getData("BarType");
      break;
    default:
      break;
  }
  
  return s;
}

