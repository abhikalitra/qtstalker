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

#include "CommandSAR.h"
#include "ta_libc.h"
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

CommandSAR::CommandSAR (QObject *p) : Command (p)
{
  _type = "SAR";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandSAR::CommandSAR: error on TA_Initialize");
}

int CommandSAR::runScript (Data *sg, Script *script)
{
  QString name = sg->get("OUTPUT");
  Data *line = script->data(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT" << name;
    return _ERROR;
  }

  QString s = sg->get("HIGH");
  Data *ihigh = script->data(s);
  if (! ihigh)
  {
    qDebug() << _type << "::runScript: invalid HIGH" << s;
    return _ERROR;
  }

  s = sg->get("LOW");
  Data *ilow = script->data(s);
  if (! ilow)
  {
    qDebug() << _type << "::runScript: invalid LOW" << s;
    return _ERROR;
  }

  double init = sg->getDouble("STEP_INITIAL");

  double max = sg->getDouble("STEP_MAX");

  QList<Data *> list;
  list << ihigh << ilow;
  line = getSAR(list, init, max);
  if (! line)
    return _ERROR;

  script->setData(name, line);

  return _OK;
}

Data * CommandSAR::getSAR (QList<Data *> &list, double init, double max)
{
  if (list.count() != 2)
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real out[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &high[0], &low[0], &low[0], &low[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_SAR(0,
                         size - 1,
                         &high[0],
                         &low[0],
			 init,
			 max,
                         &outBeg,
                         &outNb,
                         &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getSAR: TA-Lib error" << rc;
    return 0;
  }

  QList<Data *> outs;
  Data *c = new CurveData;
  outs.append(c);
  if (it.outputs(outs, keys, outNb, &out[0], &out[0], &out[0]))
  {
    delete c;
    return 0;
  }

  return c;
}

Data * CommandSAR::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT", QString());
  sg->set("HIGH", QString());
  sg->set("LOW", QString());
  sg->set("STEP_INITIAL", 0.02);
  sg->set("STEP_MAX", 0.2);
  return sg;
}
