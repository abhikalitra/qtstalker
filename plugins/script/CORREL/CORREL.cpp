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

#include "CORREL.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"

#include <QtDebug>

CORREL::CORREL ()
{
}

int CORREL::command (Command *command)
{
  // CORREL,<NAME>,<INPUT_1>,<INPUT_2>,<PERIOD>
  //    0     1        2         3        4

  if (command->count() != 5)
  {
    qDebug() << "CORREL::command: invalid settings count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "CORREL::command: no indicator";
    return 1;
  }

  int pos = 1;
  QString name = command->parm(pos);
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << "CORREL::command: duplicate name" << name;
    return 1;
  }

  pos++;
  Curve *in = i->line(command->parm(pos));
  if (! in)
  {
    qDebug() << "CORREL::command: input missing" << command->parm(pos);
    return 1;
  }

  pos++;
  Curve *in2 = i->line(command->parm(pos));
  if (! in2)
  {
    qDebug() << "CORREL::command: input 2 missing" << command->parm(pos);
    return 1;
  }

  pos++;
  bool ok;
  int period = command->parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "CORREL::command: invalid period" << command->parm(pos);
    return 1;
  }

  if (in->count() < period || in2->count() < period)
    return 1;

  QList<int> keys;
  int size = in->count();
  if (in2->count() < size)
  {
    size = in2->count();
    in2->keys(keys);
  }
  else
    in->keys(keys);

  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    if (! bar)
      continue;

    CurveBar *bar2 = in2->bar(keys.at(loop));
    if (! bar2)
      continue;

    input[loop] = (TA_Real) bar->data();
    input2[loop] = (TA_Real) bar2->data();
  }

  TA_RetCode rc = TA_CORREL(0,
                            size - 1,
                            &input[0],
                            &input2[0],
                            period,
                            &outBeg,
                            &outNb,
                            &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << "CORREL::command: TA-Lib error" << rc;
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

  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  CORREL *o = new CORREL;
  return ((ScriptPlugin *) o);
}
