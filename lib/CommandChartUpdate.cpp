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
#include "SettingString.h"
#include "SettingInteger.h"
#include "DataFactory.h"
#include "CurveFactory.h"
#include "ChartObjectFactory.h"
#include "ChartObjectDataBase.h"
#include "GlobalPlotGroup.h"
#include "GlobalSymbol.h"

#include <QtDebug>
#include <QTime>
#include <QHash>

CommandChartUpdate::CommandChartUpdate (QObject *p) : Command (p)
{
  _name = "CHART_UPDATE";
  _type = _NORMAL;
}

int CommandChartUpdate::runScript (Message *sg, Script *script)
{
  SettingString *name = new SettingString(QString("Chart"));
  QString s = sg->value("CHART");
  if (name->set(s, (void *) script))
  {
    if (name->set(s))
    {
      qDebug() << "CommandChartUpdate::runScript: invalid CHART" << s;
      emit signalResume((void *) this);
      return _ERROR;
    }
  }

  SettingString *date = new SettingString(QString("date"));
  s = sg->value("DATE");
  if (date->set(s, (void *) script))
  {
    if (date->set(s))
    {
      qDebug() << "CommandChartUpdate::runScript: invalid DATE" << s;
      emit signalResume((void *) this);
      return _ERROR;
    }
  }

  Plot *plot = g_plotGroup->plot(name->toString());
  if (! plot)
  {
    qDebug() << "CommandChartUpdate::runScript: chart not found" << name->toString();
    emit signalResume((void *) this);
    return _ERROR;
  }

  plot->clear();

  QList<QString> keys = script->dataKeys();

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *dg = script->data(keys.at(loop));

    if (dg->type() == DataFactory::_CURVE)
    {
      if (keys.at(loop) == date->toString())
      {
        plot->setDates(dg);
      }
      else
      {
        curve(dg, name->toString(), plot);
      }

      continue;
    }

    if (dg->type() == DataFactory::_CHART_OBJECT)
    {
      chartObject(dg, name->toString(), plot);
    }
  }

  update(name->toString(), plot);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}

void CommandChartUpdate::curve (Data *dg, QString name, Plot *plot)
{
  Setting *setting = dg->get(CurveData::_Z);
  if (! setting)
    return;

  if (setting->toInteger() < 0)
    return;

  setting = dg->get(CurveData::_CHART);
  if (! setting)
    return;

  if (setting->toString() != name)
    return;

  dg->setDeleteFlag(0);

  QString type = dg->get(CurveData::_TYPE)->toString();

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
  Setting *setting = dg->get(ChartObjectData::_Z);
  if (! setting)
    return;

  if (setting->toInteger() < 0)
    return;

  setting = dg->get(ChartObjectData::_CHART);
  if (! setting)
    return;

  if (setting->toString() != name)
    return;

  dg->setDeleteFlag(0);

  QString type = dg->get(ChartObjectData::_TYPE)->toString();

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
      ChartObject *co = fac.chartObject(tdg->get(ChartObjectData::_TYPE)->toString());
      if (! co)
        continue;

      co->setSettings(tdg);

      plot->addChartObject(co);
    }
  }

  plot->updatePlot();
}
