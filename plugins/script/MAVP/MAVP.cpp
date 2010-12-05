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

#include "MAVP.h"
#include "Curve.h"
#include "ta_libc.h"
#include "FunctionMA.h"

#include <QtDebug>

MAVP::MAVP ()
{
}

int MAVP::command (Command &command)
{
  // MAVP,<NAME>,<INPUT_1>,<INPUT_2>,<MIN_PERIOD>,<MAX_PERIOD>,<MA_TYPE>
  //  0      1       2         3         4             5           6

  if (command.count() != 7)
  {
    qDebug() << "MAVP::command: invalid settings count" << command.count();
    return 1;
  }

  Indicator *i = command.indicator();
  if (! i)
  {
    qDebug() << "MAVP::command: no indicator";
    return 1;
  }

  int pos = 1;
  QString name = command.parm(pos);
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << "MAVP::command: duplicate name" << name;
    return 1;
  }

  pos++;
  Curve *in = i->line(command.parm(pos));
  if (! in)
  {
    qDebug() << "MAVP::command: input missing" << command.parm(pos);
    return 1;
  }

  pos++;
  Curve *in2 = i->line(command.parm(pos));
  if (in2)
  {
    qDebug() << "MAVP::command: input 2 missing" << command.parm(pos);
    return 1;
  }

  pos++;
  bool ok;
  int min = command.parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "MAVP::command: invalid min period" << command.parm(pos);
    return 1;
  }

  pos++;
  int max = command.parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "MAVP::command: invalid max period" << command.parm(pos);
    return 1;
  }

  pos++;
  FunctionMA fma;
  int type = fma.typeFromString(command.parm(pos));
  if (type == -1)
  {
    qDebug() << "MAVP::command: invalid ma type" << command.parm(pos);
    return 1;
  }

  if (in->count() < min || in->count() < max)
    return 1;

  int flag = 0;
  int size = in->count();
  if (in2->count() < size)
  {
    size = in2->count();
    flag = 1;
  }

  QList<int> keys;
  in->keys(keys);
  QList<int> keys2;
  in2->keys(keys2);

  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = keys.count() - 1;
  int loop2 = keys2.count() - 1;
  while (loop > -1 && loop2 > -1)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    CurveBar *bar2 = in2->bar(keys2.at(loop2));
    input[loop] = (TA_Real) bar->data();
    input2[loop2] = (TA_Real) bar2->data();

    loop--;
    loop2--;
  }

  TA_RetCode rc = TA_MAVP(0,
                          size - 1,
                          &input[0],
                          &input2[0],
                          min,
                          max,
                          (TA_MAType) type,
                          &outBeg,
                          &outNb,
                          &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << "MAVP::command: TA-Lib error" << rc;
    return 1;
  }

  line = new Curve;

  if (! flag)
  {
    int keyLoop = keys.count() - 1;
    int outLoop = outNb - 1;
    while (keyLoop > -1 && outLoop > -1)
    {
      line->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
      keyLoop--;
      outLoop--;
    }
  }
  else
  {
    int keyLoop = keys2.count() - 1;
    int outLoop = outNb - 1;
    while (keyLoop > -1 && outLoop > -1)
    {
      line->setBar(keys2.at(keyLoop), new CurveBar(out[outLoop]));
      keyLoop--;
      outLoop--;
    }
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
  MAVP *o = new MAVP;
  return ((ScriptPlugin *) o);
}
