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

    QString symbol = db->getDetail(ChartDb::Symbol);
    QString type = db->getDetail(ChartDb::Type);
    QString title = db->getDetail(ChartDb::Title);
    QString fs = db->getData("First Symbol");
    QString ss = db->getData("Second Symbol");
    QString meth = db->getData("Method");

    delete db;
    dir.remove(s, TRUE);
    db = new ChartDb();
    if (db->openChart(s))
    {
      emit statusLogMessage(tr("could not open db"));
      delete db;
      continue;
    }

    s = tr("Updating ");
    s.append(dir[loop]);
    emit statusLogMessage(s);

    db->setDetail(ChartDb::Symbol, symbol);
    db->setDetail(ChartDb::Type, type);
    db->setDetail(ChartDb::Title, title);
    db->setData("First Symbol", fs);
    db->setData("Second Symbol", ss);
    db->setData("Method", meth);
    db->setDetail(ChartDb::BarType, "Daily");
    
    loadData(fs, meth);

    loadData(ss, meth);

    QDictIterator<Setting> it(data);
    for (; it.current(); ++it)
    {
      Setting *r = it.current();
      if (r->getInt("Count") == 2)
      {
        Bar *bar = new Bar;
        if (bar->setDate(r->getData("Date")))
        {
          delete bar;
          emit statusLogMessage("Bad date " + r->getData("Date"));
          continue;
        }
        bar->setOpen(r->getFloat("Open"));
        bar->setHigh(r->getFloat("High"));
        bar->setLow(r->getFloat("Low"));
        bar->setClose(r->getFloat("Close"));
        bar->setVolume(r->getFloat("Volume"));
        bar->setOI(r->getInt("OI"));
      
        db->setBar(bar);
	delete bar;
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

  tdb->setBarCompression(ChartDb::Daily);
  tdb->setBarRange(99999999);
  BarData *recordList = tdb->getHistory();
  delete tdb;

  int loop;
  for (loop = 0; loop < (int) recordList->count(); loop++)
  {
    Setting *r = data.find(recordList->getDate(loop).getDateTimeString(FALSE));
    if (! r)
    {
      r = new Setting;
      r->setData("Date", recordList->getDate(loop).getDateTimeString(FALSE));
      r->setData("Close", QString::number(recordList->getClose(loop)));
      r->setData("Volume", QString::number(recordList->getVolume(loop)));
      r->setData("OI", QString::number(recordList->getOI(loop)));
      r->setData("Open", r->getData("Close"));
      r->setData("High", r->getData("Close"));
      r->setData("Low", r->getData("Close"));
      r->setData("Count", "1");
      data.insert(r->getData("Date"), r);
    }
    else
    {
      if (! method.compare(QObject::tr("Subtract")))
      {
        r->setData("Close", QString::number(r->getFloat("Close") - (recordList->getClose(loop))));
        r->setData("Volume", QString::number(r->getFloat("Volume") - (recordList->getVolume(loop))));
        r->setData("OI", QString::number(r->getFloat("OI") - (recordList->getOI(loop))));
      }

      if (! method.compare(QObject::tr("Divide")))
      {
        r->setData("Close", QString::number(r->getFloat("Close") / (recordList->getClose(loop))));
        r->setData("Volume", QString::number(r->getFloat("Volume") / (recordList->getVolume(loop))));
        r->setData("OI", QString::number(r->getFloat("OI") / (recordList->getOI(loop))));
      }

      r->setData("Open", r->getData("Close"));
      r->setData("High", r->getData("Close"));
      r->setData("Low", r->getData("Close"));
      r->setData("Count", "2");
    }
  }

  delete recordList;
}

void Spread::prefDialog (QWidget *)
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

