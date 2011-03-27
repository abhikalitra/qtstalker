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
#include "IndicatorDataBase.h"
#include "Globals.h"
#include "ChartObjectDataBase.h"
#include "PluginFactory.h"

#include <QDebug>

Indicator::Indicator (QObject *p) : QObject (p)
{
//  _thread = 0;
  _settings = new Setting;
  init();
}

Indicator::Indicator ()
{
//  _thread = 0;
  _settings = new Setting;
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
  deleteLine(k);
  _lines.insert(k, d);
}

void Indicator::setLine (int k, Curve *d)
{
  QString s = QString::number(k);
  setLine(s, d);
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
  QList<QString> keys = _lines.keys();
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Curve *curve = _lines.value(keys.at(loop));
    if (! curve)
      continue;
    
    if (curve->z() < 0)
    {
      delete curve;
      _lines.remove(keys.at(loop));
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
  IndicatorDataBase db;
  return db.save(this);
}

int Indicator::load ()
{
  IndicatorDataBase db;
  return db.load(this);
}

void Indicator::calculate ()
{
//  IndicatorThread *thread = new IndicatorThread(this, g_barData, this);
//  connect(thread, SIGNAL(finished()), this, SLOT(calculate2()), Qt::QueuedConnection);
//  thread->start();
  
  PluginFactory fac;
  Plugin *plug = fac.plugin(_settings->data("PLUGIN"));
  if (! plug)
  {
    qDebug() << "Indicator::calculate: no plugin";
    return;
  }

  if (plug->calculate(g_barData, this))
  {
    qDebug() << "Indicator::calculate: plugin error";
    delete plug;
    return;
  }

  weedPlots();

  loadChartObjects();

  delete plug;
  
  emit signalPlot();
}

/*
void Indicator::calculate2 ()
{
//  delete _thread;
//  _thread = 0;
  
  weedPlots();
  loadChartObjects();
  emit signalPlot();
}
*/

void Indicator::loadChartObjects ()
{
  QList<Setting> l;
  ChartObjectDataBase db;
  db.load(_settings->data("NAME"), g_barData, l);

  int loop = 0;
  for (; loop < l.count(); loop++)
    addChartObject(l.at(loop));
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

Setting * Indicator::settings ()
{
  return _settings;
}

void Indicator::dialog ()
{
  PluginFactory fac;
  Plugin *plug = fac.plugin(_settings->data("PLUGIN"));
  if (! plug)
  {
    qDebug() << "Indicator::dialog: no plugin" << _settings->data("PLUGIN");
    return;
  }

  plug->dialog(0, this);
  delete plug;
}

void Indicator::dialogDone ()
{
  save();
  emit signalClear();
  calculate();
}
