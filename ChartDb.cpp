/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

ChartDb::ChartDb ()
{
  recordList.setAutoDelete(TRUE);
  db = 0;
}

ChartDb::~ChartDb ()
{
  if (db)
    db->close(db, 0);

  delete details;
}

int ChartDb::openChart (QString path)
{
  if (db_open((char *) path.latin1(), DB_BTREE, DB_CREATE, 0664, NULL, NULL, &db) != 0)
    return TRUE;
  else
  {
    loadDetails();
    return FALSE;
  }
}

QString ChartDb::getData (QString k)
{
  DBT key;
  DBT data;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  key.data = (char *) k.latin1();
  key.size = k.length() + 1;

  db->get(db, NULL, &key, &data, 0);

  QString s;
  if (data.size)
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

void ChartDb::getHistory (Compression c, QDateTime sd)
{
  compression = c;
  startDate = sd;

  QStringList format = QStringList::split("|", details->getData("Format"), FALSE);
  if (format.findIndex("Open") == -1)
  {
    getDailyHistory();
    return;
  }

  switch (compression)
  {
    case Weekly:
      getWeeklyHistory();
      break;
    case Monthly:
      getMonthlyHistory();
      break;
    default:
      getDailyHistory();
      break;
  }
}

void ChartDb::getDailyHistory ()
{
  DBT key;
  DBT data;
  DBC *cursor;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  db->cursor(db, NULL, &cursor, 0);

  while (! cursor->c_get(cursor, &key, &data, DB_PREV))
  {
    QDateTime dt = getDateTime((char *) key.data);
    if (! dt.isValid())
      continue;

    if (dt < startDate)
      break;

    recordList.prepend(getRecord((char *) key.data, (char *) data.data));
  }

  cursor->c_close(cursor);
}

void ChartDb::getWeeklyHistory ()
{
  DBT key;
  DBT data;
  DBC *cursor;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  db->cursor(db, NULL, &cursor, 0);

  Setting *tr = 0;
  QDateTime tdate = QDateTime::currentDateTime();
  
  while (! cursor->c_get(cursor, &key, &data, DB_PREV))
  {
    QDateTime dt = getDateTime((char *) key.data);
    if (! dt.isValid())
      continue;

    if (dt < startDate)
      break;

    Setting *r = getRecord((char *) key.data, (char *) data.data);

    if (dt <= tdate)
    {
      if (tr)
        recordList.prepend(tr);

      tr = new Setting;
      tr->set("Date", r->getData("Date"), Setting::Date);
      tr->set("Open", r->getData("Open"), Setting::Float);
      tr->set("High", r->getData("High"), Setting::Float);
      tr->set("Low", r->getData("Low"), Setting::Float);
      tr->set("Close", r->getData("Close"), Setting::Float);
      tr->set("Volume", r->getData("Volume"), Setting::Float);
      tr->set("OI", r->getData("OI"), Setting::Float);
      tdate = dt.addDays(- dt.date().dayOfWeek());
    }
    else
    {
      tr->setData("Open", r->getData("Open"));

      if (r->getFloat("High") > tr->getFloat("High"))
        tr->setData("High", r->getData("High"));

      if (r->getFloat("Low") < tr->getFloat("Low"))
        tr->setData("Low", r->getData("Low"));

      tr->setData("Volume", QString::number(tr->getFloat("Volume") + r->getFloat("Volume")));

      if (r->getFloat("OI") > tr->getFloat("OI"))
        tr->setData("OI", r->getData("OI"));
    }

    delete r;
  }

  if (tr)
    recordList.prepend(tr);

  cursor->c_close(cursor);
}

void ChartDb::getMonthlyHistory ()
{
  DBT key;
  DBT data;
  DBC *cursor;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  db->cursor(db, NULL, &cursor, 0);

  int month = -1;
  Setting *tr = 0;

  while (! cursor->c_get(cursor, &key, &data, DB_PREV))
  {
    QDateTime dt = getDateTime((char *) key.data);
    if (! dt.isValid())
      continue;

    if (dt < startDate)
      break;

    Setting *r = getRecord((char *) key.data, (char *) data.data);

    if (dt.date().month() != month)
    {
      if (tr)
        recordList.prepend(tr);

      tr = new Setting;
      tr->set("Date", r->getData("Date"), Setting::Date);
      tr->set("Open", r->getData("Open"), Setting::Float);
      tr->set("High", r->getData("High"), Setting::Float);
      tr->set("Low", r->getData("Low"), Setting::Float);
      tr->set("Close", r->getData("Close"), Setting::Float);
      tr->set("Volume", r->getData("Volume"), Setting::Float);
      tr->set("OI", r->getData("OI"), Setting::Float);
      month = dt.date().month();
    }
    else
    {
      tr->setData("Open", r->getData("Open"));

      if (r->getFloat("High") > tr->getFloat("High"))
        tr->setData("High", r->getData("High"));

      if (r->getFloat("Low") < tr->getFloat("Low"))
        tr->setData("Low", r->getData("Low"));

      tr->setData("Volume", QString::number(tr->getFloat("Volume") + r->getFloat("Volume")));

      if (r->getFloat("OI") > tr->getFloat("OI"))
        tr->setData("OI", r->getData("OI"));
    }

    delete r;
  }

  if (tr)
    recordList.prepend(tr);

  cursor->c_close(cursor);
}

QDateTime ChartDb::getDateTime (QString d)
{
  QDateTime dt;

  if (d.length() != 14)
    return dt;

  QString s = d;
  s.insert(4, "-");
  s.insert(7, "-");
  s.insert(12, ":");
  s.insert(15, ":");
  dt = QDateTime::fromString(s, Qt::ISODate);

  return dt;
}

void ChartDb::setRecord (Setting *set)
{
  QString date = set->getData("Date");
  if (date.length() != 14)
    return;

  QStringList format = QStringList::split("|", details->getData("Format"), FALSE);
  QStringList l;
  int loop;
  for (loop = 0; loop < (int) format.count(); loop++)
    l.append(set->getData(format[loop]));

  setData(date, l.join(","));

  QDateTime dt = getDateTime(date);

  QDateTime dt2 = getDateTime(details->getData("First Date"));
  if (dt2.isValid())
  {
    if (dt < dt2)
      details->setData("First Date", dt.toString(DATE_FORMAT));
  }
  else
    details->set("First Date", dt.toString(DATE_FORMAT), Setting::None);

  dt2 = getDateTime(details->getData("Last Date"));
  if (dt2.isValid())
  {
    if (dt > dt2)
      details->setData("Last Date", dt.toString(DATE_FORMAT));
  }
  else
    details->set("Last Date", dt.toString(DATE_FORMAT), Setting::None);

  saveDetails();
}

Setting * ChartDb::getRecord (QString k, QString d)
{
  QStringList format = QStringList::split("|", details->getData("Format"), FALSE);

  QStringList l = QStringList::split(",", d, FALSE);

  Setting *r = new Setting;
  r->set("Date", k, Setting::Date);

  int loop;
  for (loop = 0; loop < (int) format.count(); loop++)
    r->set(format[loop], l[loop], Setting::Float);

  return r;
}

QStringList ChartDb::getChartObjects ()
{
  QStringList l;
  QString s = getData("CHART_OBJECT_LIST");
  if (s.length())
    return QStringList::split(",", s, FALSE);
  else
    return l;
}

Setting * ChartDb::getChartObject (QString d)
{
  Setting *set = 0;
  QString s = "CHART_OBJECT_";
  s.append(d);
  s = getData(s);
  if (s.length())
  {
    set = new Setting;
    set->parse(s);
  }

  return set;
}

void ChartDb::setChartObject (QString d, Setting *set)
{
  QString s = "CHART_OBJECT_";
  s.append(d);
  setData(s, set->getString());

  QStringList l = getChartObjects();
  if (l.findIndex(d) == -1)
  {
    l.append(d);
    setData("CHART_OBJECT_LIST", l.join(","));
  }
}

void ChartDb::deleteChartObject (QString d)
{
  QStringList l = getChartObjects();
  l.remove(d);
  if (l.count())
    setData("CHART_OBJECT_LIST", l.join(","));
  else
    setData("CHART_OBJECT_LIST", "");

  QString s = "CHART_OBJECT_";
  s.append(d);
  deleteData(s);
}

int ChartDb::getDataSize ()
{
  return (int) recordList.count();
}

float ChartDb::getCloseData (QDateTime d)
{
  QString key = d.toString(DATE_FORMAT);
  QString s = getData(key);
  Setting *r = getRecord(key, s);
  float t = 0;
  if (r)
  {
    t = r->getFloat("Close");
    delete r;
  }

  return t;
}

Setting * ChartDb::getRecordIndex (int d)
{
  return recordList.at(d);
}

QList<Setting> ChartDb::getRecordList ()
{
  return recordList;
}

void ChartDb::saveDetails ()
{
  setData("DETAILS", details->getString());
}

Setting * ChartDb::getDetails ()
{
  return details;
}

void ChartDb::loadDetails ()
{
  details = new Setting;
  details->parse(getData("DETAILS"));
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



