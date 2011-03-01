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
#include "Script.h"
#include "ChartObjectDataBase.h"

#include <QDebug>

Indicator::Indicator (QObject *p) : QObject (p)
{
  init();
}

Indicator::Indicator ()
{
  init();
}

void Indicator::init ()
{
  _lock = FALSE;
  _date = TRUE;
  _log = FALSE;
}

void Indicator::setName (QString d)
{
  _name = d;
}

QString & Indicator::name ()
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

void Indicator::setScript (QString d)
{
  _script = d;
}

QString & Indicator::script ()
{
  return _script;
}

void Indicator::setCommand (QString d)
{
  _command = d;
}

QString & Indicator::command ()
{
  return _command;
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
  init();
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
  Script *script = new Script(this);
  script->setName(_name);
  script->setFile(_script);
  script->setCommand(_command);
  connect(script, SIGNAL(signalDone(QString)), this, SLOT(scriptFinished()));
  script->setBarData(g_barData);
  script->setIndicator(this);
  script->startScript();
}

void Indicator::scriptFinished ()
{
  weedPlots();

  loadChartObjects();
  
  emit signalPlot();
}

void Indicator::loadChartObjects ()
{
  QList<Setting> l;
  ChartObjectDataBase db;
  db.load(_name, g_barData, l);

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
