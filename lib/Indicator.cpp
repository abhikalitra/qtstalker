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
  _settings = new IndicatorSettings;
  init();
}

Indicator::Indicator ()
{
  _settings = new IndicatorSettings;
  init();
}

Indicator::~Indicator ()
{
  delete _settings;
}

void Indicator::init ()
{
  _settings->setData("LOCK", 0);
  _settings->setData("DATE_AXIS", 1);
  _settings->setData("LOG_SCALING", 0);
}

void Indicator::setName (QString d)
{
  _settings->setData("NAME", d);
}

QString Indicator::name ()
{
  return _settings->data("NAME");
}

void Indicator::setLock (bool d)
{
  _settings->setData("LOCK", d);
}

bool Indicator::lock ()
{
  bool d = _settings->getInt("LOCK");
  return d;
}

void Indicator::setDate (bool d)
{
  _settings->setData("DATE_AXIS", d);
}

bool Indicator::date ()
{
  bool d = _settings->getInt("DATE_AXIS");
  return d;
}

void Indicator::setLog (bool d)
{
  _settings->setData("LOG_SCALING", d);
}

bool Indicator::log ()
{
  bool d = _settings->getInt("LOG_SCALING");
  return d;
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
  if (_settings->save())
  {
    qDebug() << "Indicator::save: error saving indicator";
    return 1;
  }

  return 0;
}

int Indicator::load ()
{
  return _settings->load(name());
}

void Indicator::calculate ()
{
  int loop = 0;
  for (; loop < _settings->count(); loop++)
  {
    Setting *set = _settings->settings(loop);
    
    PluginFactory fac;
    Plugin *plug = fac.plugin(set->data("PLUGIN"));
    if (! plug)
    {
      qDebug() << "Indicator::calculate: no plugin";
      continue;
    }

    if (plug->calculate(g_barData, this, set))
      qDebug() << "Indicator::calculate: plugin error";

    delete plug;
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
  l << _settings->data("NAME") << g_barData->exchange() << g_barData->symbol();
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

IndicatorSettings * Indicator::settings ()
{
  return _settings;
}

void Indicator::dialog ()
{
  IndicatorEditDialog *dialog = new IndicatorEditDialog(g_parent, _settings);
  connect(dialog, SIGNAL(signalDone()), this, SLOT(dialogDone()));
  dialog->show();
}

void Indicator::dialogDone ()
{
//  save();
  emit signalClear();
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
