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
    gdbm_close(db);
}

int ChartDb::openChart ()
{
  char file[path.length() + 2];
  strcpy (file, path);

  db = gdbm_open(file, 512, GDBM_WRCREAT, 0664, 0);

  if (db)
    return FALSE;
  else
    return FALSE;
}

QString ChartDb::getData (QString k)
{
  char str[k.length() + 2];
  strcpy (str, k);

  datum key;
  key.dptr = str;
  key.dsize = k.length() + 1;

  datum data = gdbm_fetch(db, key);

  QString s;
  if (data.dptr)
  {
    s = data.dptr;
    delete data.dptr;
  }

  return s;
}

void ChartDb::setData (QString k, QString d)
{
  char str[k.length() + 2];
  strcpy (str, k);
  datum key;
  key.dptr = str;
  key.dsize = k.length() + 1;

  char str2[d.length() + 2];
  strcpy (str2, d);
  datum data;
  data.dptr = str2;
  data.dsize = d.length() + 1;

  gdbm_store(db, key, data, GDBM_REPLACE);
}

void ChartDb::deleteData (QString k)
{
  char str[k.length() + 2];
  strcpy (str, k);
  datum key;
  key.dptr = str;
  key.dsize = k.length() + 1;

  gdbm_delete(db, key);
}

void ChartDb::setPath (QString p)
{
  path = p;
}

QStringList ChartDb::getKeyList ()
{
  QStringList list;
  datum nextkey;

  datum key = gdbm_firstkey(db);
  while (key.dptr)
  {
    nextkey = gdbm_nextkey(db, key);
    QString s = key.dptr;
    list.append(s);
    delete key.dptr;
    key = nextkey;
  }

  return list;
}

void ChartDb::getHistory (Compression c, QDateTime sd)
{
  compression = c;
  startDate = sd;

  QString s = getChartType();
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
  QDateTime tdate = startDate;

  QDateTime lastDate = getLastRecord();
  if (! lastDate.isValid())
    lastDate = QDateTime::currentDateTime();

  while (tdate <= lastDate)
  {
    Setting *r = getRecord(tdate.toString(DATE_FORMAT));

    if (r)
      recordList.append(r);

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

void ChartDb::getWeeklyHistory ()
{
  QDateTime tdate = startDate;

  QDateTime lastDate = getLastRecord();
  if (! lastDate.isValid())
    lastDate = QDateTime::currentDateTime();

  while (tdate <= lastDate)
  {
    Setting *tr = new Setting;
    tr->set("Date", "0", Setting::Date);
    tr->set("Open", "0", Setting::Float);
    tr->set("High", "-9999999999", Setting::Float);
    tr->set("Low", "9999999999", Setting::Float);
    tr->set("Close", "0", Setting::Float);
    tr->set("Volume", "0", Setting::Float);
    tr->set("OI", "0", Setting::Float);

    if (tdate.date().dayOfWeek() == 6)
      tdate = tdate.addDays(2);
    else
    {
      if (tdate.date().dayOfWeek() == 7)
        tdate = tdate.addDays(1);
    }

    while (tdate.date().dayOfWeek() != 6)
    {
      Setting *r = getRecord(tdate.toString(DATE_FORMAT));
      if (r)
      {
        tr->setData("Date", r->getData("Date"));

        if (tr->getFloat("Open") == 0)
          tr->setData("Open", r->getData("Open"));

        if (r->getFloat("High") > tr->getFloat("High"))
          tr->setData("High", r->getData("High"));

        if (r->getFloat("Low") < tr->getFloat("Low"))
          tr->setData("Low", r->getData("Low"));

        tr->setData("Close", r->getData("Close"));

        tr->setData("Volume", QString::number(tr->getFloat("Volume") + r->getFloat("Volume")));

        if (r->getFloat("OI") > tr->getFloat("OI"))
          tr->setData("OI", r->getData("OI"));

        delete r;
      }

      tdate = tdate.addDays(1);
    }

    if (tr->getFloat("Close") != 0)
      recordList.append(tr);
    else
      delete tr;
  }
}

void ChartDb::getMonthlyHistory ()
{
  QDateTime tdate = startDate;

  QDateTime lastDate = getLastRecord();
  if (! lastDate.isValid())
    lastDate = QDateTime::currentDateTime();

  while (tdate <= lastDate)
  {
    Setting *tr = new Setting;
    tr->set("Date", "0", Setting::Date);
    tr->set("Open", "0", Setting::Float);
    tr->set("High", "-9999999999", Setting::Float);
    tr->set("Low", "9999999999", Setting::Float);
    tr->set("Close", "0", Setting::Float);
    tr->set("Volume", "0", Setting::Float);
    tr->set("OI", "0", Setting::Float);

    int month = tdate.date().month();

    while (tdate.date().month() == month)
    {
      Setting *r = getRecord(tdate.toString(DATE_FORMAT));
      if (r)
      {
        tr->setData("Date", r->getData("Date"));

        if (tr->getFloat("Open") == 0)
          tr->setData("Open", r->getData("Open"));

        if (r->getFloat("High") > tr->getFloat("High"))
          tr->setData("High", r->getData("High"));

        if (r->getFloat("Low") < tr->getFloat("Low"))
          tr->setData("Low", r->getData("Low"));

        tr->setData("Close", r->getData("Close"));

        tr->setData("Volume", QString::number(tr->getFloat("Volume") + r->getFloat("Volume")));

        if (r->getFloat("OI") > tr->getFloat("OI"))
          tr->setData("OI", r->getData("OI"));

        delete r;
      }

      tdate = tdate.addDays(1);
    }

    if (tr->getFloat("Close") != 0)
      recordList.append(tr);
    else
      delete tr;
  }
}

QDateTime ChartDb::getLastRecord ()
{
  QDateTime dt;

  QString s = getData("LAST_DATE");
  if (s.length())
  {
    Setting *r = getRecord(s);
    QString s = r->getData("Date");
    s.insert(4, "-");
    s.insert(7, "-");
    s.insert(12, ":");
    s.insert(15, ":");
    dt = QDateTime::fromString(s, Qt::ISODate);
    delete r;
  }

  return dt;
}

QDateTime ChartDb::getFirstRecord ()
{
  QDateTime dt;

  QString s = getData("FIRST_DATE");
  if (s.length())
  {
    Setting *r = getRecord(s);
    QString s = r->getData("Date");
    s.insert(4, "-");
    s.insert(7, "-");
    s.insert(12, ":");
    s.insert(15, ":");
    dt = QDateTime::fromString(s, Qt::ISODate);
    delete r;
  }

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

  date.insert(4, "-");
  date.insert(7, "-");
  date.insert(12, ":");
  date.insert(15, ":");
  QDateTime dt = QDateTime::fromString(date, Qt::ISODate);

  QDateTime td = getLastRecord();
  if (td.isValid())
  {
    if (dt > td)
      setData("LAST_DATE", dt.toString(DATE_FORMAT));
  }
  else
  {
    setData("LAST_DATE", dt.toString(DATE_FORMAT));
    setData("FIRST_DATE", dt.toString(DATE_FORMAT));
    return;
  }

  td = getFirstRecord();
  if (td.isValid())
  {
    if (dt < td)
      setData("FIRST_DATE", dt.toString(DATE_FORMAT));
  }
  else
  {
    setData("LAST_DATE", dt.toString(DATE_FORMAT));
    setData("FIRST_DATE", dt.toString(DATE_FORMAT));
  }
}

Setting * ChartDb::getRecord (QString d)
{
  Setting *r = 0;

  QString s = getData(d);
  if (s.length())
  {
    QStringList l = QStringList::split(",", s, FALSE);
    r = new Setting;
    r->set("Date", d, Setting::Date);
    r->set("Open", l[0], Setting::Float);
    r->set("High", l[1], Setting::Float);
    r->set("Low", l[2], Setting::Float);
    r->set("Close", l[3], Setting::Float);
    r->set("Volume", l[4], Setting::Float);
    r->set("Open Interest", l[5], Setting::Float);
  }

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
  Setting *r = getRecord(d.toString(DATE_FORMAT));
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

void ChartDb::setSymbol (QString d)
{
  setData("SYMBOL", d);
}

QString ChartDb::getSymbol ()
{
  return getData("SYMBOL");
}

void ChartDb::setChartType (QString d)
{
  setData("CHART_TYPE", d);
}

QString ChartDb::getChartType ()
{
  return getData("CHART_TYPE");
}

void ChartDb::setSource (QString d)
{
  setData("SOURCE", d);
}

QString ChartDb::getSource ()
{
  return getData("SOURCE");
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



