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
#include "Globals.h"
#include "CurveFactory.h"
#include "ChartObjectFactory.h"
#include "ChartObjectDataBase.h"
#include "CurveData.h"
#include "ChartObjectData.h"
#include "Symbol.h"
#include "DataFactory.h"

#include <QtDebug>
#include <QObject>
#include <QString>
#include <QTcpSocket>

ChartUpdate::ChartUpdate ()
{
}

int ChartUpdate::run (IPCMessage mess, Data *dg)
{
  if (mess.type() == "CHART_DATE")
  {
    if (dateCurve(dg))
    {
      delete dg;
      return 1;
    }

    delete dg;
    return 0;
  }

  if (mess.type() == "CURVE")
  {
    if (curve(dg))
    {
      delete dg;
      return 1;
    }

    return 0;
  }

  if (mess.type() == "CHART_OBJECT")
  {
    if (chartObject(dg))
    {
      delete dg;
      return 1;
    }

    return 0;
  }

  if (mess.type() == "UPDATE")
  {
    if (update(dg))
    {
      delete dg;
      return 1;
    }

    delete dg;
    return 0;
  }

  return 0;
}

int ChartUpdate::dateCurve (Data *dg)
{
  Plot *plot = g_plotGroup->plot(dg->get(CurveData::_CHART)->toString());
  if (! plot)
  {
    qDebug() << "ChartUpdate::dateCurve: chart not found" << dg->get(CurveData::_CHART)->toString();
    return 1;
  }

  plot->setDates(dg);

  return 0;
}

int ChartUpdate::curve (Data *dg)
{
  QString type = dg->get(CurveData::_TYPE)->toString();

  CurveFactory fac;
  Curve *curve = fac.curve(type);
  if (! curve)
  {
    qDebug() << "ChartUpdate::chartUpdate: no curve type" << type;
    return 1;
  }

  curve->setSettings(dg);

  Plot *plot = g_plotGroup->plot(dg->get(CurveData::_CHART)->toString());
  if (! plot)
  {
    qDebug() << "ChartUpdate::chartUpdate: chart not found" << dg->get(CurveData::_CHART)->toString();
    return 1;
  }

  plot->setCurve(curve);

  return 0;
}

int ChartUpdate::chartObject (Data *dg)
{
  QString type = dg->get(ChartObjectData::_TYPE)->toString();

  ChartObjectFactory fac;
  ChartObject *co = fac.chartObject(type);
  if (! co)
  {
    qDebug() << "ChartUpdate::chartObject: no chart object type" << type;
    return 1;
  }

  co->setSettings(dg);

  Plot *plot = g_plotGroup->plot(dg->get(ChartObjectData::_CHART)->toString());
  if (! plot)
  {
    qDebug() << "ChartUpdate::chartObject: chart not found" << dg->get(ChartObjectData::_CHART)->toString();
    delete co;
    return 1;
  }

  plot->addChartObject(co);

  return 0;
}

int ChartUpdate::update (Data *dg)
{
  QString chart = dg->get("CHART")->toString();

  Plot *plot = g_plotGroup->plot(chart);
  if (! plot)
  {
    qDebug() << "ChartUpdate::update: chart not found" << chart;
    return 1;
  }

  // load chart objects from database
  QString symbol = g_currentSymbol->get(Symbol::_EXCHANGE)->toString() + ":" + g_currentSymbol->get(Symbol::_SYMBOL)->toString();

  ChartObjectDataBase codb;
  QHash<QString, Data *> chartObjects;
  if (! codb.load(chart, symbol, chartObjects))
  {
    ChartObjectFactory fac;
    QHashIterator<QString, Data *> it(chartObjects);
    while (it.hasNext())
    {
      it.next();
      Data *tdg = it.value();
      ChartObject *co = fac.chartObject(tdg->get(ChartObjectData::_TYPE)->toString());
      if (! co)
        continue;

      co->setSettings(tdg);

      plot->addChartObject(co);
    }
  }

  plot->updatePlot();

  return 0;
}
