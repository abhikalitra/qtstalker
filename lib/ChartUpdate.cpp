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

#include "ChartUpdate.h"
#include "CurveFactory.h"
#include "ChartObjectFactory.h"
#include "GlobalPlotGroup.h"
#include "GlobalSymbol.h"
#include "KeyCurveData.h"
#include "KeyChartObject.h"
#include "EAVDataBase.h"
#include "EAVSearch.h"
#include "KeySymbol.h"

#include <QtDebug>

ChartUpdate::ChartUpdate ()
{
}

int ChartUpdate::date (Entity &e)
{
  Data name;
  e.toData(QString("CHART"), name);
  
  Plot *plot = g_plotGroup->plot(name.toString());
  if (! plot)
  {
    qDebug() << "ChartUpdate::chartDate: chart not found" << name.toString();
    return 1;
  }

  plot->clear();
  plot->setDates(e);
  
  return 0;
}

int ChartUpdate::curve (Entity &e)
{
  Data name;
  e.toData(QString("CHART"), name);
  
  Plot *plot = g_plotGroup->plot(name.toString());
  if (! plot)
  {
    qDebug() << "ChartUpdate::chartCurve: chart not found" << name.toString();
    return 1;
  }

  KeyCurveData keys;
  Data setting;
  e.toData(keys.indexToString(KeyCurveData::_TYPE), setting);

  CurveFactory fac;
  Curve *curve = fac.curve(setting.toString());
  if (! curve)
  {
    qDebug() << "ChartUpdate::chartCurve: no curve type" << setting.toString();
    return 1;
  }

  curve->setSettings(e);
  plot->setCurve(curve);
  
  return 0;
}

int ChartUpdate::object (Entity &e)
{
  Data name;
  e.toData(QString("CHART"), name);
  
  Plot *plot = g_plotGroup->plot(name.toString());
  if (! plot)
  {
    qDebug() << "ChartUpdate::chartObject: chart not found" << name.toString();
    return 1;
  }

  KeyChartObject keys;
  Data setting;
  e.toData(keys.indexToString(KeyChartObject::_TYPE), setting);

  ChartObjectFactory fac;
  ChartObject *co = fac.chartObject(setting.toString());
  if (! co)
  {
    qDebug() << "ChartUpdate::chartObject: no chart object type" << setting.toString();
    return 1;
  }

  co->setSettings(e);
  plot->addChartObject(co);
  
  return 0;
}

int ChartUpdate::loadObjects (Entity &e)
{
  Data name;
  e.toData(QString("CHART"), name);
  
  Plot *plot = g_plotGroup->plot(name.toString());
  if (! plot)
  {
    qDebug() << "ChartUpdate::loadObjects: chart not found" << name.toString();
    return 1;
  }
  
  // load chart objects from database
  KeySymbol skeys;
  Data symbol;
  g_currentSymbol.toData(skeys.indexToString(KeySymbol::_SYMBOL), symbol);

  KeyChartObject cokeys;
  EAVSearch request;
  request.append(cokeys.indexToString(KeyChartObject::_CHART), "=", name.toString());
  request.append(cokeys.indexToString(KeyChartObject::_SYMBOL), "=", symbol.toString());

  EAVDataBase codb("chartObjects");
  QStringList names;
  if (codb.search(request, names))
    return 0;

  int loop = 0;
  for (; loop < names.size(); loop++)
  {
    Entity osettings;
    osettings.setName(names.at(loop));
    if (codb.get(osettings))
      continue;

    Data type;
    osettings.toData(cokeys.indexToString(KeyChartObject::_TYPE), type);
      
    ChartObjectFactory fac;
    ChartObject *co = fac.chartObject(type.toString());
    if (! co)
      continue;

    Entity nsettings = co->settings();
    nsettings.merge(osettings);
    co->setSettings(nsettings);
    
    plot->addChartObject(co);
  }
  
  return 0;
}

int ChartUpdate::update (Entity &e)
{
  Data name;
  e.toData(QString("CHART"), name);
  
  Plot *plot = g_plotGroup->plot(name.toString());
  if (! plot)
  {
    qDebug() << "ChartUpdate::chartUpdate: chart not found" << name.toString();
    return 1;
  }

  plot->updatePlot();
  
  return 0;
}
