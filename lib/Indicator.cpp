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

#include <QDateTime>
#include <cmath>

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

Scaler & Indicator::scaler ()
{
  return _scaler;
}

void Indicator::setScale (PlotData &pd, DateBar &dateBars)
{
  double tscaleHigh = -99999999;
  double tscaleLow = 99999999;

  int loop = 0;
  for (; loop < _plotOrder.count(); loop++)
  {
    PlotLine *tline = line(_plotOrder[loop]);

    double h, l;
    if (tline->highLowRange(pd.startIndex, pd.endIndex, h, l))
      continue;

    if (h > tscaleHigh)
      tscaleHigh = h;
    if (l < tscaleLow)
      tscaleLow = l;
  }

  QDateTime sd, ed;
  dateBars.getDate(pd.startIndex, sd);
  dateBars.getDate(pd.endIndex, ed);

  QList<int> keyList;
  coKeys(keyList);
  
  for (loop = 0; loop < keyList.count(); loop++)
  {
    COPlugin *co = chartObject(keyList.at(loop));

    if (! co->inDateRange(sd, ed, dateBars))
      continue;

    double h, l;
    if (co->getHighLow(h, l))
      continue;

    if (h > tscaleHigh)
      tscaleHigh = h;

    if (l < tscaleLow)
      tscaleLow = l;
  }

  // create a little more room between chart edges and plots
  double t = (tscaleHigh - tscaleLow) * 0.02; // get 2% of the range
  tscaleHigh += t;
  if (tscaleLow != 0)
    tscaleLow -= t;

  // handle log scaling if toggled
  double tlogScaleHigh = 1;
  double tlogRange = 0;
  if (getLog())
  {
    tlogScaleHigh = tscaleHigh > 0.0 ? log(tscaleHigh) : 1;
    double tlogScaleLow = tscaleLow > 0.0 ? log(tscaleLow) : 0;
    tlogRange = tlogScaleHigh - tlogScaleLow;
  }

  int height = pd.buffer.height();
  if (date())
    height -= pd.dateHeight;

  _scaler.set(height,
              tscaleHigh,
              tscaleLow,
              tlogScaleHigh,
              tlogRange,
              getLog());
}

