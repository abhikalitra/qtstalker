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
#include "ChartDb.h"
#include <qdir.h>
#include <qstringlist.h>
#include <qmessagebox.h>
#include <qtimer.h>

Spread::Spread ()
{
  pluginName = "Spread";
  createFlag = TRUE;
  data.setAutoDelete(TRUE);

  about = "Creates and updates a chart to form a spread chart.\n";
}

Spread::~Spread ()
{
}

void Spread::update ()
{
  QTimer::singleShot(250, this, SLOT(updateSpread()));
}

void Spread::updateSpread ()
{
  data.clear();

  QString s = dataPath;
  s.append("/Spread");
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
    db->saveDetails();
    db->setFormat();

    loadData(details->getData(tr("First Symbol")), details->getData(tr("Method")));

    loadData(details->getData(tr("Second Symbol")), details->getData(tr("Method")));

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

void Spread::loadData (QString symbol, QString method)
{
  ChartDb *tdb = new ChartDb();
  if (tdb->openChart(symbol))
  {
    qDebug("could not open db");
    delete tdb;
    return;
  }

  Setting *details = tdb->getDetails();

  QDateTime dt = tdb->getDateTime(details->getData("First Date"));

  QList<Setting> *recordList = tdb->getHistory(ChartDb::Daily, dt);

  int loop;
  for (loop = 0; loop < (int) recordList->count(); loop++)
  {
    Setting *tr = recordList->at(loop);
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
      if (! method.compare(QObject::tr("Subtract")))
      {
        r->setData("Close", QString::number(r->getFloat("Close") - (tr->getFloat("Close"))));
        r->setData("Volume", QString::number(r->getFloat("Volume") - (tr->getFloat("Volume"))));
        r->setData("Open Interest", QString::number(r->getFloat("Open Interest") - (tr->getFloat("Open Interest"))));
      }

      if (! method.compare(QObject::tr("Divide")))
      {
        r->setData("Close", QString::number(r->getFloat("Close") / (tr->getFloat("Close"))));
        r->setData("Volume", QString::number(r->getFloat("Volume") / (tr->getFloat("Volume"))));
        r->setData("Open Interest", QString::number(r->getFloat("Open Interest") / (tr->getFloat("Open Interest"))));
      }

      r->setData("Open", r->getData("Close"));
      r->setData("High", r->getData("Close"));
      r->setData("Low", r->getData("Close"));
      r->setData("Count", "2");
    }
  }

  delete recordList;
  delete tdb;
}

Setting * Spread::getCreateDetails ()
{
  Setting *set = new Setting;
  set->set(tr("First Symbol"), "", Setting::Symbol);
  set->set(tr("Second Symbol"), "", Setting::Symbol);

  QStringList l;
  l.append(tr("Subtract"));
  l.append(tr("Divide"));
  l.sort();
  set->set(tr("Method"), tr("Subtract"), Setting::List);
  set->setList(tr("Method"), l);

  return set;
}

void Spread::createChart (Setting *set)
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

  QString path = createDirectory("Spread");
  if (! path.length())
  {
    qDebug("Spread plugin: Unable to create directory");
    return;
  }

  QStringList l = QStringList::split("/", set->getData(tr("First Symbol")), FALSE);
  QString symbol = l[l.count() - 1];
  symbol.append("-");
  l = QStringList::split("/", set->getData(tr("Second Symbol")), FALSE);
  symbol.append(l[l.count() - 1]);

  path.append("/");
  path.append(symbol);
  QDir dir(path);
  if (dir.exists(path, TRUE))
  {
    QMessageBox::information(0, tr("Error"), tr("Duplicate chart"), 0, 0, QMessageBox::Ok);
    return;
  }

  ChartDb *db = new ChartDb();
  db->openChart(path);

  Setting *details = db->getDetails();
  details->set("Format", "Open|High|Low|Close|Volume|Open Interest", Setting::None);
  details->set("Chart Type", tr("Spread"), Setting::None);
  details->set("Method", set->getData(tr("Method")), Setting::None);
  details->set("Symbol", symbol, Setting::None);
  details->set("Title", symbol, Setting::Text);
  details->set("First Symbol", set->getData(tr("First Symbol")), Setting::Symbol);
  details->set("Second Symbol", set->getData(tr("Second Symbol")), Setting::Symbol);

  db->saveDetails();

  delete db;
}

Plugin * create ()
{
  Spread *o = new Spread;
  return ((Plugin *) o);
}

