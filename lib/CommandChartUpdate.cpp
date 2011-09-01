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

#include <QtDebug>
#include <QHash>

CommandChartUpdate::CommandChartUpdate (QObject *p) : Command (p)
{
  _type = "CHART_UPDATE";
}

int CommandChartUpdate::runScript (Data *sg, Script *script)
{
  QString name = sg->get("CHART");
  QString date = sg->get("DATE");

  QList<QString> keys = script->dataKeys();

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *dg = script->data(keys.at(loop));

    if (dg->type() == "CURVE")
    {
      // send dates to chart
      if (keys.at(loop) == date)
      {
        // specify which chart to set
        dg->set(CurveData::_CHART, name);

        IPCMessage ipcm(script->session(), _type, "CHART_DATE", script->file(), dg->type());
        MessageSend ms(this);
        ms.send(ipcm, dg->toString());
        continue;
      }

      if (dg->getInteger(CurveData::_Z) < 0)
        continue;

      if (dg->get(CurveData::_CHART) != name)
        continue;

      QString type = dg->get(CurveData::_TYPE);

      IPCMessage ipcm(script->session(), _type, "CURVE", script->file(), dg->type());
      MessageSend ms(this);
      ms.send(ipcm, dg->toString());
      continue;
    }

    if (dg->type() == "CHART_OBJECT")
    {
      if (dg->getInteger(ChartObjectData::_Z) < 0)
        continue;

      if (dg->get(ChartObjectData::_CHART) != name)
        continue;

      QString type = dg->get(ChartObjectData::_TYPE);

      IPCMessage ipcm(script->session(), _type, "CHART_OBJECT", script->file(), dg->type());
      MessageSend ms(this);
      ms.send(ipcm, dg->toString());
    }
  }

  // send the update command
  IPCMessage ipcm(script->session(), _type, "UPDATE", script->file(), sg->type());

  MessageSend ms(this);
  ms.send(ipcm, sg->toString());

  return _OK;
}

Data * CommandChartUpdate::settings ()
{
  Data *sg = new Data;
  sg->set("CHART", QString());
  sg->set("DATE", QString("date"));
  return sg;
}
