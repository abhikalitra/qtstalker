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

#include <QDebug>

Indicator::Indicator ()
{
  clear();
}

void Indicator::setName (QString &d)
{
  _name = d;
}

QString & Indicator::name ()
{
  return _name;
}

void Indicator::setEnable (int d)
{
  _enable = d;
}

int Indicator::enable ()
{
  return _enable;
}

void Indicator::setTabRow (int d)
{
  _tabRow = d;
}

int Indicator::tabRow ()
{
  return _tabRow;
}

void Indicator::setColumn (int d)
{
  _column = d;
}

int Indicator::column ()
{
  return _column;
}

void Indicator::setDate (int d)
{
  _date = d;
}

int Indicator::date ()
{
  return _date;
}

void Indicator::setCUS (int d)
{
  _cus = d;
}

int Indicator::CUS ()
{
  return _cus;
}

void Indicator::setLog (int d)
{
  _log = d;
}

int Indicator::getLog ()
{
  return _log;
}

void Indicator::setIndicator (QString &d)
{
  _indicator = d;
}

QString & Indicator::indicator ()
{
  return _indicator;
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

void Indicator::setSettings (Setting &d)
{
  _settings = d;
}

Setting & Indicator::settings ()
{
  return _settings;
}

void Indicator::clear ()
{
  init();

  qDeleteAll(_lines);
  _lines.clear();

  clearChartObjects();
}

ChartObjectSettings Indicator::chartObject (int k)
{
  return _chartObjects.value(k);
}

void Indicator::addChartObject (int id, ChartObjectSettings &co)
{
  _chartObjects.insert(id, co);
}

void Indicator::clearChartObjects ()
{
  _chartObjects.clear();
}

void Indicator::deleteChartObject (int d)
{
  _chartObjects.remove(d);
}

void Indicator::weedPlots ()
{
  QHashIterator<QString, Curve *> it(_lines);
  while (it.hasNext())
  {
    it.next();
    Curve *curve = it.value();
    
    if (curve->z() == -1)
      deleteLine(it.key());
  }
}

void Indicator::cleanClear ()
{
  _lines.clear();
  _chartObjects.clear();
  init();
}

void Indicator::init ()
{
  _enable = 0;
  _tabRow = 1;
  _date = 1;
  _log = 0;
  _cus = 0;
  _column = 1;
//  _name.clear();
  _indicator.clear();
  _settings.clear();
}

void Indicator::coKeys (QList<int> &l)
{
  l = _chartObjects.keys();
}

int Indicator::coCount ()
{
  return _chartObjects.count();
}

void Indicator::clearLines ()
{
  if (_lines.count())
  {
    qDeleteAll(_lines);
    _lines.clear();
  }
}

