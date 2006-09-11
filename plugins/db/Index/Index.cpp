/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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

void Index::getHistory (BarData *bd, QDateTime &dt)
{
  updateIndex();
  DbPlugin::getHistory(bd, dt);
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
  
  getHeaderField(DbPlugin::Symbol, s);
  dialog->setSymbol(s);
  
  getHeaderField(DbPlugin::Title, s);
  dialog->setName(s);
  
  getHeaderField(DbPlugin::Type, s);
  dialog->setType(s);
  
  s.truncate(0);
  Bar bar;
  getFirstBar(bar);
  if (! bar.getEmptyFlag())
    bar.getDateTimeString(TRUE, s);
  dialog->setFirstDate(s);
  
  s.truncate(0);
  Bar bar2;
  getLastBar(bar2);
  if (! bar2.getEmptyFlag())
    bar2.getDateTimeString(TRUE, s);
  dialog->setLastDate(s);
  
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
    
    s = dialog->getName();
    setHeaderField(DbPlugin::Title, s);
  }
  
  delete dialog;
}

void Index::updateIndex ()
{
  data.clear();
  fdate = QDateTime::currentDateTime();
  
  QString s = "Index";
  QString s2;
  getData(s, s2);  
  QStringList l = QStringList::split(":", s2, FALSE);
  if (! l.count())
    return;
    
  emit signalStatusMessage(QString(tr("Updating index...")));
    
  int loop;
  int count = 0;
  for (loop = 0; loop < (int) l.count(); loop = loop + 2)
  {
    emit signalProgMessage(loop, l.count());
  
    QString symbol = l[loop];
    float weight = l[loop + 1].toFloat();
    if (weight == 0)
      weight = 1;

    loadData(symbol, weight);
    count++;
  }

  Bar *r = data.find(fdate.toString("yyyyMMddhhmmss"));
  if (r)
  {
    Bar &bar = *r;
    setBar(bar);

    if (r->getOI() != count)
    {
      s = fdate.toString("yyyyMMddhhmmss");
      deleteData(s);
    }
  }
  
  QDictIterator<Bar> it(data);
  for (; it.current(); ++it)
  {
    r = it.current();
    if (r->getOI() == count)
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
  emit signalProgMessage(-1, -1);
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
  
  db->setBarLength(BarData::DailyBar);
  db->setBarRange(99999999);
  
  QString s = "Rebuild";
  QString s2;
  getData(s, s2);
  bool rebuild = s2.toInt();
  if (! rebuild)
  {
    Bar bar;
    getLastBar(bar);
    if (! bar.getEmptyFlag())
    {
      QDate d = QDate::currentDate();
      QDateTime dt;
      bar.getDate(dt);
      db->setBarRange(dt.date().daysTo(d));
    }
  }

  BarData *recordList = new BarData;  
  QDateTime dt = QDateTime::currentDateTime();
  db->getHistory(recordList, dt);
  
  int loop;
  for (loop = 0; loop < (int) recordList->count(); loop++)
  {
    recordList->getDate(loop, dt);
    s = dt.toString("yyyyMMddhhmmss");
    Bar *r = data.find(s);
    if (! r)
    {
      r = new Bar;
      recordList->getDate(loop, dt);
      r->setDate(dt);
      r->setOpen(recordList->getOpen(loop) * weight);
      r->setHigh(recordList->getHigh(loop) * weight);
      r->setLow(recordList->getLow(loop) * weight);
      r->setClose(recordList->getClose(loop) * weight);
      r->setOI(1);
      r->getDateTimeString(FALSE, s);
      data.insert(s, r);
      
      if (dt < fdate)
        fdate = dt;
    }
    else
    {
      r->setOpen(r->getOpen() + (recordList->getOpen(loop) * weight));
      r->setHigh(r->getHigh() + (recordList->getHigh(loop) * weight));
      r->setLow(r->getLow() + (recordList->getLow(loop) * weight));
      r->setClose(r->getClose() + (recordList->getClose(loop) * weight));
      r->setOI((int) r->getOI() + 1);
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
			       QObject::tr("Could not create ~/.qtstalker/data/Index directory."));
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

void Index::getBar (QString &k, QString &d, Bar &bar)
{
  QStringList l = QStringList::split(",", d, FALSE);
  bar.setDate(k);
  bar.setOpen(l[0].toDouble());
  bar.setHigh(l[1].toDouble());
  bar.setLow(l[2].toDouble());
  bar.setClose(l[3].toDouble());
}

void Index::setBar (Bar &bar)
{
  QString k;
  getHeaderField(BarType, k);
  if (k.toInt() != bar.getTickFlag())
    return;

  bar.getDateTimeString(FALSE, k);
  
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

