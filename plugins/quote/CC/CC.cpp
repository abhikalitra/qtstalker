/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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
#include <qtimer.h>
#include <qstringlist.h>
#include <qdatetime.h>

CC::CC ()
{
  pluginName = "CC";
  version = 0.2;
  createFlag = FALSE;
  
  set("Rollover","20", Setting::Integer);

  set("Maximum Years","10", Setting::Integer);

  about = "Creates a continuous adjusted futures contract\n";
  about.append("for the selected futures symbol.");
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
  QString s = dataPath;
  s.append("/CC");
  QDir dir(s);
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
    {
      qDebug("CC plugin: Unable to create directory");
      return;
    }
  }

  FuturesData *fd = new FuturesData;
  QStringList symbols = fd->getSymbolList();

  int symbolLoop;
  for (symbolLoop = 0; symbolLoop < (int) symbols.count(); symbolLoop++)
  {
    s = tr("Updating ");
    s.append(symbols[symbolLoop]);
    emit message(s);

    s = dataPath;
    s.append("/Futures/");
    s.append(symbols[symbolLoop]);
    if (! dir.exists(s, TRUE))
      continue;
    dir.setPath(s);

    s = dataPath;
    s.append("/CC/");
    s.append(symbols[symbolLoop]);
    
    dir.remove(s, TRUE);

    ChartDb *db = new ChartDb();
    db->openChart(s);
    newChart(db, symbols[symbolLoop], fd, dir);
    delete db;
  }

  delete fd;

  emit done();
}

void CC::newChart (ChartDb *db, QString symbol, FuturesData *fd, QDir dir)
{
  int rollover = getInt("Rollover");
  
  QDate tdate = QDate::currentDate();
  int years = tdate.year() - getInt("Maximum Years");

  Setting *details = db->getDetails();
  details->set("Format", "Open|High|Low|Close|Volume|Open Interest", Setting::None);
  details->set("Chart Type", tr("Futures"), Setting::None);
  details->set("Symbol", symbol, Setting::None);
  QString s = symbol;
  s.append(" - Continuous Adjusted");
  details->set("Title", s, Setting::Text);
  details->set("Futures Type", symbol, Setting::None);
  db->saveDetails();
  db->setFormat();

  fd->setSymbol(symbol);
  QString currentContract = fd->getContract();

  Setting *pr = new Setting;
  
  QDateTime startDate;

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
    Setting *tdetails = tdb->getDetails();

    if (! startDate.isValid())
      startDate = QDateTime::fromString(tdetails->getDateTime("First Date"), Qt::ISODate);

    QList<Setting> *recordList = tdb->getHistory(ChartDb::Daily, startDate);

    int loop2;
    int count = recordList->count() - rollover;
    if (flag)
      count = recordList->count();
    if (count < 1)
    {
      delete recordList;
      delete tdb;
      continue;
    }

    for (loop2 = 1; loop2 < count; loop2++)
    {
      Setting *r = recordList->at(loop2);
      Setting *r2 = recordList->at(loop2 - 1);

      if (! pr->count())
        pr->set("Close", r2->getData("Close"), Setting::Float);

      double c = pr->getFloat("Close") + (r->getFloat("Close") - r2->getFloat("Close"));
      double h = c + (r->getFloat("High") - r->getFloat("Close"));
      double l = c - (r->getFloat("Close") - r->getFloat("Low"));
      double o = h - (r->getFloat("High") - r->getFloat("Open"));

      delete pr;

      pr = new Setting;
      pr->set("Date", r->getData("Date"), Setting::Date);
      pr->set("Open", QString::number(o), Setting::Float);
      pr->set("High", QString::number(h), Setting::Float);
      pr->set("Low", QString::number(l), Setting::Float);
      pr->set("Close", QString::number(c), Setting::Float);
      pr->set("Volume", r->getData("Volume"), Setting::Float);
      pr->set("Open Interest", r->getData("Open Interest"), Setting::Float);
      db->setRecord(pr);

      startDate = QDateTime::fromString(r->getDateTime("Date"), Qt::ISODate);
    }

    details->set("Last Contract", dir[loop], Setting::None);
    db->saveDetails();

    delete recordList;
    delete tdb;

    if (flag)
      break;
  }

  delete pr;
}

Plugin * create ()
{
  CC *o = new CC;
  return ((Plugin *) o);
}




