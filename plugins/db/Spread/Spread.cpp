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

#include "Spread.h"
#include "PrefDialog.h"
#include "ChartDb.h"
#include "Config.h"
#include "Bar.h"
#include <qdir.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qobject.h>

Spread::Spread ()
{
  data.setAutoDelete(TRUE);
}

Spread::~Spread ()
{
}

BarData * Spread::getHistory ()
{
  updateSpread();
  return DbPlugin::getHistory();
}

Bar * Spread::getBar (QString k, QString d)
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

void Spread::dbPrefDialog ()
{
  Config config;
  QStringList l;
  l.append(QObject::tr("Subtract"));
  l.append(QObject::tr("Divide"));
  
  QString s = config.getData(Config::DataPath) + "/Futures";
  
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("Spread Prefs"));
  dialog->createPage (QObject::tr("Details"));
  dialog->addSymbolItem(QObject::tr("First Symbol"), QObject::tr("Details"), s, getData("First Symbol"));
  dialog->addSymbolItem(QObject::tr("Second Symbol"), QObject::tr("Details"), s, getData("Second Symbol"));
  dialog->addComboItem(QObject::tr("Method"), QObject::tr("Details"), l, getData("Method"));
  dialog->addCheckItem(QObject::tr("Rebuild"), QObject::tr("Details"), getData("Rebuild").toInt());
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    setData("First Symbol", dialog->getSymbol(QObject::tr("First Symbol")));
    setData("Second Symbol", dialog->getSymbol(QObject::tr("Second Symbol")));
    setData("Method", dialog->getCombo(QObject::tr("Method")));
    setData("Rebuild", QString::number(dialog->getCheck(QObject::tr("Rebuild"))));
    updateSpread();
  }
  
  delete dialog;
}

void Spread::setBar (Bar *bar)
{
  QStringList l;
  l.append(QString::number(bar->getOpen()));
  l.append(QString::number(bar->getHigh()));
  l.append(QString::number(bar->getLow()));
  l.append(QString::number(bar->getClose()));
  setData(bar->getDate().getDateTimeString(FALSE), l.join(","));
}

void Spread::updateSpread ()
{
  data.clear();
  
  QString fs = getData("First Symbol");
  QString ss = getData("Second Symbol");
  QString meth = getData("Method");

  loadData(fs, meth);

  loadData(ss, meth);

  QDictIterator<Bar> it(data);
  for (; it.current(); ++it)
  {
    Bar *r = it.current();
    if (r->getData("Count") == 2)
    {
      Bar *bar = new Bar;
      bar->setDate(r->getDate());
      bar->setOpen(r->getClose());
      bar->setHigh(r->getClose());
      bar->setLow(r->getClose());
      bar->setClose(r->getClose());
      setBar(bar);
      delete bar;
    }
  }

  data.clear();
}

void Spread::loadData (QString symbol, QString method)
{
  ChartDb *db = new ChartDb;
  if (db->openChart(symbol))
  {
    qDebug("Spread::loadData: can't open db");
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
      r->setClose(recordList->getClose(loop));
      r->setData("Count", 1);
      data.insert(r->getDate().getDateTimeString(FALSE), r);
    }
    else
    {
      if (! method.compare(QObject::tr("Subtract")))
        r->setClose(r->getClose() - recordList->getClose(loop));

      if (! method.compare(QObject::tr("Divide")))
        r->setClose(r->getClose() / recordList->getClose(loop));
      
      r->setData("Count", 2);
    }
  }

  delete db;
}

QString Spread::createNew ()
{
  bool ok = FALSE;
  QString spread = QInputDialog::getText(QObject::tr("New Spread"),
                                         QObject::tr("Enter symbol name for the new Spread"),
					 QLineEdit::Normal,
					 QString::null,
					 &ok,
					 0);
  if (! spread.length() || ok == FALSE)
    return QString();

  QDir dir;
  Config config;
  QString s = config.getData(Config::DataPath) + "/Spread";
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
    {
      QMessageBox::information(0,
                               QObject::tr("Qtstalker: Error"),
			       QObject::tr("Could not create ~/Qtstalker/data/Spread directory."));
      return QString();
    }
  }
  
  s.append("/" + spread);
  if (dir.exists(s))
  {
    QMessageBox::information(0,
                             QObject::tr("Qtstalker: Error"),
			     QObject::tr("This Spread already exists."));
    return QString();
  }

  return s;  
}

void Spread::saveDbDefaults (Setting *set)
{
  setData("Symbol", set->getData("Symbol"));
  setData("Type", "Spread");
  setData("Title", set->getData("Title"));
  setData("BarType", set->getData("BarType"));
  setData("Plugin", "Spread");
}

DbPlugin * createDbPlugin ()
{
  Spread *o = new Spread;
  return ((DbPlugin *) o);
}

