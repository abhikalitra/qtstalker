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

#include "CommandBETA.h"
#include "ta_libc.h"
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

CommandBETA::CommandBETA (QObject *p) : Command (p)
{
  _type = "BETA";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandBETA::CommandBETA: error on TA_Initialize");
}

int CommandBETA::runScript (Data *sg, Script *script)
{
  QString name = sg->get("OUTPUT");
  Data *line = script->data(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate OUTPUT" << name;
    return _ERROR;
  }

  QString s = sg->get("INPUT");
  Data *in = script->data(s);
  if (! in)
  {
    qDebug() << _type << "::runScript: INPUT missing" << s;
    return _ERROR;
  }

  s = sg->get("INDEX");
  Data *in2 = script->data(s);
  if (! in2)
  {
    qDebug() << _type << "::runScript: INDEX missing" << s;
    return _ERROR;
  }

  int period = sg->getInteger("PERIOD");

  QList<Data *> list;
  list << in << in2;
  line = getBETA(list, period);
  if (! line)
    return _ERROR;

  script->setData(name, line);

  return _OK;
}

Data * CommandBETA::getBETA (QList<Data *> &list, int period)
{
  if (list.count() != 2)
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &input[0], &input2[0], &input2[0], &input2[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_BETA(0,
                          size - 1,
                          &input[0],
                          &input2[0],
                          period,
                          &outBeg,
                          &outNb,
                          &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getBETA: TA-Lib error" << rc;
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

Data * CommandBETA::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT", QString());
  sg->set("INPUT", QString());
  sg->set("INDEX", QString());
  sg->set("PERIOD", 5);
  return sg;
}
