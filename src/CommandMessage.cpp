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

#include "CommandMessage.h"
#include "Globals.h"
#include "CommandFactory.h"
#include "CurveFactory.h"
#include "ChartObjectFactory.h"
#include "ChartObjectDataBase.h"
#include "DataFactory.h"
#include "CurveData.h"
#include "ChartObjectData.h"
#include "Symbol.h"

#include <QtDebug>
#include <QObject>

CommandMessage::CommandMessage ()
{
}

int CommandMessage::message (IPCMessage m, QString d)
{
  DataFactory dfac;
  Data *dg = dfac.data(m.dataType());
  if (! dg)
  {
    qDebug() << "CommandMessage::message: invalid data type" << m.dataType();
    return 1;
  }

  if (dg->fromString(d))
  {
    qDebug() << "CommandMessage::message: invalid data" << d;
    delete dg;
    return 1;
  }

  CommandFactory fac;

  switch ((CommandFactory::Type) fac.stringToType(m.command()))
  {
    case CommandFactory::_CHART:
      chart(m, dg);
      delete dg;
      break;
    case CommandFactory::_CHART_PANEL_REFRESH:
      g_chartPanel->updateList();
      delete dg;
      break;
    case CommandFactory::_CHART_UPDATE:
      chartUpdate(m, dg);
      break;
    case CommandFactory::_SCRIPT_DONE:
      g_scriptPanel->done(dg->get("SCRIPT").toString());
      delete dg;
      break;
    case CommandFactory::_SCRIPT_START:
      g_scriptPanel->runScript(dg->get("SCRIPT").toString());
      delete dg;
      break;
    case CommandFactory::_DEBUG:
      qDebug() << dg->get("MESSAGE").toString();
      delete dg;
      break;
    default:
      delete dg;
      return 1;
      break;
  }

  return 0;
}

int CommandMessage::chartUpdate (IPCMessage mess, Data *dg)
{
  if (mess.type() == "CHART_DATE")
  {
    Plot *plot = g_plotGroup->plot(dg->get(CurveData::_CHART).toString());
    if (! plot)
    {
      qDebug() << "CommandMessage::chartUpdate: chart not found" << dg->get(CurveData::_CHART).toString();
      delete dg;
      return 1;
    }

    plot->setDates(dg);

//qDebug() << "CommandMessage::chartUpdate: DATE curve OK" << dg->get(CurveData::_CHART);

    delete dg;

    return 0;
  }

  if (mess.type() == "CURVE")
  {
    QString type = dg->get(CurveData::_TYPE).toString();

    CurveFactory fac;
    Curve *curve = fac.curve(type);
    if (! curve)
    {
      qDebug() << "CommandMessage::chartUpdate: no curve type" << type;
      delete dg;
      return 1;
    }

    curve->setSettings(dg);

    Plot *plot = g_plotGroup->plot(dg->get(CurveData::_CHART).toString());
    if (! plot)
    {
      qDebug() << "CommandMessage::chartUpdate: chart not found" << dg->get(CurveData::_CHART).toString();
      delete dg;
      return 1;
    }

    plot->setCurve(curve);
//qDebug() << "CommandMessage::chartUpdate: curve OK" << dg->get(CurveData::_LABEL) << dg->barKeyCount();
    return 0;
  }

  if (mess.type() == "CHART_OBJECT")
  {
    QString type = dg->get(ChartObjectData::_TYPE).toString();

    ChartObjectFactory fac;
    ChartObject *co = fac.chartObject(type);
    if (! co)
    {
      qDebug() << "CommandMessage::chartUpdate: no chart object type" << type;
      delete dg;
      return 1;
    }

    co->setSettings(dg);
//    co->setPlotName(sg.get("CHART")->getString());
//    co->setReadOnly(TRUE);

    Plot *plot = g_plotGroup->plot(dg->get(ChartObjectData::_CHART).toString());
    if (! plot)
    {
      qDebug() << "CommandMessage::chartUpdate: chart not found" << dg->get(ChartObjectData::_CHART).toString();
      delete co;
      delete dg;
      return 1;
    }

    plot->addChartObject(co);

    return 0;
  }

  if (mess.type() == "UPDATE")
  {
    QString chart = dg->get("CHART").toString();

    Plot *plot = g_plotGroup->plot(chart);
    if (! plot)
    {
      qDebug() << "CommandMessage::chartUpdate: chart not found" << chart;
      delete dg;
      return 1;
    }

    // load chart objects from database
    QString symbol = g_currentSymbol->get(Symbol::_EXCHANGE).toString() + ":" + g_currentSymbol->get(Symbol::_SYMBOL).toString();
    ChartObjectDataBase codb;
    QHash<QString, Data *> chartObjects;
    if (! codb.load(chart, symbol, chartObjects))
    {
      ChartObjectFactory fac;
      QHashIterator<QString, Data *> it(chartObjects);
      while (it.hasNext())
      {
        it.next();
        Data *dg = it.value();
        ChartObject *co = fac.chartObject(dg->get(ChartObjectData::_TYPE).toString());
        if (! co)
          continue;

        co->setSettings(dg);

        plot->addChartObject(co);
      }
    }

    plot->updatePlot();

    delete dg;
  }

  return 0;
}

int CommandMessage::chart (IPCMessage mess, Data *dg)
{
  QString chart = dg->get("NAME").toString();
  if (chart.isEmpty())
  {
    qDebug() << "CommandMessage::chart: invalid NAME" << chart;
    return 1;
  }

  Plot *plot = g_plotGroup->plot(chart);
  if (! plot)
  {
    plot = new Plot(chart, g_plotGroup);
    plot->setBarSpacing(g_controlPanel->barSpaceButton()->getPixelSpace());
    plot->loadSettings();
    plot->setScriptFile(mess.scriptFile());
    plot->setRow(dg->get("ROW").toInt());
    plot->setCol(dg->get("COL").toInt());

    QObject::connect(plot, SIGNAL(signalInfoMessage(Message)), g_infoPanel, SLOT(showInfo(Message)));
    QObject::connect(g_controlPanel->barSpaceButton(), SIGNAL(signalPixelSpace(int)), plot, SLOT(setBarSpacing(int)));
    QObject::connect(g_controlPanel, SIGNAL(signalValueChanged(int)), plot, SLOT(setStartIndex(int)));
    QObject::connect(plot, SIGNAL(signalIndex(int)), g_controlPanel, SLOT(setStartValue(int)));

    g_plotGroup->setPlot(plot);
  }

  plot->clear();

  plot->showDate(dg->get("DATE").toBool());

  plot->setLogScaling(dg->get("LOG").toBool());

  plot->update();

  return 0;
}
