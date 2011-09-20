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
#include "IPCMessage.h"
#include "MessageSend.h"
#include "CurveData.h"
#include "ChartObjectData.h"
#include "SettingString.h"
#include "DataFactory.h"

#include <QtDebug>
#include <QHash>

CommandChartUpdate::CommandChartUpdate (QObject *p) : Command (p)
{
  _type = "CHART_UPDATE";
}

int CommandChartUpdate::runScript (Message *sg, Script *script)
{
  Data d;
  SettingString *name = new SettingString(QString("Chart"));
  QString s = sg->value("CHART");
  if (name->set(s, (void *) script))
  {
    if (name->set(s))
    {
      _message << "invalid CHART " + s;
      return _ERROR;
    }
  }
  d.set("CHART", name);

  SettingString *date = new SettingString(QString("date"));
  s = sg->value("DATE");
  if (date->set(s, (void *) script))
  {
    if (date->set(s))
    {
      _message << "invalid DATE " + s;
      return _ERROR;
    }
  }
  d.set("DATE", date);

  QList<QString> keys = script->dataKeys();

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *dg = script->data(keys.at(loop));

    if (dg->type() == DataFactory::_CURVE)
    {
      // send dates to chart
      if (keys.at(loop) == date->toString())
      {
        // specify which chart to set
        dg->set(CurveData::_CHART, new SettingString(name->toString()));

        IPCMessage ipcm(script->session(), _type, "CHART_DATE", script->file(), QString::number(dg->type()));
        MessageSend ms(this);
        ms.send(ipcm, dg->toString());
        continue;
      }

      Setting *setting = dg->get(CurveData::_Z);
      if (! setting)
        continue;
      if (setting->toInteger() < 0)
        continue;

      setting = dg->get(CurveData::_CHART);
      if (! setting)
        continue;
      if (setting->toString() != name->toString())
        continue;

      IPCMessage ipcm(script->session(), _type, "CURVE", script->file(), QString::number(dg->type()));
      MessageSend ms(this);
      ms.send(ipcm, dg->toString());
      continue;
    }

    if (dg->type() == DataFactory::_CHART_OBJECT)
    {
      Setting *setting = dg->get(CurveData::_Z);
      if (! setting)
        continue;
      if (setting->toInteger() < 0)
        continue;

      setting = dg->get(CurveData::_CHART);
      if (! setting)
        continue;
      if (setting->toString() != name->toString())
        continue;

      IPCMessage ipcm(script->session(), _type, "CHART_OBJECT", script->file(), QString::number(dg->type()));
      MessageSend ms(this);
      ms.send(ipcm, dg->toString());
    }
  }

  // send the update command
  IPCMessage ipcm(script->session(), _type, "UPDATE", script->file(), QString::number(d.type()));

  MessageSend ms(this);
  ms.send(ipcm, d.toString());

  return _OK;
}
