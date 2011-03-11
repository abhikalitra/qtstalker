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

#include "FI.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"

#include <QtDebug>

FI::FI ()
{
  _plugin = "FI";
  _maList << "SMA" << "EMA" << "WMA" << "DEMA" << "TEMA" << "TRIMA" << "KAMA";
}

int FI::command (Command *command)
{
  // PARMS:
  // INPUT_CLOSE
  // INPUT_VOLUME
  // NAME
  // PERIOD
  // MA_TYPE

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  Curve *iclose = i->line(command->parm("INPUT_CLOSE"));
  if (! iclose)
  {
    qDebug() << _plugin << "::command: invalid INPUT_CLOSE" << command->parm("INPUT_CLOSE");
    return 1;
  }

  Curve *ivol = i->line(command->parm("INPUT_VOLUME"));
  if (! ivol)
  {
    qDebug() << _plugin << "::command: invalid INPUT_VOLUME" << command->parm("INPUT_VOLUME");
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate name" << name;
    return 1;
  }

  bool ok;
  int period = command->parm("PERIOD").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid period" << command->parm("PERIOD");
    return 1;
  }

  int type = _maList.indexOf(command->parm("MA_TYPE"));
  if (type == -1)
  {
    qDebug() << _plugin << "::command: invalid ma type" << command->parm("MA_TYPE");
    return 1;
  }

  line = new Curve;

  int ipos = 0;
  int end = 0;
  iclose->keyRange(ipos, end);
  ipos++;
  double force = 0;
  for (; ipos <= end; ipos++)
  {
    CurveBar *cbar = iclose->bar(ipos);
    if (! cbar)
      continue;

    CurveBar *cybar = iclose->bar(ipos - 1);
    if (! cybar)
      continue;

    CurveBar *vbar = ivol->bar(ipos);
    if (! vbar)
      continue;

    double cdiff = cbar->data() - cybar->data();
    force = vbar->data() * cdiff;

    line->setBar(ipos, new CurveBar(force));
  }

  if (period > 1)
  {
    Curve *ma = getMA(line, period, type);
    if (! ma)
    {
      delete line;
      return 1;
    }

    delete line;
    line = ma;
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

Curve * FI::getMA (Curve *in, int period, int method)
{
  if (in->count() < period)
    return 0;

  QList<int> keys;
  in->keys(keys);

  int size = keys.count();

  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_MA(0, size - 1, &input[0], period, (TA_MAType) method, &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "FI::calculate: TA-Lib error" << rc;
    return 0;
  }

  Curve *line = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    line->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    keyLoop--;
    outLoop--;
  }

  return line;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  FI *o = new FI;
  return ((ScriptPlugin *) o);
}
