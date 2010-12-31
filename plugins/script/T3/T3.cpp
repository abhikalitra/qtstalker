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

#include "T3.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"

#include <QtDebug>

T3::T3 ()
{
}

int T3::command (Command *command)
{
  // T3,<NAME>,<INPUT>,<PERIOD>,<VFACTOR>
  //  0     1      2       3        4

  if (command->count() != 5)
  {
    qDebug() << "T3::command: invalid settings count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "T3::command: no indicator";
    return 1;
  }

  int pos = 1;
  QString name = command->parm(pos);
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << "T3::command: duplicate name" << name;
    return 1;
  }

  pos++;
  Curve *in = i->line(command->parm(pos));
  if (! in)
  {
    qDebug() << "T3::command: input missing" << command->parm(pos);
    return 1;
  }

  pos++;
  bool ok;
  int period = command->parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "T3::command: invalid period" << command->parm(pos);
    return 1;
  }

  pos++;
  double vfactor = command->parm(pos).toDouble(&ok);
  if (! ok)
  {
    qDebug() << "T3::command: invalid vfactor" << command->parm(pos);
    return 1;
  }

  if (in->count() < period)
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

  TA_RetCode rc = TA_T3(0,
                        size - 1,
                        &input[0],
                        period,
                        vfactor,
                        &outBeg,
                        &outNb,
                        &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << "T3::command: TA-Lib error" << rc;
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
  T3 *o = new T3;
  return ((ScriptPlugin *) o);
}
