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

#include "CC.h"
#include "DbPlugin.h"
#include "FuturesData.h"
#include "PrefDialog.h"
#include "Config.h"
#include "BarData.h"
#include <qstring.h>
#include <qdir.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qobject.h>


CC::CC ()
{
  helpFile = "cc.html";
}

CC::~CC ()
{
}

void CC::getHistory (BarData *bd)
{
  update();
  DbPlugin::getHistory(bd);
}

void CC::dbPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("CC Prefs"));
  dialog->createPage(QObject::tr("Details"));
  dialog->setHelpFile(helpFile);
  
  QString s = "Maximum Years";
  QString s2;
  getData(s, s2);
  dialog->addIntItem(QObject::tr("Maximum Years"), QObject::tr("Details"), s2.toInt());
  
  s = "Details";
  getData(s, s2);
  dialog->addCheckItem(QObject::tr("Rebuild"), QObject::tr("Details"), s2.toInt());
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    s = "Maximum Years";
    s2 = QString::number(dialog->getInt(QObject::tr("Maximum Years")));
    setData(s, s2);
    
    s = "Rebuild";
    s2 = QString::number(dialog->getCheck(QObject::tr("Rebuild")));
    setData(s, s2);
  }
  
  delete dialog;
}

void CC::update ()
{
  QDateTime startDate = QDateTime::currentDateTime();

  // figure out if we have to rebuild chart
  QString s = "Rebuild";
  QString s2;
  getData(s, s2);
  if (! s2.toInt())
  {
    BarDate dt;
    s = "Last Rebuild Date";
    getData(s, s2);
    if (! dt.setDate(s2))
    {
      if (dt.getDate() == startDate.date())
        return; // no rebuild since chart was updated this same day
      else
      {
        s = "Last Rebuild Date";
	s2 = startDate.toString("yyyyMMdd000000");
        setData(s, s2);
      }
    }
    else
    {
      s = "Last Rebuild Date";
      s2 = startDate.toString("yyyyMMdd000000");
      setData(s, s2);
    }
  }

  FuturesData fd;
  getHeaderField(Symbol, s2);
  if (fd.setSymbol(s2))
  {
    qDebug("CC::newChart:invalid futures symbol");
    return;
  }
    
  Config config;
  QString baseDir = config.getData(Config::DataPath) + "/Futures/" + fd.getExchange() + "/" + fd.getSymbol();
  QDir dir(baseDir);
  if (! dir.exists(baseDir, TRUE))
    return;
    
  s = "Maximum Years";
  getData(s, s2);
  int maxYears = s2.toInt();
  
  QString lastChart = fd.getCurrentContract(startDate);
  QString ey = lastChart.right(5);
  ey.truncate(4);
  
  startDate = startDate.addYears(-maxYears);
  double pr = 0;

  // first chart
  QString sy = fd.getCurrentContract(startDate).right(5);
  sy.truncate(4);
  
  QDict<Dummy> data;
  data.setAutoDelete(TRUE);
  bool flag = FALSE;

  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    QString s = dir[loop].right(5);
    s.truncate(4);
    
    if (s.toInt() < sy.toInt())
      continue;
      
    if (s.toInt() > ey.toInt())
      break;
    
    if (! lastChart.compare(dir[loop]))
      flag = TRUE;
      
    s = baseDir + "/" + dir[loop];
    if (! dir.exists(s))
      continue;
    
    DbPlugin *tdb = config.getDbPlugin("Futures");
    if (! tdb)
    {
      config.closePlugin("Futures");
      continue;
    }
      
    tdb->openChart(s);
    tdb->setBarCompression(BarData::DailyBar);
    tdb->setBarRange(100);

    BarData *recordList = new BarData;
    tdb->getHistory(recordList);

    int loop2;
    for (loop2 = 1; loop2 < (int) recordList->count(); loop2++)
    {
      recordList->getDate(loop2).getDateTimeString(FALSE, s);
      Dummy *r = data.find(s);
      if (r)
        continue;
	
      double c = pr + (recordList->getClose(loop2) - recordList->getClose(loop2 - 1));
      double h = c + (recordList->getHigh(loop2) - recordList->getClose(loop2));
      double l = c - (recordList->getClose(loop2) - recordList->getLow(loop2));
      double o = h - (recordList->getHigh(loop2) - recordList->getOpen(loop2));
	
      Bar bar;
      BarDate dt = recordList->getDate(loop2);
      bar.setDate(dt);
      bar.setOpen(o);
      bar.setHigh(h);
      bar.setLow(l);
      bar.setClose(c);
      bar.setVolume(recordList->getVolume(loop2));
      bar.setOI((int) recordList->getOI(loop2));
      setBar(bar);
      pr = c;
      bar.getDate().getDateTimeString(FALSE, s);
      data.insert(s, new Dummy);
    }

    delete recordList;
    config.closePlugin("Futures");
    
    if (flag)
      break;
  }
}

void CC::createNew ()
{
  FuturesData fd;
  bool ok = FALSE;
  QString symbol = QInputDialog::getItem(QObject::tr("New CC"),
                                         QObject::tr("Select futures symbol to create."),
					 fd.getSymbolList("All"),
					 0,
					 FALSE,
					 &ok,
					 0);
  if (! symbol.length() || ok == FALSE)
    return;

  QDir dir;
  Config config;
  QString s = config.getData(Config::DataPath) + "/CC";
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s, TRUE))
    {
      QMessageBox::information(0,
                               QObject::tr("Qtstalker: Error"),
			       QObject::tr("Could not create ~/Qtstalker/data/CC directory."));
      return;
    }
  }
  
  s.append("/" + symbol);
  if (dir.exists(s))
  {
    QMessageBox::information(0,
                             QObject::tr("Qtstalker: Error"),
			     QObject::tr("This CC already exists."));
    return;
  }

  openChart(s);
  
  setHeaderField(Symbol, symbol);  
  
  s = "CC";
  setHeaderField(Type, s);  
  setHeaderField(Plugin, s);  
  
  s = symbol + " - Continuous Adjusted";
  setHeaderField(Title, s);  
  
  s = QString::number(BarData::Daily);
  setHeaderField(BarType, s);
  
  s = "Maximum Years";
  QString s2 = QString::number(10);
  setData(s, s2); // MAXYEARS
  
  dbPrefDialog();
}

Bar * CC::getBar (QString &k, QString &d)
{
  Bar *bar = new Bar;
  QStringList l = QStringList::split(",", d, FALSE);
  bar->setDate(k);
  bar->setOpen(l[0].toDouble());
  bar->setHigh(l[1].toDouble());
  bar->setLow(l[2].toDouble());
  bar->setClose(l[3].toDouble());
  bar->setVolume(l[4].toDouble());
  bar->setOI(l[5].toInt());
  return bar;
}

void CC::setBar (Bar &bar)
{
  QString k;
  getHeaderField(BarType, k);
  if (k.toInt() != bar.getTickFlag())
    return;

  bar.getDate().getDateTimeString(FALSE, k);
  
  QString d = QString::number(bar.getOpen()) + "," + QString::number(bar.getHigh()) + "," +
              QString::number(bar.getLow()) + "," + QString::number(bar.getClose()) + "," +
	      QString::number(bar.getVolume(), 'f', 0) + "," + QString::number(bar.getOI());
  
  setData(k, d);
}

//***********************************************************
//***********************************************************
//***********************************************************

DbPlugin * createDbPlugin ()
{
  CC *o = new CC;
  return ((DbPlugin *) o);
}

