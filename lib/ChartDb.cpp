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

ChartDb::ChartDb ()
{
  db = 0;
  details = 0;
}

ChartDb::~ChartDb ()
{
  if (db)
    db->close(db, 0);

  if (details)
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

QList<Setting> * ChartDb::getHistory (Compression c, QDateTime sd)
{
  compression = c;
  startDate = sd;

  switch (compression)
  {
    case Weekly:
      return getWeeklyHistory();
      break;
    case Monthly:
      return getMonthlyHistory();
      break;
    default:
      return getDailyHistory();
      break;
  }
}

QList<Setting> * ChartDb::getDailyHistory ()
{
  DBT key;
  DBT data;
  DBC *cursor;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  db->cursor(db, NULL, &cursor, 0);
  
  QList<Setting> *recordList = new QList<Setting>;
  recordList->setAutoDelete(TRUE);

  while (! cursor->c_get(cursor, &key, &data, DB_PREV))
  {
    QDateTime dt = getDateTime((char *) key.data);
    if (! dt.isValid())
      continue;

    if (dt < startDate)
      break;

    recordList->prepend(getRecord((char *) key.data, (char *) data.data));
  }

  cursor->c_close(cursor);
  
  return recordList;
}

QList<Setting> * ChartDb::getWeeklyHistory ()
{
  DBT key;
  DBT data;
  DBC *cursor;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  db->cursor(db, NULL, &cursor, 0);

  Setting *tr = 0;
  QDateTime tdate = QDateTime::currentDateTime();
  
  QList<Setting> *recordList = new QList<Setting>;
  recordList->setAutoDelete(TRUE);

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
        recordList->prepend(tr);

      tr = new Setting;
      tr->parse(r->getString());
      tdate = dt.addDays(- dt.date().dayOfWeek());
    }
    else
    {
      QString s = r->getData("Open");
      if (s.length())
        tr->setData("Open", r->getData("Open"));

      s = r->getData("High");
      if (s.length())
      {
        if (r->getFloat("High") > tr->getFloat("High"))
          tr->setData("High", r->getData("High"));
      }

      s = r->getData("Low");
      if (s.length())
      {
        if (r->getFloat("Low") < tr->getFloat("Low"))
          tr->setData("Low", r->getData("Low"));
      }

      s = r->getData("Volume");
      if (s.length())
        tr->setData("Volume", QString::number(tr->getFloat("Volume") + r->getFloat("Volume")));

      s = r->getData("Open Interest");
      if (s.length())
      {
        if (r->getFloat("Open Interest") > tr->getFloat("Open Interest"))
          tr->setData("Open Interest", r->getData("Open Interest"));
      }
    }

    delete r;
  }

  if (tr)
    recordList->prepend(tr);

  cursor->c_close(cursor);

  return recordList;
}

QList<Setting> * ChartDb::getMonthlyHistory ()
{
  DBT key;
  DBT data;
  DBC *cursor;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  db->cursor(db, NULL, &cursor, 0);

  int month = -1;
  Setting *tr = 0;

  QList<Setting> *recordList = new QList<Setting>;
  recordList->setAutoDelete(TRUE);

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
        recordList->prepend(tr);

      tr = new Setting;
      tr->parse(r->getString());
      month = dt.date().month();
    }
    else
    {
      QString s = r->getData("Open");
      if (s.length())
        tr->setData("Open", r->getData("Open"));

      s = r->getData("High");
      if (s.length())
      {
        if (r->getFloat("High") > tr->getFloat("High"))
          tr->setData("High", r->getData("High"));
      }

      s = r->getData("Low");
      if (s.length())
      {
        if (r->getFloat("Low") < tr->getFloat("Low"))
          tr->setData("Low", r->getData("Low"));
      }

      s = r->getData("Volume");
      if (s.length())
        tr->setData("Volume", QString::number(tr->getFloat("Volume") + r->getFloat("Volume")));

      s = r->getData("Open Interest");
      if (s.length())
      {
        if (r->getFloat("Open Interest") > tr->getFloat("Open Interest"))
          tr->setData("Open Interest", r->getData("Open Interest"));
      }
    }

    delete r;
  }

  if (tr)
    recordList->prepend(tr);

  cursor->c_close(cursor);

  return recordList;
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

void ChartDb::deleteRecord (Setting *set)
{
  QString date = set->getData("Date");
  if (date.length() != 14)
    return;

  deleteData(date);
  
  DBT key;
  DBT data;
  DBC *cursor;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  bool flag = FALSE;
  db->cursor(db, NULL, &cursor, 0);
  while (! cursor->c_get(cursor, &key, &data, DB_NEXT))
  {
    QDateTime dt = getDateTime((char *) key.data);
    if (! dt.isValid())
      continue;
    else
    {
      details->setData("First Date", dt.toString(DATE_FORMAT));
      flag = TRUE;
      break;
    }
  }
  cursor->c_close(cursor);
  if (! flag)
    details->remove("First Date");

  flag = FALSE;
  db->cursor(db, NULL, &cursor, 0);
  while (! cursor->c_get(cursor, &key, &data, DB_PREV))
  {
    QDateTime dt = getDateTime((char *) key.data);
    if (! dt.isValid())
      continue;
    else
    {
      details->setData("Last Date", dt.toString(DATE_FORMAT));
      flag = TRUE;
      break;
    }
  }
  cursor->c_close(cursor);
  if (! flag)
    details->remove("Last Date");

  saveDetails();
}

Setting * ChartDb::getRecord (QString k, QString d)
{
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
  setFormat();
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

void ChartDb::setFormat ()
{
  format = QStringList::split("|", details->getData("Format"), FALSE);
}




