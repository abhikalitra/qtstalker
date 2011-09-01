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

#include "CommandLINEARREG.h"
#include "ta_libc.h"
#include "InputType.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

CommandLINEARREG::CommandLINEARREG (QObject *p) : Command (p)
{
  _type = "LINEARREG";
  _method << "LINEARREG" << "ANGLE" << "INTERCEPT" "SLOPE" << "TSF";

  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("CommandLINEARREG::CommandLINEARREG: error on TA_Initialize");
}

int CommandLINEARREG::runScript (Data *sg, Script *script)
{
  QString name = sg->get("OUPUT");
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

  s = sg->get("METHOD");
  int method = _method.indexOf(s);
  if (method == -1)
  {
    qDebug() << _type << "::runScript: invalid METHOD" << s;
    return _ERROR;
  }

  int period = sg->getInteger("PERIOD");

  QList<Data *> list;
  list << in;
  line = getLR(list, period, method);
  if (! line)
    return _ERROR;

  script->setData(name, line);

  return _OK;
}

Data * CommandLINEARREG::getLR (QList<Data *> &list, int period, int method)
{
  if (! list.count())
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  int size = keys.count();
  TA_Real out[size];
  TA_Real input[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  size = it.fill(list, keys, &input[0], &input[0], &input[0], &input[0]);
  if (! size)
    return 0;

  TA_RetCode rc = TA_SUCCESS;
  switch ((Method) method)
  {
    case _LINEARREG:
      rc = TA_LINEARREG(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _ANGLE:
      rc = TA_LINEARREG_ANGLE(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _INTERCEPT:
      rc = TA_LINEARREG_INTERCEPT(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _SLOPE:
      rc = TA_LINEARREG_SLOPE(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _TSF:
      rc = TA_TSF(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << _type << "::getLR: TA-Lib error" << rc;
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

Data * CommandLINEARREG::settings ()
{
  Data *sg = new Data;
  sg->set("OUTPUT", QString());
  sg->set("INPUT", QString());
  sg->set("PERIOD", 14);
  sg->set("METHOD", _method.at(0));
  return sg;
}
