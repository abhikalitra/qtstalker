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

#include "Ratio.h"
#include "ChartDb.h"
#include <qdir.h>
#include <qstringlist.h>
#include <qmessagebox.h>
#include <qtimer.h>

Ratio::Ratio ()
{
  pluginName = "Ratio";
  version = 0.2;
  createFlag = TRUE;
  data.setAutoDelete(TRUE);

  about = "Creates and updates a chart to form a ratio chart.\n";
}

Ratio::~Ratio ()
{
}

void Ratio::update ()
{
  QTimer::singleShot(250, this, SLOT(updateRatio()));
}

void Ratio::updateRatio ()
{
  data.clear();

  QString s = dataPath;
  s.append("/Ratio");
  QDir dir(s);

  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    s = dir.path();
    s.append("/");
    s.append(dir[loop]);

    ChartDb *db = new ChartDb();
    if (db->openChart(s))
    {
      qDebug("could not open db");
      delete db;
      continue;
    }

    Setting *details = db->getDetails();
    Setting *tdetails = new Setting;
    tdetails->parse(details->getString());
    tdetails->remove("First Date");
    tdetails->remove("Last Date");

    delete db;
    dir.remove(s, TRUE);
    db = new ChartDb();
    if (db->openChart(s))
    {
      qDebug("could not open db");
      delete db;
      delete tdetails;
      continue;
    }

    s = tr("Updating ");
    s.append(dir[loop]);
    emit message(s);

    details = db->getDetails();
    details->parse(tdetails->getString());
    delete tdetails;

    loadData(details->getData(tr("First Symbol")));

    loadData(details->getData(tr("Second Symbol")));

    QDictIterator<Setting> it(data);
    for (; it.current(); ++it)
    {
      Setting *r = it.current();
      if (r->getInt("Count") == 2)
        db->setRecord(r);
    }
    
    delete db;

    data.clear();
  }
  
  emit done();
}

void Ratio::loadData (QString symbol)
{
  QString s = dataPath;
  s.append("/");
  s.append(symbol);

  ChartDb *db = new ChartDb();
  if (db->openChart(s))
  {
    qDebug("could not open db");
    delete db;
    return;
  }
  
  Setting *details = db->getDetails();

  QDateTime dt = db->getDateTime(details->getData("First Date"));

  db->getHistory(ChartDb::Daily, dt);

  int loop;
  for (loop = 0; loop < db->getDataSize(); loop++)
  {
    Setting *tr = db->getRecordIndex(loop);
    Setting *r = data.find(tr->getData("Date"));
    if (! r)
    {
      r = new Setting;
      r->set("Date", tr->getData("Date"), Setting::Date);
      r->set("Close", QString::number(tr->getFloat("Close")), Setting::Float);
      r->set("Volume", QString::number(tr->getFloat("Volume")), Setting::Float);
      r->set("Open Interest", QString::number(tr->getFloat("Open Interest")), Setting::Float);
      r->set("Open", r->getData("Close"), Setting::Float);
      r->set("High", r->getData("Close"), Setting::Float);
      r->set("Low", r->getData("Close"), Setting::Float);
      r->set("Count", "1", Setting::Integer);
      data.insert(r->getData("Date"), r);
    }
    else
    {
      r->setData("Close", QString::number(r->getFloat("Close") / (tr->getFloat("Close"))));
      r->setData("Volume", QString::number(r->getFloat("Volume") / (tr->getFloat("Volume"))));
      r->setData("Open Interest", QString::number(r->getFloat("Open Interest") / (tr->getFloat("Open Interest"))));
      r->setData("Open", r->getData("Close"));
      r->setData("High", r->getData("Close"));
      r->setData("Low", r->getData("Close"));
      r->setData("Count", QString::number(r->getFloat("Count") + 1));
    }
  }

  delete db;
}

Setting * Ratio::getCreateDetails ()
{
  Setting *set = new Setting;
  set->set(tr("First Symbol"), "", Setting::Symbol);
  set->set(tr("Second Symbol"), "", Setting::Symbol);
  return set;
}

void Ratio::createChart (Setting *set)
{
  QString s = set->getData(tr("First Symbol"));
  if (! s.length())
  {
    QMessageBox::information(0, tr("Error"), tr("Invalid first symbol"), 0, 0, QMessageBox::Ok);
    return;
  }

  s = set->getData(tr("Second Symbol"));
  if (! s.length())
  {
    QMessageBox::information(0, tr("Error"), tr("Invalid second symbol"), 0, 0, QMessageBox::Ok);
    return;
  }

  QString base = dataPath;
  base.append("/Ratio");
  QDir dir(base);
  if (! dir.exists(base, TRUE))
  {
    if (! dir.mkdir(base, TRUE))
    {
      qDebug("Ratio plugin: Unable to create directory");
      return;
    }
  }
  base.append("/");

  QStringList l = QStringList::split("/", set->getData(tr("First Symbol")), FALSE);
  QString symbol = l[l.count() - 1];
  symbol.append("-");
  l = QStringList::split("/", set->getData(tr("Second Symbol")), FALSE);
  symbol.append(l[l.count() - 1]);

  s = base;
  s.append(symbol);
  if (dir.exists(s, TRUE))
  {
    QMessageBox::information(0, tr("Error"), tr("Duplicate chart"), 0, 0, QMessageBox::Ok);
    return;
  }

  ChartDb *db = new ChartDb();
  db->openChart(s);

  Setting *details = db->getDetails();
  details->set("Format", "Open|High|Low|Close|Volume|Open Interest", Setting::None);
  details->set("Chart Type", tr("Ratio"), Setting::None);
  details->set("Symbol", symbol, Setting::None);
  details->set("Title", symbol, Setting::Text);
  details->set("First Symbol", set->getData(tr("First Symbol")), Setting::Symbol);
  details->set("Second Symbol", set->getData(tr("Second Symbol")), Setting::Symbol);

  db->saveDetails();

  delete db;
}

Plugin * create ()
{
  Ratio *o = new Ratio;
  return ((Plugin *) o);
}

