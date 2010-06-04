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

int Indicator::log ()
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

void Indicator::setLine (QString &k, PlotLine *d)
{
  PlotLine *l = line(k);
  if (l)
  {
    delete l;
    _lines.remove(k);
  }
  
  _lines.insert(k, d);
}

PlotLine * Indicator::line (QString &k)
{
  return _lines.value(k);
}

int Indicator::deleteLine (QString &k)
{
  PlotLine *line = _lines.value(k);
  if (! line)
    return 1;

  delete line;
  _lines.remove(k);

  return 0;
}

void Indicator::setSettings (Setting &d)
{
  _settings = d;
}

Setting & Indicator::settings ()
{
  return _settings;
}

void Indicator::addPlotOrder (QString &d)
{
  _plotOrder.append(d);
}

void Indicator::clear ()
{
  init();

  qDeleteAll(_lines);
  _lines.clear();

  clearChartObjects();
}

COPlugin * Indicator::chartObject (int k)
{
  return _chartObjects.value(k);
}

void Indicator::addChartObject (COPlugin *d)
{
  _chartObjects.insert(d->getID(), d);
}

void Indicator::clearChartObjects ()
{
  if (! _cus)
  {
    qDeleteAll(_chartObjects);
    _chartObjects.clear();
  }
}

void Indicator::deleteChartObject (int d)
{
  COPlugin *co = _chartObjects.value(d);
  if (! co)
    return;
  
  delete co;
  _chartObjects.remove(d);
}

QStringList & Indicator::plotOrder ()
{
  return _plotOrder;
}

void Indicator::weedPlots ()
{
  QHashIterator<QString, PlotLine *> it(_lines);
  while (it.hasNext())
  {
    it.next();
    if (_plotOrder.indexOf(it.key()) == -1)
    {
      delete it.value();
      _lines.remove(it.key());
    }
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
  _name.clear();
  _indicator.clear();
  _settings.clear();
  _plotOrder.clear();
}

void Indicator::coKeys (QList<int> &l)
{
  l.clear();
  
  QHashIterator<int, COPlugin *> it(_chartObjects);
  while (it.hasNext())
  {
    it.next();
    l.append(it.key());
  }
}

