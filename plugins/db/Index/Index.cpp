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
#include "DbPlugin.h"
#include "Bar.h"
#include "BarData.h"
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

void Index::getHistory (BarData *bd)
{
  updateIndex();
  DbPlugin::getHistory(bd);
}

void Index::dbPrefDialog ()
{
  IndexDialog *dialog = new IndexDialog(helpFile);
  
  QString s = "Index";
  QString s2;
  getData(s, s2);
  dialog->setList(s2);
  
  s = "Rebuild";
  getData(s, s2);
  dialog->setRebuild(s2.toInt());
  
  s = "Symbol";
  getData(s, s2);
  dialog->setName(s2);
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    QString s = dialog->getList();
    QString s2;
    if (s.length())
    {
      s2 = "Index";
      setData(s2, s);
    }
    
    s = "Rebuild";
    s2 = QString::number(dialog->getRebuild());
    setData(s, s2);
  }
  
  delete dialog;
}

void Index::updateIndex ()
{
  data.clear();
  fdate = 99999999999999.0;
  
  QString s = "Index";
  QString s2;
  getData(s, s2);  
  QStringList l = QStringList::split(":", s2, FALSE);
  if (! l.count())
    return;
    
  int loop;
  int count = 0;
  for (loop = 0; loop < (int) l.count(); loop = loop + 2)
  {
    QString symbol = l[loop];
    float weight = l[loop + 1].toFloat();
    if (weight == 0)
      weight = 1;

    loadData(symbol, weight);
    count++;
  }

  Bar *r = data.find(QString::number(fdate, 'f', 0));
  if (r)
  {
    Bar &bar = *r;
    setBar(bar);

    s = "Count";
    if (r->getData(s) != count)
    {
      s = QString::number(fdate, 'f', 0);
      deleteData(s);
    }
  }
  
  QDictIterator<Bar> it(data);
  for (; it.current(); ++it)
  {
    r = it.current();
    s = "Count";
    if (r->getData(s) == count)
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
	
      Bar &bar = *r;
      setBar(bar);
    }
  }

  data.clear();
}

void Index::loadData (QString &symbol, float weight)
{
  Config config;
  QString plugin = config.parseDbPlugin(symbol);
  DbPlugin *db = config.getDbPlugin(plugin);
  if (! db)
  {
    config.closePlugin(plugin);
    return;
  }

  if (db->openChart(symbol))
  {
    qDebug("Index::loadData: can't open db");
    config.closePlugin(plugin);
    return;
  }
  
  db->setBarCompression(BarData::DailyBar);
  db->setBarRange(99999999);
  
  QString s = "Rebuild";
  QString s2;
  getData(s, s2);
  bool rebuild = s2.toInt();
  if (! rebuild)
  {
    Bar *bar = getLastBar();
    if (bar)
    {
      QDate d = QDate::currentDate();
      db->setBarRange(bar->getDate().getDate().daysTo(d));
      delete bar;
    }
  }

  BarData *recordList = new BarData;  
  db->getHistory(recordList);
  
  int loop;
  for (loop = 0; loop < (int) recordList->count(); loop++)
  {
    recordList->getDate(loop).getDateTimeString(FALSE, s);
    Bar *r = data.find(s);
    if (! r)
    {
      r = new Bar;
      BarDate dt = recordList->getDate(loop);
      r->setDate(dt);
      r->setOpen(recordList->getOpen(loop) * weight);
      r->setHigh(recordList->getHigh(loop) * weight);
      r->setLow(recordList->getLow(loop) * weight);
      r->setClose(recordList->getClose(loop) * weight);
      s = "Count";
      r->setData(s, 1);
      r->getDate().getDateTimeString(FALSE, s);
      data.insert(s, r);
      
      if (r->getDate().getDateValue() < fdate)
        fdate = r->getDate().getDateValue();
    }
    else
    {
      r->setOpen(r->getOpen() + (recordList->getOpen(loop) * weight));
      r->setHigh(r->getHigh() + (recordList->getHigh(loop) * weight));
      r->setLow(r->getLow() + (recordList->getLow(loop) * weight));
      r->setClose(r->getClose() + (recordList->getClose(loop) * weight));
      s = "Count";
      r->setData(s, r->getData(s) + 1);
    }
  }

  delete recordList;
  config.closePlugin(plugin);
}

void Index::createNew ()
{
  bool ok = FALSE;
  QString index = QInputDialog::getText(QObject::tr("New Index"),
                                        QObject::tr("Enter symbol name for the new Index"),
					QLineEdit::Normal,
					QString::null,
					&ok,
					0);
  if (! index.length() || ok == FALSE)
    return;

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
      return;
    }
  }
  
  s.append("/" + index);
  if (dir.exists(s))
  {
    QMessageBox::information(0,
                             QObject::tr("Qtstalker: Error"),
			     QObject::tr("This Index already exists."));
    return;
  }
  
  openChart(s);

  setHeaderField(Symbol, index);
  
  s = "Index";
  setHeaderField(Type, s);  
  setHeaderField(Plugin, s);  
  setHeaderField(Title, index);
  s = QString::number(BarData::Daily);
  setHeaderField(BarType, s);  
  
  dbPrefDialog();
}

Bar * Index::getBar (QString &k, QString &d)
{
  Bar *bar = new Bar;
  QStringList l = QStringList::split(",", d, FALSE);
  bar->setDate(k);
  bar->setOpen(l[0].toDouble());
  bar->setHigh(l[1].toDouble());
  bar->setLow(l[2].toDouble());
  bar->setClose(l[3].toDouble());
  return bar;
}

void Index::setBar (Bar &bar)
{
  QString k;
  getHeaderField(BarType, k);
  if (k.toInt() != bar.getTickFlag())
    return;

  bar.getDate().getDateTimeString(FALSE, k);
  
  QString d = QString::number(bar.getOpen()) + "," + QString::number(bar.getHigh()) + "," +
              QString::number(bar.getLow()) + "," + QString::number(bar.getClose());
  
  setData(k, d);
}

//********************************************************************
//********************************************************************
//********************************************************************

DbPlugin * createDbPlugin ()
{
  Index *o = new Index;
  return ((DbPlugin *) o);
}

