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
#include "DbPlugin.h"
#include "Config.h"
#include "Bar.h"
#include "BarData.h"
#include <qdir.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qobject.h>

Spread::Spread ()
{
  data.setAutoDelete(TRUE);
  helpFile = "spread.html";
}

Spread::~Spread ()
{
}

BarData * Spread::getHistory ()
{
  updateSpread();
  return DbPlugin::getHistory();
}

void Spread::dbPrefDialog ()
{
  Config config;
  QStringList l;
  l.append(QObject::tr("Subtract"));
  l.append(QObject::tr("Divide"));
  
  QString s = config.getData(Config::DataPath);
  
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("Spread Prefs"));
  dialog->createPage (QObject::tr("Details"));
  dialog->setHelpFile (helpFile);
  dialog->addSymbolItem(QObject::tr("First Symbol"), QObject::tr("Details"), s, getData("First Symbol")); // First Symbol
  dialog->addSymbolItem(QObject::tr("Second Symbol"), QObject::tr("Details"), s, getData("Second Symbol")); // Second Symbol
  dialog->addComboItem(QObject::tr("Method"), QObject::tr("Details"), l, getData("Method")); // Method
  dialog->addCheckItem(QObject::tr("Rebuild"), QObject::tr("Details"), getData("Rebuild").toInt()); // Rebuild
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    QString s = dialog->getSymbol(QObject::tr("First Symbol"));
    if (s.length())
      setData("First Symbol", s);
      
    s = dialog->getSymbol(QObject::tr("Second Symbol"));
    if (s.length())
      setData("Second Symbol", s);
      
    setData("Method", dialog->getCombo(QObject::tr("Method")));
    setData("Rebuild", QString::number(dialog->getCheck(QObject::tr("Rebuild"))));
  }
  
  delete dialog;
}

void Spread::updateSpread ()
{
  data.clear();
  fdate = 99999999999999.0;
  
  QString fs = getData("First Symbol");
  if (! fs.length())
    return;
    
  QString ss = getData("Second Symbol");
  if (! ss.length())
    return;
  
  QString meth = getData("Method");

  loadData(fs, meth);

  loadData(ss, meth);

  Bar *r = data.find(QString::number(fdate, 'f', 0));
  if (r)
  {
    setBar(r);
    
    if (r->getData("Count") != 2)
      deleteData(QString::number(fdate, 'f', 0));
  }
  
  QDictIterator<Bar> it(data);
  for (; it.current(); ++it)
  {
    r = it.current();
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
    qDebug("Spread::loadData: can't open db");
    config.closePlugin(plugin);
    return;
  }
  
  db->setBarCompression(BarData::DailyBar);
  db->setBarRange(99999999);
  
  bool rebuild = getData("Rebuild").toInt();
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
      
      if (r->getDate().getDateValue() < fdate)
        fdate = r->getDate().getDateValue();
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

  config.closePlugin(plugin);
}

void Spread::createNew ()
{
  bool ok = FALSE;
  QString spread = QInputDialog::getText(QObject::tr("New Spread"),
                                         QObject::tr("Enter symbol name for the new Spread"),
					 QLineEdit::Normal,
					 QString::null,
					 &ok,
					 0);
  if (! spread.length() || ok == FALSE)
    return;

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
      return;
    }
  }
  
  s.append("/" + spread);
  if (dir.exists(s))
  {
    QMessageBox::information(0,
                             QObject::tr("Qtstalker: Error"),
			     QObject::tr("This Spread already exists."));
    return;
  }

  openChart(s);

  setHeaderField(Symbol, spread);  
  setHeaderField(Type, "Spread");  
  setHeaderField(Title, spread);  
  setHeaderField(BarType, QString::number(BarData::Daily));  
  setHeaderField(Plugin, "Spread");  
  
  dbPrefDialog();
}

Bar * Spread::getBar (QString k, QString d)
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

void Spread::setBar (Bar *bar)
{
  if (getHeaderField(BarType).toInt())
  {
    if (! bar->getTickFlag())
      return;
  }
  else
  {
    if (bar->getTickFlag())
      return;
  }

  QStringList l;
  l.append(QString::number(bar->getOpen()));
  l.append(QString::number(bar->getHigh()));
  l.append(QString::number(bar->getLow()));
  l.append(QString::number(bar->getClose()));
  setData(bar->getDate().getDateTimeString(FALSE), l.join(","));
}

//********************************************************************
//********************************************************************
//********************************************************************

DbPlugin * createDbPlugin ()
{
  Spread *o = new Spread;
  return ((DbPlugin *) o);
}

