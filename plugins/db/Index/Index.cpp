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

#include "Index.h"
#include "IndexDialog.h"
#include "Config.h"
#include "ChartDb.h"
#include "Bar.h"
#include <qdir.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qobject.h>

Index::Index ()
{
  data.setAutoDelete(TRUE);
  helpFile = "indexes.html";
}

Index::~Index ()
{
}

BarData * Index::getHistory ()
{
  updateIndex();
  return DbPlugin::getHistory();
}

Bar * Index::getBar (QString k, QString d)
{
  QStringList l = QStringList::split(",", d, FALSE);
  Bar *bar = new Bar;
  bar->setDate(k);
  bar->setData("Open", l[0].toDouble());
  bar->setData("High", l[1].toDouble());
  bar->setData("Low", l[2].toDouble());
  bar->setData("Close", l[3].toDouble());
  return bar;
}

void Index::dbPrefDialog ()
{
  IndexDialog *dialog = new IndexDialog(helpFile);
  dialog->setList(getData("Index"));
  dialog->setRebuild(getData("Rebuild").toInt());
  dialog->setName(getData("Symbol"));
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    QString s = dialog->getList();
    if (s.length())
      setData("Index", s);
    setData("Rebuild", QString::number(dialog->getRebuild()));
    updateIndex();
  }
  
  delete dialog;
}

void Index::setBar (Bar *bar)
{
  QStringList l;
  l.append(QString::number(bar->getOpen()));
  l.append(QString::number(bar->getHigh()));
  l.append(QString::number(bar->getLow()));
  l.append(QString::number(bar->getClose()));
  setData(bar->getDate().getDateTimeString(FALSE), l.join(","));
}

void Index::updateIndex ()
{
  data.clear();
  
  QStringList l = QStringList::split(":", getData("Index"), FALSE);
  if (! l.count())
    return;
    
  int loop;
  for (loop = 0; loop < (int) l.count(); loop = loop + 2)
  {
    QString symbol = l[loop];
    float weight = l[loop + 1].toFloat();
    if (weight == 0)
      weight = 1;

    loadData(symbol, weight);
  }

  int count = l.count() / 2;
  QDictIterator<Bar> it(data);
  for (; it.current(); ++it)
  {
    Bar *r = it.current();
    if (r->getData("Count") == count)
    {
      r->setOpen(r->getOpen() / count);
      r->setHigh(r->getHigh() / count);
      r->setLow(r->getLow() / count);
      r->setClose(r->getClose() / count);
      
      if (r->getOpen() > r->getHigh())
	r->setHigh(r->getOpen());
      if (r->getOpen() < r->getLow())
	r->setLow(r->getOpen());

      if (r->getClose() > r->getHigh())
	r->setHigh(r->getClose());
      if (r->getClose() < r->getLow())
	r->setLow(r->getClose());
	
      setBar(r);
    }
  }

  data.clear();
}

void Index::loadData (QString symbol, float weight)
{
  ChartDb *db = new ChartDb;
  if (db->openChart(symbol))
  {
    qDebug("Index::loadData: can't open db");
    delete db;
    return;
  }
  
  db->setBarCompression(BarData::DailyBar);
  
  QString rebuild = getData("Rebuild");
  if (rebuild.toInt())
    db->setBarRange(99999999);
  else
  {
    Bar *bar = getLastBar();
    if (bar)
    {
      QDate d = QDate::currentDate();
      db->setBarRange(bar->getDate().getDate().daysTo(d));
      delete bar;
    }
    else
      db->setBarRange(99999999);
  }

  BarData *recordList = db->getHistory();

  int loop;
  for (loop = 0; loop < (int) recordList->count(); loop++)
  {
    Bar *r = data.find(recordList->getDate(loop).getDateTimeString(FALSE));
    if (! r)
    {
      r = new Bar;
      r->setDate(recordList->getDate(loop));
      r->setOpen(recordList->getOpen(loop) * weight);
      r->setHigh(recordList->getHigh(loop) * weight);
      r->setLow(recordList->getLow(loop) * weight);
      r->setClose(recordList->getClose(loop) * weight);
      r->setData("Count", 1);
      data.insert(r->getDate().getDateTimeString(FALSE), r);
    }
    else
    {
      r->setOpen(r->getOpen() + (recordList->getOpen(loop) * weight));
      r->setHigh(r->getHigh() + (recordList->getHigh(loop) * weight));
      r->setLow(r->getLow() + (recordList->getLow(loop) * weight));
      r->setClose(r->getClose() + (recordList->getClose(loop) * weight));
      r->setData("Count", r->getData("Count") + 1);
    }
  }

  delete db;
}

QString Index::createNew ()
{
  bool ok = FALSE;
  QString index = QInputDialog::getText(QObject::tr("New Index"),
                                        QObject::tr("Enter symbol name for the new Index"),
					QLineEdit::Normal,
					QString::null,
					&ok,
					0);
  if (! index.length() || ok == FALSE)
    return QString();

  QDir dir;
  Config config;
  QString s = config.getData(Config::DataPath) + "/Index";
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
    {
      QMessageBox::information(0,
                               QObject::tr("Qtstalker: Error"),
			       QObject::tr("Could not create ~/Qtstalker/data/Index directory."));
      return QString();
    }
  }
  
  s.append("/" + index);
  if (dir.exists(s))
  {
    QMessageBox::information(0,
                             QObject::tr("Qtstalker: Error"),
			     QObject::tr("This Index already exists."));
    return QString();
  }
  
  return s;
}

void Index::saveDbDefaults (Setting *set)
{
  setData("Symbol", set->getData("Symbol"));
  setData("Type", "Index");
  setData("Title", set->getData("Title"));
  setData("BarType", set->getData("BarType"));
  setData("Plugin", "Index");
}

DbPlugin * createDbPlugin ()
{
  Index *o = new Index;
  return ((DbPlugin *) o);
}
