/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include "Index.h"
#include "ChartDb.h"
#include <qdir.h>
#include <qstringlist.h>
#include <qmessagebox.h>
#include <qtimer.h>

Index::Index ()
{
  pluginName = "Index";
  createFlag = TRUE;
  data.setAutoDelete(TRUE);

  about = "Updates a chart to form a composite index chart.\n";
}

Index::~Index ()
{
}

void Index::update ()
{
  QTimer::singleShot(250, this, SLOT(updateIndex()));
}

void Index::updateIndex ()
{
  data.clear();

  QString s = dataPath;
  s.append("/Index");
  QDir dir(s);

  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    s = dir.path();
    s.append("/");
    s.append(dir[loop]);

    ChartDb *db = new ChartDb();
    if (db->openChart(s))
    {
      qDebug("could not open db");
      delete db;
      continue;
    }

    Setting *details = db->getDetails();

    s = tr("Updating ");
    s.append(dir[loop]);
    emit message(s);

    int loop2;
    int count = 0;
    for (loop2 = 1; loop2 < 11; loop2++)
    {
      s = tr("Symbol ");
      if (loop2 < 10)
        s.append("0");
      s.append(QString::number(loop2));
      QString symbol = details->getData(s);

      s = tr("Weight ");
      if (loop2 < 10)
        s.append("0");
      s.append(QString::number(loop2));
      float weight = details->getFloat(s);
      if (weight == 0)
        weight = 1;

      if (! loadData(symbol, weight))
        count++;
    }

    QDictIterator<Setting> it(data);
    for (; it.current(); ++it)
    {
      Setting *r = it.current();
      if (r->getInt("Count") == count)
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

        db->setRecord(r2);

	delete r2;
      }
    }
    
    delete db;

    data.clear();
  }
  
  emit done();
}

int Index::loadData (QString symbol, float weight)
{
  QDir dir(symbol);
  if (! dir.exists(symbol, TRUE))
    return TRUE;

  ChartDb *db = new ChartDb();
  if (db->openChart(symbol))
  {
    qDebug("could not open db");
    delete db;
    return TRUE;
  }

  Setting *details = db->getDetails();

  QDateTime dt = db->getDateTime(details->getData("First Date"));

  BarData *recordList = db->getHistory(ChartDb::Daily, dt, BarData::Bars);

  int loop;
  for (loop = 0; loop < (int) recordList->count(); loop++)
  {
    Setting *r = data.find(recordList->getDate(loop).toString("yyyyMMdd000000"));
    if (! r)
    {
      r = new Setting;

      r->set("Date", recordList->getDate(loop).toString("yyyyMMdd000000"), Setting::Date);
      r->set("Close", QString::number(recordList->getClose(loop) * weight), Setting::Float);
      r->set("Volume", QString::number(recordList->getVolume(loop) * weight), Setting::Float);
      r->set("Open Interest", QString::number(recordList->getOI(loop) * weight), Setting::Float);
      r->set("Open", QString::number(recordList->getOpen(loop) * weight), Setting::Float);
      r->set("High", QString::number(recordList->getHigh(loop) * weight), Setting::Float);
      r->set("Low", QString::number(recordList->getLow(loop) * weight), Setting::Float);
      r->set("Count", "1", Setting::Integer);
      data.insert(r->getData("Date"), r);
    }
    else
    {
      r->setData("Close", QString::number(r->getFloat("Close") + (recordList->getClose(loop) * weight)));
      r->setData("Volume", QString::number(r->getFloat("Volume") + (recordList->getVolume(loop) * weight)));
      r->setData("Open Interest", QString::number(r->getFloat("Open Interest") + (recordList->getOI(loop) * weight)));
      r->setData("Open", QString::number(r->getFloat("Open") + (recordList->getOpen(loop) * weight)));
      r->setData("High", QString::number(r->getFloat("High") + (recordList->getHigh(loop) * weight)));
      r->setData("Low", QString::number(r->getFloat("Low") + (recordList->getLow(loop) * weight)));
      r->setData("Count", QString::number(r->getInt("Count") + 1));
    }
  }

  delete recordList;
  delete db;

  return FALSE;
}

Setting * Index::getCreateDetails ()
{
  Setting *set = new Setting;
  set->set(tr("Symbol"), "New Index", Setting::Text);
  set->set(tr("Symbol 01"), " ", Setting::Symbol);
  set->set(tr("Weight 01"), "1", Setting::Float);
  set->set(tr("Symbol 02"), " ", Setting::Symbol);
  set->set(tr("Weight 02"), "1", Setting::Float);
  set->set(tr("Symbol 03"), " ", Setting::Symbol);
  set->set(tr("Weight 03"), "1", Setting::Float);
  set->set(tr("Symbol 04"), " ", Setting::Symbol);
  set->set(tr("Weight 04"), "1", Setting::Float);
  set->set(tr("Symbol 05"), " ", Setting::Symbol);
  set->set(tr("Weight 05"), "1", Setting::Float);
  set->set(tr("Symbol 06"), " ", Setting::Symbol);
  set->set(tr("Weight 06"), "1", Setting::Float);
  set->set(tr("Symbol 07"), " ", Setting::Symbol);
  set->set(tr("Weight 07"), "1", Setting::Float);
  set->set(tr("Symbol 08"), " ", Setting::Symbol);
  set->set(tr("Weight 08"), "1", Setting::Float);
  set->set(tr("Symbol 09"), " ", Setting::Symbol);
  set->set(tr("Weight 09"), "1", Setting::Float);
  set->set(tr("Symbol 10"), " ", Setting::Symbol);
  set->set(tr("Weight 10"), "1", Setting::Float);
  return set;
}

void Index::createChart (Setting *set)
{
  QString symbol = set->getData(tr("Symbol"));
  if (! symbol.length())
  {
    QMessageBox::information(0, tr("Error"), tr("Invalid symbol"), 0, 0, QMessageBox::Ok);
    return;
  }

  QString path = createDirectory("Index");
  if (! path.length())
  {
    qDebug("Index plugin: Unable to create directory");
    return;
  }

  path.append("/");
  path.append(symbol);
  QDir dir(path);
  if (dir.exists(path, TRUE))
  {
    QMessageBox::information(0, tr("Error"), tr("Duplicate chart"), 0, 0, QMessageBox::Ok);
    return;
  }

  ChartDb *db = new ChartDb();
  db->openChart(path);

  Setting *details = db->getDetails();
  details->set("Format", "Open|High|Low|Close|Volume|Open Interest", Setting::None);
  details->set("Chart Type", tr("Index"), Setting::None);
  details->set("Symbol", symbol, Setting::None);
  details->set("Title", symbol, Setting::Text);
  details->set(tr("Symbol 01"), set->getData(tr("Symbol 01")), set->getType(tr("Symbol 01")));
  details->set(tr("Weight 01"), set->getData(tr("Weight 01")), set->getType(tr("Weight 01")));
  details->set(tr("Symbol 02"), set->getData(tr("Symbol 02")), set->getType(tr("Symbol 02")));
  details->set(tr("Weight 02"), set->getData(tr("Weight 02")), set->getType(tr("Weight 02")));
  details->set(tr("Symbol 03"), set->getData(tr("Symbol 03")), set->getType(tr("Symbol 03")));
  details->set(tr("Weight 03"), set->getData(tr("Weight 03")), set->getType(tr("Weight 03")));
  details->set(tr("Symbol 04"), set->getData(tr("Symbol 04")), set->getType(tr("Symbol 04")));
  details->set(tr("Weight 04"), set->getData(tr("Weight 04")), set->getType(tr("Weight 04")));
  details->set(tr("Symbol 05"), set->getData(tr("Symbol 05")), set->getType(tr("Symbol 05")));
  details->set(tr("Weight 05"), set->getData(tr("Weight 05")), set->getType(tr("Weight 05")));
  details->set(tr("Symbol 06"), set->getData(tr("Symbol 06")), set->getType(tr("Symbol 06")));
  details->set(tr("Weight 06"), set->getData(tr("Weight 06")), set->getType(tr("Weight 06")));
  details->set(tr("Symbol 07"), set->getData(tr("Symbol 07")), set->getType(tr("Symbol 07")));
  details->set(tr("Weight 07"), set->getData(tr("Weight 07")), set->getType(tr("Weight 07")));
  details->set(tr("Symbol 08"), set->getData(tr("Symbol 08")), set->getType(tr("Symbol 08")));
  details->set(tr("Weight 08"), set->getData(tr("Weight 08")), set->getType(tr("Weight 08")));
  details->set(tr("Symbol 09"), set->getData(tr("Symbol 09")), set->getType(tr("Symbol 09")));
  details->set(tr("Weight 09"), set->getData(tr("Weight 09")), set->getType(tr("Weight 09")));
  details->set(tr("Symbol 10"), set->getData(tr("Symbol 10")), set->getType(tr("Symbol 10")));
  details->set(tr("Weight 10"), set->getData(tr("Weight 10")), set->getType(tr("Weight 10")));

  db->saveDetails();

  delete db;
}

Plugin * create ()
{
  Index *o = new Index;
  return ((Plugin *) o);
}

