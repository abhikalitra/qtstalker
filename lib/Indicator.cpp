/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#include "Indicator.h"
#include "DataDataBase.h"
#include "Globals.h"
#include "PluginFactory.h"
#include "IndicatorEditDialog.h"

#include <QDebug>
#include <QSettings>

Indicator::Indicator (QObject *p) : QObject (p)
{
  init();
}

Indicator::Indicator ()
{
  init();
}

Indicator::~Indicator ()
{
}

void Indicator::init ()
{
  _lock = 0;
  _date = 1;
  _log = 0;
  _name.clear();
}

void Indicator::setName (QString d)
{
  _name = d;
}

QString Indicator::name ()
{
  return _name;
}

void Indicator::setLock (bool d)
{
  _lock = d;
}

bool Indicator::lock ()
{
  return _lock;
}

void Indicator::setDate (bool d)
{
  _date = d;
}

bool Indicator::date ()
{
  return _date;
}

void Indicator::setLog (bool d)
{
  _log = d;
}

bool Indicator::log ()
{
  return _log;
}

void Indicator::setLine (QString k, Curve *d)
{
  Curve *line = _lines.value(k);
  if (line)
    delete line;
  
  _lines.insert(k, d);
}

Curve * Indicator::line (QString k)
{
  return _lines.value(k);
}

int Indicator::deleteLine (QString k)
{
  Curve *line = _lines.value(k);
  if (! line)
    return 1;

  delete line;
  _lines.remove(k);

  return 0;
}

QHash<QString, Curve *> &  Indicator::curves ()
{
  return _lines;
}

void Indicator::clear ()
{
  clearLines();
  clearChartObjects();
}

Setting Indicator::chartObject (QString k)
{
  return _chartObjects.value(k);
}

void Indicator::addChartObject (Setting co)
{
  _chartObjects.insert(co.data("ID"), co);
}

void Indicator::clearChartObjects ()
{
  _chartObjects.clear();
}

void Indicator::weedPlots ()
{
  QHashIterator<QString, Curve *> it(_lines);
  while (it.hasNext())
  {
    it.next();
    Curve *curve = it.value();
    if (! curve)
      continue;
    
    if (curve->z() < 0)
    {
      delete curve;
      _lines.remove(it.key());
    }
  }
}

void Indicator::clearLines ()
{
  if (_lines.count())
  {
    qDeleteAll(_lines);
    _lines.clear();
  }
}

int Indicator::save ()
{
  DataDataBase db("indicators");
  db.transaction();
  
  if (db.removeName(_name))
    return 1;

  if (db.save(_name, "LOCK", QString::number((int) _lock)))
    return 1;
  
  if (db.save(_name, "DATE_AXIS", QString::number((int) _date)))
    return 1;

  if (db.save(_name, "LOG_SCALING", QString::number((int) _log)))
    return 1;

  if (db.save(_name, _commands))
    return 1;

  db.commit();

  return 0;
}

int Indicator::load ()
{
  DataDataBase db("indicators");

  QString data;
  if (db.load(_name, "LOCK", data))
    return 1;
  if (! data.isEmpty())
    _lock = data.toInt();

  if (db.load(_name, "DATE_AXIS", data))
    return 1;
  if (! data.isEmpty())
    _date = data.toInt();

  if (db.load(_name, "LOG_SCALING", data))
    return 1;
  if (! data.isEmpty())
    _log = data.toInt();

  if (db.load(_name, _commands))
    return 1;

  return 0;
}

void Indicator::calculate ()
{
  PluginFactory fac;
  int loop = 0;
  for (; loop < _commands.count(); loop++)
  {
    Command command(_commands.at(loop));
    if (! command.count())
      continue;

    Plugin *plug = fac.plugin(command.plugin());
    if (! plug)
    {
      qDebug() << "Indicator::calculate: syntax error" << command.plugin();
      continue;
    }

    command.setIndicator(this);
    command.setBarData(g_barData);
    plug->command(&command);
  }

  weedPlots();

  loadChartObjects();

  emit signalPlot();
}

void Indicator::loadChartObjects ()
{
  DataDataBase db("chartObjects");
  QStringList names;
  QStringList l;
  l << _name << g_barData->exchange() << g_barData->symbol();
  if (db.search("KEY", l.join("_"), names))
  {
    qDebug() << "Indicator::loadChartObjects: search error";
    return;
  }

  int loop = 0;
  for (; loop < names.count(); loop++)
  {
    Setting set;
    if (db.load(names.at(loop), &set))
      continue;
    
    addChartObject(set);
  }
}

QHash<QString, Setting> &  Indicator::chartObjects ()
{
  return _chartObjects;
}

int Indicator::chartObjectCount ()
{
  return _chartObjects.count();
}

int Indicator::lineCount ()
{
  return _lines.count();
}

void Indicator::dialog ()
{
  IndicatorEditDialog *dialog = new IndicatorEditDialog(g_parent, _name);
  connect(dialog, SIGNAL(signalDone()), this, SLOT(dialogDone()));
  dialog->show();
}

void Indicator::dialogDone ()
{
  emit signalClear();
  load();
  calculate();
}

void Indicator::remove (int delFlag, QStringList l)
{
  QSettings set(g_localSettings);
  QStringList tl = set.value("indicators").toStringList();

  int loop = 0;
  for (; loop < l.count(); loop++)
    tl.removeAll(l.at(loop));
  set.setValue("indicators", tl);
  set.sync();

  if (delFlag)
  {
    DataDataBase db("indicators");
    db.transaction();
    loop = 0;
    for (; loop < l.count(); loop++)
      db.removeName(l.at(loop));
    db.commit();
  }
}

void Indicator::add (QString d)
{
  QSettings set(g_localSettings);
  QStringList l = set.value("indicators").toStringList();
  l.append(d);
  l.removeDuplicates();
  set.setValue("indicators", l);
  set.sync();
}

void Indicator::setCommands (QStringList &l)
{
  _commands = l;
}
