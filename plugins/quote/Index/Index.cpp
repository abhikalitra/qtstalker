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

    s = tr("Updating ");
    s.append(dir[loop]);
    emit statusLogMessage(s);
    
    QStringList l = QStringList::split(":", db->getData("Index"), FALSE);

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
        Bar *r2 = new Bar;
        if (r2->setDate(r->getData("Date")))
        {
          delete r2;
          emit statusLogMessage("Bad date " + r->getData("Date"));
          continue;
        }

      	r2->setDate(r->getData("Date"));
	r2->setClose(r->getFloat("Close") / count);
	r2->setVolume(r->getFloat("Volume") / count);
	r2->setOI(r->getInt("OI") / count);
	r2->setOpen(r->getFloat("Open") / count);
	r2->setHigh(r->getFloat("High") / count);
	r2->setLow(r->getFloat("Low") / count);

	if (r2->getOpen() > r2->getHigh())
	  r2->setHigh(r2->getOpen());
	if (r2->getOpen() < r2->getLow())
	  r2->setLow(r2->getOpen());

	if (r2->getClose() > r2->getHigh())
	  r2->setHigh(r2->getClose());
	if (r2->getClose() < r2->getLow())
	  r2->setLow(r2->getClose());

        db->setBar(r2);

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

  db->setBarRange(99999999);
  db->setBarCompression(ChartDb::Daily);
  BarData *recordList = db->getHistory();

  int loop;
  for (loop = 0; loop < (int) recordList->count(); loop++)
  {
    Setting *r = data.find(recordList->getDate(loop).getDateTimeString(FALSE));
    if (! r)
    {
      r = new Setting;

      r->setData("Date", recordList->getDate(loop).getDateTimeString(FALSE));
      r->setData("Close", QString::number(recordList->getClose(loop) * weight));
      r->setData("Volume", QString::number(recordList->getVolume(loop) * weight));
      r->setData("OI", QString::number(recordList->getOI(loop) * weight));
      r->setData("Open", QString::number(recordList->getOpen(loop) * weight));
      r->setData("High", QString::number(recordList->getHigh(loop) * weight));
      r->setData("Low", QString::number(recordList->getLow(loop) * weight));
      r->setData("Count", "1");
      data.insert(r->getData("Date"), r);
    }
    else
    {
      r->setData("Close", QString::number(r->getFloat("Close") + (recordList->getClose(loop) * weight)));
      r->setData("Volume", QString::number(r->getFloat("Volume") + (recordList->getVolume(loop) * weight)));
      r->setData("OI", QString::number(r->getFloat("OI") + (recordList->getOI(loop) * weight)));
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

