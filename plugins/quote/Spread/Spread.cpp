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

#include "Spread.h"
#include "ChartDb.h"
#include "SpreadDialog.h"
#include <qdir.h>
#include <qstringlist.h>
#include <qmessagebox.h>
#include <qtimer.h>

Spread::Spread ()
{
  pluginName = "Spread";
  data.setAutoDelete(TRUE);
}

Spread::~Spread ()
{
}

void Spread::update ()
{
  QTimer::singleShot(250, this, SLOT(updateSpread()));
}

void Spread::updateSpread ()
{
  data.clear();

  QString s = dataPath;
  s.append("/Spread");
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
    Setting *tdetails = new Setting;
    tdetails->parse(details->getString());
    tdetails->remove("First Date");
    tdetails->remove("Last Date");

    delete db;
    dir.remove(s, TRUE);
    db = new ChartDb();
    if (db->openChart(s))
    {
      emit statusLogMessage(tr("could not open db"));
      delete db;
      delete tdetails;
      continue;
    }

    s = tr("Updating ");
    s.append(dir[loop]);
    emit statusLogMessage(s);

    details = db->getDetails();
    details->parse(tdetails->getString());
    delete tdetails;
    db->saveDetails();
    db->setFormat();

    loadData(details->getData(tr("First Symbol")), details->getData(tr("Method")));

    loadData(details->getData(tr("Second Symbol")), details->getData(tr("Method")));

    QDictIterator<Setting> it(data);
    for (; it.current(); ++it)
    {
      Setting *r = it.current();
      if (r->getInt("Count") == 2)
      {
        db->setRecord(r);
	setDataLogMessage(r);
      }
    }

    delete db;

    data.clear();
  }

  emit done();
  emit statusLogMessage(tr("Done"));
}

void Spread::loadData (QString symbol, QString method)
{
  ChartDb *tdb = new ChartDb();
  if (tdb->openChart(symbol))
  {
    emit statusLogMessage(tr("could not open db"));
    delete tdb;
    return;
  }

  Setting *details = tdb->getDetails();

  QDateTime dt = tdb->getDateTime(details->getData("First Date"));

  BarData *recordList = tdb->getHistory(ChartDb::Daily, dt, BarData::Bars);

  int loop;
  for (loop = 0; loop < (int) recordList->count(); loop++)
  {
//    Setting *tr = recordList->at(loop);
    Setting *r = data.find(recordList->getDate(loop).toString("yyyyMMdd000000"));
    if (! r)
    {
      r = new Setting;
      r->set("Date", recordList->getDate(loop).toString("yyyyMMdd000000"), Setting::Date);
      r->set("Close", QString::number(recordList->getClose(loop)), Setting::Float);
      r->set("Volume", QString::number(recordList->getVolume(loop)), Setting::Float);
      r->set("Open Interest", QString::number(recordList->getOI(loop)), Setting::Float);
      r->set("Open", r->getData("Close"), Setting::Float);
      r->set("High", r->getData("Close"), Setting::Float);
      r->set("Low", r->getData("Close"), Setting::Float);
      r->set("Count", "1", Setting::Integer);
      data.insert(r->getData("Date"), r);
    }
    else
    {
      if (! method.compare(QObject::tr("Subtract")))
      {
        r->setData("Close", QString::number(r->getFloat("Close") - (recordList->getClose(loop))));
        r->setData("Volume", QString::number(r->getFloat("Volume") - (recordList->getVolume(loop))));
        r->setData("Open Interest", QString::number(r->getFloat("Open Interest") - (recordList->getOI(loop))));
      }

      if (! method.compare(QObject::tr("Divide")))
      {
        r->setData("Close", QString::number(r->getFloat("Close") / (recordList->getClose(loop))));
        r->setData("Volume", QString::number(r->getFloat("Volume") / (recordList->getVolume(loop))));
        r->setData("Open Interest", QString::number(r->getFloat("Open Interest") / (recordList->getOI(loop))));
      }

      r->setData("Open", r->getData("Close"));
      r->setData("High", r->getData("Close"));
      r->setData("Low", r->getData("Close"));
      r->setData("Count", "2");
    }
  }

  delete recordList;
  delete tdb;
}

void Spread::prefDialog ()
{
  SpreadDialog *dialog = new SpreadDialog();
  dialog->setCaption(tr("Spread Prefs"));
  dialog->exec();
  delete dialog;
}

Plugin * create ()
{
  Spread *o = new Spread;
  return ((Plugin *) o);
}

