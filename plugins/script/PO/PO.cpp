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

#include "PO.h"
#include "Curve.h"
#include "ta_libc.h"
#include "FunctionMA.h"

#include <QtDebug>

PO::PO ()
{
  _method << "APO" << "PPO";
}

int PO::command (Command &command)
{
  // PO,<METHOD>,<NAME>,<INPUT>,<FAST_PERIOD>,<SLOW_PERIOD>,<MA_TYPE>
  //  0    1       2       3         4              5          6

  if (command.count() != 7)
  {
    qDebug() << "PO::command: invalid settings count" << command.count();
    return 1;
  }

  Indicator *i = command.indicator();
  if (! i)
  {
    qDebug() << "PO::command: no indicator";
    return 1;
  }

  int pos = 1;
  int method = _method.indexOf(command.parm(pos));
  if (method == -1)
  {
    qDebug() << "PO::command: invalid method" << command.parm(pos);
    return 1;
  }

  pos++;
  QString name = command.parm(pos);
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << "PO::command: duplicate name" << name;
    return 1;
  }

  pos++;
  Curve *in = i->line(command.parm(pos));
  if (! in)
  {
    qDebug() << "PO::command: input missing" << command.parm(pos);
    return 1;
  }

  pos++;
  bool ok;
  int fast = command.parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "PO::command: invalid fast period" << command.parm(pos);
    return 1;
  }

  pos++;
  int slow = command.parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "PO::command: invalid slow period" << command.parm(pos);
    return 1;
  }

  pos++;
  FunctionMA fma;
  int type = fma.typeFromString(command.parm(pos));
  if (type == -1)
  {
    qDebug() << "PO::command: invalid ma type" << command.parm(pos);
    return 1;
  }

  if (in->count() < fast || in->count() < slow)
    return 1;

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

  TA_RetCode rc = TA_SUCCESS;

  switch ((Method) method)
  {
    case _APO:
      rc = TA_APO(0, size - 1, &input[0], fast, slow, (TA_MAType) type, &outBeg, &outNb, &out[0]);
      break;
    case _PPO:
      rc = TA_PPO(0, size - 1, &input[0], fast, slow, (TA_MAType) type, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << "PO::command: TA-Lib error" << rc;
    return 1;
  }

  line = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    line->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    keyLoop--;
    outLoop--;
  }

  line->setLabel(name);

  i->setLine(name, line);

  command.setReturnData("0");

  emit signalDone();

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  PO *o = new PO;
  return ((ScriptPlugin *) o);
}
