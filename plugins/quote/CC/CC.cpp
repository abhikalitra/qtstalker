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

#include "CC.h"
#include "PrefDialog.h"
#include <qtimer.h>
#include <qstringlist.h>
#include <qdatetime.h>
#include <qsettings.h>

CC::CC ()
{
  pluginName = "CC";
  loadSettings();
}

CC::~CC ()
{
}

void CC::update ()
{
  QTimer::singleShot(250, this, SLOT(parse()));
}

void CC::parse ()
{
  emit statusLogMessage("CC: Starting...");
  
  QString path = createDirectory("CC");
  if (! path.length())
  {
    emit statusLogMessage("Unable to create directory...bailing out");
    return;
  }
  path.append("/");

  QStringList symbols = fd.getSymbolList();

  int symbolLoop;
  for (symbolLoop = 0; symbolLoop < (int) symbols.count(); symbolLoop++)
  {
    QString s = tr("Updating ");
    s.append(symbols[symbolLoop]);
    emit statusLogMessage(s);

    s = dataPath;
    s.append("/Futures/");
    s.append(symbols[symbolLoop]);
    QDir dir(s);
    if (! dir.exists(s, TRUE))
      continue;
    dir.setPath(s);

    s = path;
    s.append(symbols[symbolLoop]);

    dir.remove(s, TRUE);

    ChartDb *db = new ChartDb();
    db->openChart(s);
    newChart(db, symbols[symbolLoop], dir);
    delete db;
  }

  emit statusLogMessage("Done");
  emit done();
}

void CC::newChart (ChartDb *db, QString symbol, QDir dir)
{
  QDate tdate = QDate::currentDate();
  int years = tdate.year() - maxYears;

  db->setDetail(ChartDb::Type, "Futures");
  db->setDetail(ChartDb::Symbol, symbol);
  QString s = symbol;
  s.append(" - Continuous Adjusted");
  db->setDetail(ChartDb::Title, s);
  db->setDetail(ChartDb::FuturesType, symbol);
  db->setDetail(ChartDb::BarType, QString::number(BarData::Daily));

  if (fd.setSymbol(symbol))
  {
    qDebug("CC::newChart:invalid futures symbol");
    return;
  }
  QString currentContract = fd.getContract();

  Bar *pr = new Bar;
  
  BarDate startDate;

  bool flag = FALSE;
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    s = dir[loop];
    s.truncate(s.length() - 1);
    if (s.right(4).toInt() < years)
      continue;

    if (! dir[loop].compare(currentContract))
      flag = TRUE;

    s = dir.absPath();
    s.append("/");
    s.append(dir[loop]);

    ChartDb *tdb = new ChartDb();
    tdb->openChart(s);
    
    Bar *bar = tdb->getFirstBar();
    if (! bar)
    {
      qDebug("CC::newChart:no first bar %s", dir[loop].latin1());
      delete tdb;
      continue;
    }
    if (! startDate.getDate().isValid())
      startDate = bar->getDate();
    delete bar;

    tdb->setBarCompression(ChartDb::Daily);
    tdb->setBarRange(260);
    BarData *recordList = tdb->getHistory();
    delete tdb;

    int loop2;
    int count = recordList->count() - rollover;
    if (flag)
      count = recordList->count();
    if (count < 1)
    {
      delete recordList;
      continue;
    }

    for (loop2 = 1; loop2 < count; loop2++)
    {
      if (recordList->getDate(loop2).getDate() < startDate.getDate())
        continue;
	
      if (! pr->getDate().getDate().isValid())
        pr->setClose(recordList->getClose(loop2 - 1));

      double c = pr->getClose() + (recordList->getClose(loop2) - recordList->getClose(loop2 - 1));
      double h = c + (recordList->getHigh(loop2) - recordList->getClose(loop2));
      double l = c - (recordList->getClose(loop2) - recordList->getLow(loop2));
      double o = h - (recordList->getHigh(loop2) - recordList->getOpen(loop2));

      delete pr;

      pr = new Bar;
      pr->setDate(recordList->getDate(loop2));
      pr->setOpen(o);
      pr->setHigh(h);
      pr->setLow(l);
      pr->setClose(c);
      pr->setVolume(recordList->getVolume(loop2));
      pr->setOI(recordList->getOI(loop2));
      db->setBar(pr);
      
      startDate = recordList->getDate(loop2);
    }

    delete recordList;

    if (flag)
      break;
  }

  delete pr;
}

void CC::prefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("CC Prefs"));
  dialog->createPage (tr("Details"));
  dialog->addIntItem(tr("Rollover"), tr("Details"), rollover);
  dialog->addIntItem(tr("Maximum Years"), tr("Details"), maxYears);
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    rollover = dialog->getInt(tr("Rollover"));
    maxYears = dialog->getInt(tr("Maximum Years"));
    saveFlag = TRUE;
  }
  
  delete dialog;
}

void CC::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/CC plugin");

  QString s = settings.readEntry("/Rollover", "20");
  rollover = s.toInt();
  
  s = settings.readEntry("/MaxYears", "10");
  maxYears = s.toInt();
  
  settings.endGroup();
}

void CC::saveSettings ()
{
  if (! saveFlag)
    return;
    
  QSettings settings;
  settings.beginGroup("/Qtstalker/CC plugin");
  
  settings.writeEntry("/Rollover", QString::number(rollover));
  settings.writeEntry("/MaxYears", QString::number(maxYears));
  
  settings.endGroup();
}

Plugin * create ()
{
  CC *o = new CC;
  return ((Plugin *) o);
}




