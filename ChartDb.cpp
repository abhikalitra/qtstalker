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

ChartDb::ChartDb ()
{
  recordList.setAutoDelete(TRUE);
  db = 0;
}

ChartDb::~ChartDb ()
{
  if (db)
    db->close(db, 0);
}

int ChartDb::openChart ()
{
  char file[path.length() + 2];
  strcpy (file, path);

  if (db_open(file, DB_BTREE, DB_CREATE, 0664, NULL, NULL, &db) != 0)
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

  char str[k.length() + 2];
  strcpy (str, k);

  key.data = str;
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

  char str[k.length() + 2];
  strcpy (str, k);
  key.data = str;
  key.size = k.length() + 1;

  char str2[d.length() + 2];
  strcpy (str2, d);
  data.data = str2;
  data.size = d.length() + 1;

  db->put(db, NULL, &key, &data, 0);
}

void ChartDb::deleteData (QString k)
{
  DBT key;
  memset(&key, 0, sizeof(DBT));

  char str[k.length() + 2];
  strcpy (str, k);
  key.data = str;
  key.size = k.length() + 1;

  db->del(db, NULL, &key, 0);
}

void ChartDb::setPath (QString p)
{
  path = p;
}

QStringList ChartDb::getKeyList ()
{
  DBT key;
  DBT data;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  QStringList list;

  db->cursor(db, NULL, &dbc, 0);

  while (! dbc->c_get(dbc, &key, &data, DB_NEXT))
    list.append((char *) key.data);

  dbc->c_close(dbc);

  return list;
}

void ChartDb::getHistory (Compression c, QDateTime sd)
{
  compression = c;
  startDate = sd;

  Setting *set = getDetails();
  QString s = set->getData("Chart Type");
  delete set;
  if (s.contains(QObject::tr("Stock")) || s.contains(QObject::tr("Future")))
  {
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

    return;
  }

  if (! s.compare(QObject::tr("Ratio")))
  {
    getRatio();
    return;
  }

  if (! s.compare(QObject::tr("Spread")))
  {
    getSpread();
    return;
  }

  if (! s.compare(QObject::tr("Index")))
    getIndex();
}

void ChartDb::getDailyHistory ()
{
  DBT key;
  DBT data;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  db->cursor(db, NULL, &dbc, 0);

  while (! dbc->c_get(dbc, &key, &data, DB_PREV))
  {
    QDateTime dt = getDateTime((char *) key.data);
    if (! dt.isValid())
      continue;

    if (dt < startDate)
      break;

    recordList.prepend(getRecord((char *) key.data, (char *) data.data));
  }

  dbc->c_close(dbc);
}

void ChartDb::getWeeklyHistory ()
{
  DBT key;
  DBT data;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  db->cursor(db, NULL, &dbc, 0);

  bool flag = FALSE;
  Setting *tr = 0;
  QDateTime tdate;

  while (! dbc->c_get(dbc, &key, &data, DB_PREV))
  {
    QDateTime dt = getDateTime((char *) key.data);
    if (! dt.isValid())
      continue;

    if (dt < startDate)
      break;

    Setting *r = getRecord((char *) key.data, (char *) data.data);

    if (! flag)
    {
      tr = new Setting;
      tr->set("Date", r->getData("Date"), Setting::Date);
      tr->set("Open", r->getData("Open"), Setting::Float);
      tr->set("High", r->getData("High"), Setting::Float);
      tr->set("Low", r->getData("Low"), Setting::Float);
      tr->set("Close", r->getData("Close"), Setting::Float);
      tr->set("Volume", r->getData("Volume"), Setting::Float);
      tr->set("OI", r->getData("OI"), Setting::Float);
      flag = TRUE;
      tdate = dt.addDays(0 - dt.date().dayOfWeek());
    }
    else
    {
      if (dt < tdate)
      {
        recordList.prepend(tr);
	flag = FALSE;
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
    }

    delete r;
  }

  if (tr)
    recordList.prepend(tr);

  dbc->c_close(dbc);
}

void ChartDb::getMonthlyHistory ()
{
  DBT key;
  DBT data;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  db->cursor(db, NULL, &dbc, 0);

  int month = -1;
  Setting *tr = 0;

  while (! dbc->c_get(dbc, &key, &data, DB_PREV))
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

  dbc->c_close(dbc);
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

QDateTime ChartDb::getLastRecord ()
{
  QDateTime dt;

  DBT key;
  DBT data;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  db->cursor(db, NULL, &dbc, 0);

  while (! dbc->c_get(dbc, &key, &data, DB_PREV))
  {
    dt = getDateTime((char *) key.data);
    if (dt.isValid())
      break;
  }

  dbc->c_close(dbc);

  return dt;
}

QDateTime ChartDb::getFirstRecord ()
{
  QDateTime dt;

  DBT key;
  DBT data;
  memset(&key, 0, sizeof(DBT));
  memset(&data, 0, sizeof(DBT));

  db->cursor(db, NULL, &dbc, 0);

  while (! dbc->c_get(dbc, &key, &data, DB_NEXT))
  {
    dt = getDateTime((char *) key.data);
    if (dt.isValid())
      break;
  }

  dbc->c_close(dbc);

  return dt;
}

void ChartDb::setRecord (Setting *set)
{
  QString date = set->getData("Date");
  if (date.length() != 14)
    return;

  QStringList l;
  l.append(set->getData("Open"));
  l.append(set->getData("High"));
  l.append(set->getData("Low"));
  l.append(set->getData("Close"));
  l.append(set->getData("Volume"));
  l.append(set->getData("Open Interest"));

  setData(date, l.join(","));

  Setting *details = getDetails();

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

  setDetails(details);
  delete details;
}

Setting * ChartDb::getRecord (QString k, QString d)
{
  QStringList l = QStringList::split(",", d, FALSE);

  Setting *r = new Setting;
  r->set("Date", k, Setting::Date);
  r->set("Open", l[0], Setting::Float);
  r->set("High", l[1], Setting::Float);
  r->set("Low", l[2], Setting::Float);
  r->set("Close", l[3], Setting::Float);
  r->set("Volume", l[4], Setting::Float);
  r->set("Open Interest", l[5], Setting::Float);

  return r;
}

void ChartDb::getSpread ()
{
  QDict<Setting> data;
  data.setAutoDelete(TRUE);

  QString base = path;
  int i = base.findRev('/', -1, TRUE);
  base.truncate(i + 1);

  Setting *comp = getComposite();

  int loop;
  for (loop = 0; loop < 2; loop++)
  {
    QString s = base;
    QStringList l;
    if (loop == 0)
      l = QStringList::split(" ", comp->getData("1"), FALSE);
    else
      l = QStringList::split(" ", comp->getData("2"), FALSE);
    s.append(l[0]);
    float weight = l[1].toFloat();

    ChartDb *tdb = new ChartDb();
    tdb->setPath(s);
    if (tdb->openChart())
    {
      qDebug("could not open db");
      delete tdb;
      return;
    }

    tdb->getHistory(compression, startDate);

    int loop2;
    for (loop2 = 0; loop2 < tdb->getDataSize(); loop2++)
    {
      Setting *tr = tdb->getRecordIndex(loop2);
      Setting *r = data.find(tr->getData("Date"));
      if (! r)
      {
        r = new Setting;
	r->set("Date", tr->getData("Date"), Setting::Date);
	r->set("Close", QString::number(tr->getFloat("Close") * weight), Setting::Float);
	r->set("Volume", QString::number(tr->getFloat("Volume") * weight), Setting::Float);
	r->set("Open Interest", QString::number(tr->getFloat("Open Interest") * weight), Setting::Float);
	r->set("Open", r->getData("Close"), Setting::Float);
	r->set("High", r->getData("Close"), Setting::Float);
	r->set("Low", r->getData("Close"), Setting::Float);
	r->set("Count", "1", Setting::Integer);
        data.insert(r->getData("Date"), r);
      }
      else
      {
	r->setData("Close", QString::number(r->getFloat("Close") - (tr->getFloat("Close") * weight)));
	r->setData("Volume", QString::number(r->getFloat("Volume") - (tr->getFloat("Volume") * weight)));
	r->setData("Open Interest", QString::number(r->getFloat("Open Interest") - (tr->getFloat("Open Interest") * weight)));
	r->setData("Open", r->getData("Close"));
	r->setData("High", r->getData("Close"));
	r->setData("Low", r->getData("Close"));
	r->setData("Count", QString::number(r->getFloat("Count") + 1));
      }
    }

    delete tdb;
  }

  QDateTime tdate = startDate;
  QDateTime lastDate = QDateTime::currentDateTime();

  while (1)
  {
    if (tdate > lastDate)
      break;

    Setting *r = data.find(tdate.toString(DATE_FORMAT));
    if (r)
    {
      if (r->getFloat("Count") == 2)
      {
        Setting *r2 = new Setting;
	r2->set("Date", r->getData("Date"), Setting::Date);
	r2->set("Close", r->getData("Close"), Setting::Float);
	r2->set("Volume", r->getData("Volume"), Setting::Float);
	r2->set("Open Interest", r->getData("Open Interest"), Setting::Float);
	r2->set("Open", r->getData("Close"), Setting::Float);
	r2->set("High", r->getData("Close"), Setting::Float);
	r2->set("Low", r->getData("Close"), Setting::Float);
	
        recordList.append(r2);
      }
    }

    tdate = tdate.addDays(1);
    if (tdate.date().dayOfWeek() == 6)
      tdate = tdate.addDays(2);
    else
    {
      if (tdate.date().dayOfWeek() == 7)
        tdate = tdate.addDays(1);
    }
  }
}

void ChartDb::getRatio ()
{
  QDict<Setting> data;
  data.setAutoDelete(TRUE);

  QString base = path;
  int i = base.findRev('/', -1, TRUE);
  base.truncate(i + 1);

  Setting *comp = getComposite();

  int loop;
  for (loop = 0; loop < 2; loop++)
  {
    QString s = base;
    QStringList l;
    if (loop == 0)
      l = QStringList::split(" ", comp->getData("1"), FALSE);
    else
      l = QStringList::split(" ", comp->getData("2"), FALSE);
    s.append(l[0]);
    float weight = l[1].toFloat();

    ChartDb *tdb = new ChartDb();
    tdb->setPath(s);
    if (tdb->openChart())
    {
      qDebug("could not open db");
      delete tdb;
      return;
    }

    tdb->getHistory(compression, startDate);

    int loop2;
    for (loop2 = 0; loop2 < tdb->getDataSize(); loop2++)
    {
      Setting *tr = tdb->getRecordIndex(loop2);
      Setting *r = data.find(tr->getData("Date"));
      if (! r)
      {
        r = new Setting;
	r->set("Date", tr->getData("Date"), Setting::Date);
	r->set("Close", QString::number(tr->getFloat("Close") * weight), Setting::Float);
	r->set("Volume", QString::number(tr->getFloat("Volume") * weight), Setting::Float);
	r->set("Open Interest", QString::number(tr->getFloat("Open Interest") * weight), Setting::Float);
	r->set("Open", r->getData("Close"), Setting::Float);
	r->set("High", r->getData("Close"), Setting::Float);
	r->set("Low", r->getData("Close"), Setting::Float);
	r->set("Count", "1", Setting::Integer);
        data.insert(r->getData("Date"), r);
      }
      else
      {
	r->setData("Close", QString::number(r->getFloat("Close") / (tr->getFloat("Close") * weight)));
	r->setData("Volume", QString::number(r->getFloat("Volume") / (tr->getFloat("Volume") * weight)));
	r->setData("Open Interest", QString::number(r->getFloat("Open Interest") / (tr->getFloat("Open Interest") * weight)));
	r->setData("Open", r->getData("Close"));
	r->setData("High", r->getData("Close"));
	r->setData("Low", r->getData("Close"));
	r->setData("Count", QString::number(r->getFloat("Count") + 1));
      }
    }

    delete tdb;
  }

  QDateTime tdate = startDate;
  QDateTime lastDate = QDateTime::currentDateTime();

  while (1)
  {
    if (tdate > lastDate)
      break;

    Setting *r = data.find(tdate.toString(DATE_FORMAT));
    if (r)
    {
      if (r->getFloat("Count") == 2)
      {
        Setting *r2 = new Setting;
	r2->set("Date", r->getData("Date"), Setting::Date);
	r2->set("Close", r->getData("Close"), Setting::Float);
	r2->set("Volume", r->getData("Volume"), Setting::Float);
	r2->set("Open Interest", r->getData("Open Interest"), Setting::Float);
	r2->set("Open", r->getData("Close"), Setting::Float);
	r2->set("High", r->getData("Close"), Setting::Float);
	r2->set("Low", r->getData("Close"), Setting::Float);

        recordList.append(r2);
      }
    }

    tdate = tdate.addDays(1);
    if (tdate.date().dayOfWeek() == 6)
      tdate = tdate.addDays(2);
    else
    {
      if (tdate.date().dayOfWeek() == 7)
        tdate = tdate.addDays(1);
    }
  }
}

void ChartDb::getIndex ()
{
  QDict<Setting> data;
  data.setAutoDelete(TRUE);

  QString base = path;
  int i = base.findRev('/', -1, TRUE);
  base.truncate(i + 1);

  Setting *comp = getComposite();
  QStringList key = comp->getKeyList();

  int loop;
  for (loop = 0; loop < (int) key.count(); loop++)
  {
    QStringList l = QStringList::split(" ", comp->getData(key[loop]), FALSE);

    QString s = base;
    s.append(l[0]);

    float weight = l[1].toFloat();

    ChartDb *tdb = new ChartDb();
    tdb->setPath(s);
    if (tdb->openChart())
    {
      qDebug("could not open db");
      delete tdb;
      return;
    }

    tdb->getHistory(compression, startDate);

    int loop2;
    for (loop2 = 0; loop2 < tdb->getDataSize(); loop2++)
    {
      Setting *tr = tdb->getRecordIndex(loop2);
      Setting *r = data.find(tr->getData("Date"));
      if (! r)
      {
        r = new Setting;
	r->set("Date", tr->getData("Date"), Setting::Date);
	r->set("Close", QString::number(tr->getFloat("Close") * weight), Setting::Float);
	r->set("Volume", QString::number(tr->getFloat("Volume") * weight), Setting::Float);
	r->set("Open Interest", QString::number(tr->getFloat("Open Interest") * weight), Setting::Float);
	r->set("Open", QString::number(tr->getFloat("Open") * weight), Setting::Float);
	r->set("High", QString::number(tr->getFloat("High") * weight), Setting::Float);
	r->set("Low", QString::number(tr->getFloat("Low") * weight), Setting::Float);
	r->set("Count", "1", Setting::Integer);
        data.insert(r->getData("Date"), r);
      }
      else
      {
	r->setData("Close", QString::number(r->getFloat("Close") + (tr->getFloat("Close") * weight)));
	r->setData("Volume", QString::number(r->getFloat("Volume") + (tr->getFloat("Volume") * weight)));
	r->setData("Open Interest", QString::number(r->getFloat("Open Interest") + (tr->getFloat("Open Interest") * weight)));
	r->setData("Open", QString::number(r->getFloat("Open") + (tr->getFloat("Open") * weight)));
	r->setData("High", QString::number(r->getFloat("High") + (tr->getFloat("High") * weight)));
	r->setData("Low", QString::number(r->getFloat("Low") + (tr->getFloat("Low") * weight)));
	r->setData("Count", QString::number(r->getFloat("Count") + 1));
      }
    }

    delete tdb;
  }

  QDateTime tdate = startDate;
  QDateTime lastDate = QDateTime::currentDateTime();
  int count = key.count();

  while (1)
  {
    if (tdate > lastDate)
      break;

    Setting *r = data.find(tdate.toString(DATE_FORMAT));
    if (r)
    {
      if (r->getFloat("Count") == count)
      {
        Setting *r2 = new Setting;
	r2->set("Date", r->getData("Date"), Setting::Date);
	r2->set("Close", QString::number(r->getFloat("Close") / count), Setting::Float);
	r2->set("Volume", QString::number(r->getFloat("Volume") / count), Setting::Float);
	r2->set("Open Interest", QString::number(r->getFloat("Open Interest") / count), Setting::Float);
	r2->set("Open", QString::number(r->getFloat("Open") / count), Setting::Float);
	r2->set("High", QString::number(r->getFloat("High") / count), Setting::Float);
	r2->set("Low", QString::number(r->getFloat("Low") / count), Setting::Float);

	if (r2->getFloat("Open") > r2->getFloat("High"))
	  r2->setData("High", r2->getData("Open"));
	if (r2->getFloat("Open") < r2->getFloat("Low"))
	  r2->setData("Low", r2->getData("Open"));

	if (r2->getFloat("Close") > r2->getFloat("High"))
	  r2->setData("High", r2->getData("Close"));
	if (r2->getFloat("Close") < r2->getFloat("Low"))
	  r2->setData("Low", r2->getData("Close"));

        recordList.append(r2);
      }
    }

    tdate = tdate.addDays(1);
    if (tdate.date().dayOfWeek() == 6)
      tdate = tdate.addDays(2);
    else
    {
      if (tdate.date().dayOfWeek() == 7)
        tdate = tdate.addDays(1);
    }
  }
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

void ChartDb::setComposite (Setting *set)
{
  setData("COMPOSITE_LIST", set->getString());
}

Setting * ChartDb::getComposite ()
{
  Setting *set = new Setting;
  set->parse(getData("COMPOSITE_LIST"));
  return set;
}

Setting * ChartDb::getRecordIndex (int d)
{
  return recordList.at(d);
}

QList<Setting> ChartDb::getRecordList ()
{
  return recordList;
}

void ChartDb::setDetails (Setting *d)
{
  setData("DETAILS", d->getString());
}

Setting * ChartDb::getDetails ()
{
  Setting *set = new Setting;
  set->parse(getData("DETAILS"));
  return set;
}



