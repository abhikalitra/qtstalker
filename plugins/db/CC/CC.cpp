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

BarData * CC::getHistory ()
{
  update();
  return DbPlugin::getHistory();
}

void CC::dbPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setCaption(QObject::tr("CC Prefs"));
  dialog->createPage (QObject::tr("Details"));
  dialog->setHelpFile (helpFile);
  dialog->addIntItem(QObject::tr("Maximum Years"), QObject::tr("Details"), getData("Maximum Years").toInt());
  dialog->addCheckItem(QObject::tr("Rebuild"), QObject::tr("Details"), getData("Details").toInt());
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    setData("Maximum Years", QString::number(dialog->getInt(QObject::tr("Maximum Years"))));
    setData("Rebuild", QString::number(dialog->getCheck(QObject::tr("Rebuild"))));
  }
  
  delete dialog;
}

void CC::update ()
{
  QDateTime startDate = QDateTime::currentDateTime();

  // figure out if we have to rebuild chart
  if (! getData("Rebuild").toInt())
  {
    BarDate dt;
    if (! dt.setDate(getData("Last Rebuild Date")))
    {
      if (dt.getDate() == startDate.date())
        return; // no rebuild since chart was updated this same day
      else
        setData("Last Rebuild Date", startDate.toString("yyyyMMdd000000"));
    }
    else
      setData("Last Rebuild Date", startDate.toString("yyyyMMdd000000"));
  }
  
  Config config;
  QString baseDir = config.getData(Config::DataPath) + "/Futures/" + getHeaderField(Symbol);
  QDir dir(baseDir);
  if (! dir.exists(baseDir, TRUE))
    return;
    
  int maxYears = getData("Maximum Years").toInt();
  
  FuturesData fd;
  if (fd.setSymbol(getHeaderField(Symbol)))
  {
    qDebug("CC::newChart:invalid futures symbol");
    return;
  }
  
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

    BarData *recordList = tdb->getHistory();

    int loop2;
    for (loop2 = 1; loop2 < (int) recordList->count(); loop2++)
    {
      Dummy *r = data.find(recordList->getDate(loop2).getDateTimeString(FALSE));
      if (r)
        continue;
	
      double c = pr + (recordList->getClose(loop2) - recordList->getClose(loop2 - 1));
      double h = c + (recordList->getHigh(loop2) - recordList->getClose(loop2));
      double l = c - (recordList->getClose(loop2) - recordList->getLow(loop2));
      double o = h - (recordList->getHigh(loop2) - recordList->getOpen(loop2));
	
      Bar *bar = new Bar;
      bar->setDate(recordList->getDate(loop2));
      bar->setOpen(o);
      bar->setHigh(h);
      bar->setLow(l);
      bar->setClose(c);
      bar->setVolume(recordList->getVolume(loop2));
      bar->setOI((int) recordList->getOI(loop2));
      setBar(bar);
      pr = c;
      data.insert(bar->getDate().getDateTimeString(FALSE), new Dummy);
      delete bar;
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
					 fd.getSymbolList(),
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
  setHeaderField(Type, "CC");  
  
  s = symbol + " - Continuous Adjusted";
  setHeaderField(Title, s);  
  
  setHeaderField(BarType, QString::number(BarData::Daily));  
  setHeaderField(Plugin, "CC");  
  setData("Maximum Years", QString::number(10)); // MAXYEARS
  
  dbPrefDialog();
}

Bar * CC::getBar (QString k, QString d)
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

void CC::setBar (Bar *bar)
{
  if (getHeaderField(BarType).toInt() != bar->getTickFlag())
    return;

  QStringList l;
  l.append(QString::number(bar->getOpen()));
  l.append(QString::number(bar->getHigh()));
  l.append(QString::number(bar->getLow()));
  l.append(QString::number(bar->getClose()));
  l.append(QString::number(bar->getVolume(), 'f', 0));
  l.append(QString::number(bar->getOI()));
  setData(bar->getDate().getDateTimeString(FALSE), l.join(","));
}

//***********************************************************
//***********************************************************
//***********************************************************

DbPlugin * createDbPlugin ()
{
  CC *o = new CC;
  return ((DbPlugin *) o);
}

