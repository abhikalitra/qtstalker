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

void Spread::getHistory (BarData *bd)
{
  updateSpread();
  DbPlugin::getHistory(bd);
}

void Spread::dbPrefDialog ()
{
  Config config;
  QStringList l;
  l.append("Subtract");
  l.append("Divide");
  
  QString s = config.getData(Config::DataPath);
  
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setHelpFile (helpFile);
  dialog->setCaption(QObject::tr("Spread Prefs"));
  
  QString pl = QObject::tr("Details");
  dialog->createPage (pl);
  
  QString t = QObject::tr("Symbol");
  QString t2;
  getHeaderField(DbPlugin::Symbol, t2);
  dialog->addLabelItem(t, pl, t2);

  t = QObject::tr("Name");
  getHeaderField(DbPlugin::Title, t2);
  dialog->addTextItem(t, pl, t2);

  t = QObject::tr("Type");
  getHeaderField(DbPlugin::Type, t2);
  dialog->addLabelItem(t, pl, t2);
  
  t = QObject::tr("First Date");
  t2.truncate(0);
  Bar *bar = getFirstBar();
  if (bar)
  {
    bar->getDate().getDateTimeString(TRUE, t2);
    delete bar;
  }
  dialog->addLabelItem(t, pl, t2);
  
  t = QObject::tr("Last Date");
  t2.truncate(0);
  bar = getLastBar();
  if (bar)
  {
    bar->getDate().getDateTimeString(TRUE, t2);
    delete bar;
  }
  dialog->addLabelItem(t, pl, t2);
  
  pl = QObject::tr("Parms");
  dialog->createPage (pl);
  
  t = "First Symbol";
  getData(t, t2);
  t = QObject::tr("First Symbol");
  dialog->addSymbolItem(t, pl, s, t2); // First Symbol

  t = "Second Symbol";
  getData(t, t2);
  t = QObject::tr("Second Symbol");
  dialog->addSymbolItem(t, pl, s, t2); // Second Symbol
  
  t = "Method";
  getData(t, t2);
  t = QObject::tr("Method");
  dialog->addComboItem(t, pl, l, t2); // Method
  
  t = "Rebuild";
  getData(t, t2);
  t = QObject::tr("Rebuild");
  dialog->addCheckItem(t, pl, t2.toInt()); // Rebuild
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    t = QObject::tr("Name");
    t2 = dialog->getText(t);
    setHeaderField(DbPlugin::Title, t2);

    t = QObject::tr("First Symbol");
    t2 = dialog->getSymbol(t);
    if (t2.length())
    {
      t = "First Symbol";
      setData(t, t2);
    }
      
    t = QObject::tr("Second Symbol");
    t2 = dialog->getSymbol(t);
    if (t2.length())
    {
      t = "Second Symbol";
      setData(t, t2);
    }

    t = QObject::tr("Method");
    t2 = dialog->getCombo(t);
    t = "Method";
    setData(t, t2);
    
    t = QObject::tr("Rebuild");
    t2 = QString::number(dialog->getCheck(t));
    t = "Rebuild";
    setData(t, t2);
  }
  
  delete dialog;
}

void Spread::updateSpread ()
{
  data.clear();
  fdate = 99999999999999.0;
  
  QString s = "First Symbol";
  QString fs;
  getData(s, fs);
  if (! fs.length())
    return;
  
  s = "Second Symbol";
  QString ss;
  getData(s, ss);
  if (! ss.length())
    return;
    
  emit signalStatusMessage(QString(tr("Updating spread...")));
  emit signalProgMessage(0, 3);
  
  s = "Method";
  QString meth;
  getData(s, meth);

  loadData(fs, meth);
  emit signalProgMessage(1, 3);
  
  loadData(ss, meth);
  emit signalProgMessage(2, 3);

  Bar *r = data.find(QString::number(fdate, 'f', 0));
  if (r)
  {
    Bar &bar = *r;
    setBar(bar);

    s = "Count";
    if (r->getData(s) != 2)
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
    if (r->getData(s) == 2)
    {
      Bar bar;
      BarDate dt = r->getDate();
      bar.setDate(dt);
      bar.setOpen(r->getClose());
      bar.setHigh(r->getClose());
      bar.setLow(r->getClose());
      bar.setClose(r->getClose());
      setBar(bar);
    }
  }

  data.clear();
  emit signalProgMessage(-1, -1);
}

void Spread::loadData (QString &symbol, QString &method)
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
      r->setClose(recordList->getClose(loop));
      s = "Count";
      r->setData(s, 1);
      r->getDate().getDateTimeString(FALSE, s);
      data.insert(s, r);
      
      if (r->getDate().getDateValue() < fdate)
        fdate = r->getDate().getDateValue();
    }
    else
    {
      if (! method.compare("Subtract"))
        r->setClose(r->getClose() - recordList->getClose(loop));

      if (! method.compare("Divide"))
        r->setClose(r->getClose() / recordList->getClose(loop));
      
      s = "Count";
      r->setData(s, 2);
    }
  }

  delete recordList;
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
  
  s = "Spread";
  setHeaderField(Type, s);  
  setHeaderField(Plugin, s);  
  setHeaderField(Title, spread);
  s = QString::number(BarData::Daily);
  setHeaderField(BarType, s);  
  
  dbPrefDialog();
}

Bar * Spread::getBar (QString &k, QString &d)
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

void Spread::setBar (Bar &bar)
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
  Spread *o = new Spread;
  return ((DbPlugin *) o);
}

