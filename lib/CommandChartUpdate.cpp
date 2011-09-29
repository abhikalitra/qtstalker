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

#include <QtDebug>
#include <QTime>
#include <QHash>

CommandChartUpdate::CommandChartUpdate (QObject *p) : Command (p)
{
  _name = "CHART_UPDATE";
}

int CommandChartUpdate::runScript (Message *sg, Script *script)
{
  Data *d = new Data;
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
  d->set("CHART", name);

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
  d->set("DATE", date);

  QList<QString> keys = script->dataKeys();

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *dg = script->data(keys.at(loop));

    if (dg->type() == DataFactory::_CURVE)
    {
      if (keys.at(loop) == date->toString())
        dateCurve(script, dg, name->toString());
      else
        curve(script, dg, name->toString());

      continue;
    }

    if (dg->type() == DataFactory::_CHART_OBJECT)
      chartObject(script, dg, name->toString());
  }

  // send the update command
  d->setCommand(_name);
  d->setCommandType("UPDATE");
  d->setScriptFile(script->file());
  emit signalMessage(d);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}

void CommandChartUpdate::dateCurve (Script *script, Data *dg, QString name)
{
  dg->set(CurveData::_CHART, new SettingString(name));
  dg->setCommand(_name);
  dg->setCommandType("CHART_DATE");
  dg->setScriptFile(script->file());
  emit signalMessage(dg);
}

void CommandChartUpdate::curve (Script *script, Data *dg, QString name)
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
  dg->setCommand(_name);
  dg->setCommandType("CURVE");
  dg->setScriptFile(script->file());
  emit signalMessage(dg);
}

void CommandChartUpdate::chartObject (Script *script, Data *dg, QString name)
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
  dg->setCommand(_name);
  dg->setCommandType("CHART_OBJECT");
  dg->setScriptFile(script->file());
  emit signalMessage(dg);
}
