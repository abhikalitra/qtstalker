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
#include "Globals.h"
#include "Script.h"
#include "IPCMessage.h"
#include "CurveFactory.h"
#include "MessageSend.h"
#include "ChartObjectDataBase.h"
#include "ChartObjectFactory.h"
#include "SettingString.h"

#include <QtDebug>
#include <QHash>

CommandChartUpdate::CommandChartUpdate (QObject *p) : Command (p)
{
  _type = "CHART_UPDATE";
}

int CommandChartUpdate::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  QString key = sg->get("CHART")->getString();
  QString name = script->setting(key)->getString();

  QHashIterator<QString, Curve *> it(script->curves());
  while (it.hasNext())
  {
    it.next();
    Curve *curve = it.value();
    if (curve->z() == -1)
      continue;

    if (curve->plotName() != name)
      continue;

    IPCMessage ipcm(script->session(), _type, "CURVE", script->file());

    MessageSend ms(this);
    ms.send(ipcm, curve->toString());
  }

  // set any chart objects
  QHashIterator<QString, ChartObject *> it2(script->chartObjects());
  while (it2.hasNext())
  {
    it2.next();
    ChartObject *co = it2.value();

    if (co->plotName() != name)
      continue;

    if (co->z() == -1)
      continue;

    QStringList data;
    data << "TYPE=" + co->type();
    data << "CHART=" + co->plotName();
    data << co->settings()->toString();

    IPCMessage ipcm(script->session(), _type, "CHART_OBJECT", script->file());
    MessageSend ms(this);
    ms.send(ipcm, data.join(","));
  }

  // send the update command
  IPCMessage ipcm(script->session(), _type, "UPDATE", script->file());

  QStringList data;
  data << name;

  MessageSend ms(this);
  ms.send(ipcm, data);

  return _OK;
}

int CommandChartUpdate::message (IPCMessage &mess, QString &d)
{
  if (mess.type() == "CURVE")
  {
    int pos = d.indexOf(";");
    QString type = d.left(pos);

    CurveFactory fac;
    Curve *curve = fac.curve(type);
    if (! curve)
    {
      qDebug() << "CommandChartUpdate::message: no curve type" << type;
      return _ERROR;
    }

    if (curve->fromString(d))
    {
      delete curve;
      qDebug() << _type << "::message: invalid curve data";
      return _ERROR;
    }

    Plot *plot = g_plotGroup->plot(curve->plotName());
    if (! plot)
    {
      qDebug() << _type << "::message: chart not found" << curve->plotName();
      return 1;
    }

    plot->setCurve(curve);
  }

  if (mess.type() == "CHART_OBJECT")
  {
    QStringList l = d.split(",");
    SettingGroup sg;
    if (sg.parse(l))
    {
      qDebug() << _type << "::message: invalid chart object data" << d;
      return _ERROR;
    }

    QString type = sg.get("TYPE")->getString();

    ChartObjectFactory fac;
    ChartObject *co = fac.chartObject(type);
    if (! co)
    {
      qDebug() << "CommandChartUpdate::message: no chart object type" << type;
      return _ERROR;
    }

    SettingGroup *settings = co->settings();
    settings->merge(&sg);

    co->setPlotName(sg.get("CHART")->getString());
    co->setReadOnly(TRUE);

    Plot *plot = g_plotGroup->plot(co->plotName());
    if (! plot)
    {
      qDebug() << _type << "::message: chart not found" << co->plotName();
      delete co;
      return 1;
    }

    plot->addChartObject(co);
  }

  if (mess.type() == "UPDATE")
  {
    Plot *plot = g_plotGroup->plot(d);
    if (! plot)
    {
      qDebug() << _type << "::message: chart not found" << d;
      return _ERROR;
    }

    // load chart objects from database
    ChartObjectDataBase codb;
    QHash<QString, ChartObject *> chartObjects;
    if (! codb.load(d, g_currentSymbol.key(), chartObjects))
    {
      QHashIterator<QString, ChartObject *> it(chartObjects);
      while (it.hasNext())
      {
        it.next();
        plot->addChartObject(it.value());
      }
    }

    plot->updatePlot();
  }

  return _OK;
}

SettingGroup * CommandChartUpdate::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_CHART, Setting::_NONE, QString());
  ss->setKey("CHART");
  sg->set(ss);

  return sg;
}
