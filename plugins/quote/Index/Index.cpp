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
#include "IndexDialog.h"
#include <qdir.h>
#include <qstringlist.h>
#include <qmessagebox.h>
#include <qtimer.h>
#include <qsettings.h>

Index::Index ()
{
  pluginName = "Index";
  data.setAutoDelete(TRUE);
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
      emit statusLogMessage(tr("could not open db"));
      delete db;
      continue;
    }

    Setting *details = db->getDetails();

    s = tr("Updating ");
    s.append(dir[loop]);
    emit statusLogMessage(s);
    
    QStringList l = QStringList::split(":", details->getData("Index"), FALSE);

    int loop2;
    int count = 0;
    for (loop2 = 0; loop2 < (int) l.count(); loop2 = loop2 + 2)
    {
      QString symbol = l[loop2];
      float weight = l[loop2 + 1].toFloat();
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
        setDataLogMessage(r2);

	delete r2;
      }
    }
    
    delete db;

    data.clear();
  }
  
  emit done();
  emit statusLogMessage(tr("Done"));
}

int Index::loadData (QString symbol, float weight)
{
  QDir dir(symbol);
  if (! dir.exists(symbol, TRUE))
    return TRUE;

  ChartDb *db = new ChartDb();
  if (db->openChart(symbol))
  {
    emit statusLogMessage(tr("could not open db"));
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

void Index::prefDialog ()
{
  IndexDialog *dialog = new IndexDialog();
  dialog->setCaption(tr("Index Prefs"));
  dialog->exec();
  delete dialog;
}

Plugin * create ()
{
  Index *o = new Index;
  return ((Plugin *) o);
}

