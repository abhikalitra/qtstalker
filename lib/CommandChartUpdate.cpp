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

#include "CommandChartUpdate.h"
#include "CurveData.h"
#include "ChartObjectData.h"
#include "DataFactory.h"
#include "CurveFactory.h"
#include "ChartObjectFactory.h"
#include "ChartObjectDataBase.h"
#include "GlobalPlotGroup.h"
#include "GlobalSymbol.h"
#include "DataString.h"
#include "ScriptVerifyCurve.h"

#include <QtDebug>
#include <QTime>
#include <QHash>

CommandChartUpdate::CommandChartUpdate (QObject *p) : Command (p)
{
  _name = "CHART_UPDATE";
  _type = _WAIT;

  _values.insert(_ParmTypeChart, new DataString());
  _values.insert(_ParmTypeDate, new DataString());
}

void CommandChartUpdate::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandChartUpdate::runScript: parse error");
    return;
  }

  if (chartUpdate(script,
	          _values.value(_ParmTypeChart)->toString(),
	          _values.value(_ParmTypeDate)->toString()))
  {
    Command::error(QString());
    return;
  }

  Command::done(QString());
}

int CommandChartUpdate::chartUpdate (Script *script, QString name, QString ds)
{
  Plot *plot = g_plotGroup->plot(name);
  if (! plot)
  {
    qDebug() << "CommandChartUpdate::chartUpdate: chart not found" << name;
    return 1;
  }

  int offset = 0;
  ScriptVerifyCurve svc;
  Data *date = svc.toCurve(script, ds, offset);
  if (! date)
  {
    qDebug() << "CommandChartUpdate::chartUpdate: date not found" << ds;
    return 1;
  }

  plot->clear();
  plot->setDates(date);

  QList<QString> keys = script->dataKeys();

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *dg = script->data(keys.at(loop));

    switch ((DataFactory::Type) dg->type())
    {
      case DataFactory::_CURVE:
        curve(dg, name, plot);
        break;
      case DataFactory::_CHART_OBJECT:
        chartObject(dg, name, plot);
        break;
      default:
        break;
    }
  }

  update(name, plot);

  return 0;
}

void CommandChartUpdate::curve (Data *dg, QString name, Plot *plot)
{
  Data *setting = dg->toData(CurveData::_Z);
  if (! setting)
    return;

  if (setting->toInteger() < 0)
    return;

  setting = dg->toData(CurveData::_CHART);
  if (! setting)
    return;

  if (setting->toString() != name)
    return;

  dg->setDeleteFlag(0);

  QString type = dg->toData(CurveData::_TYPE)->toString();

  CurveFactory fac;
  Curve *curve = fac.curve(type);
  if (! curve)
  {
    qDebug() << "CommandChartUpdate::curve: no curve type" << type;
    return;
  }

  curve->setSettings(dg);

  plot->setCurve(curve);
}

void CommandChartUpdate::chartObject (Data *dg, QString name, Plot *plot)
{
  Data *setting = dg->toData(ChartObjectData::_Z);
  if (! setting)
    return;

  if (setting->toInteger() < 0)
    return;

  setting = dg->toData(ChartObjectData::_CHART);
  if (! setting)
    return;

  if (setting->toString() != name)
    return;

  dg->setDeleteFlag(0);

  QString type = dg->toData(ChartObjectData::_TYPE)->toString();

  ChartObjectFactory fac;
  ChartObject *co = fac.chartObject(type);
  if (! co)
  {
    qDebug() << "CommandChartUpdate::chartObject: no chart object type" << type;
    return;
  }

  co->setSettings(dg);

  plot->addChartObject(co);
}

void CommandChartUpdate::update (QString chart, Plot *plot)
{
  // load chart objects from database
  QString symbol = g_currentSymbol->exchange() + ":" + g_currentSymbol->symbol();

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
      ChartObject *co = fac.chartObject(tdg->toData(ChartObjectData::_TYPE)->toString());
      if (! co)
        continue;

      co->setSettings(tdg);

      plot->addChartObject(co);
    }
  }

  plot->updatePlot();
}
