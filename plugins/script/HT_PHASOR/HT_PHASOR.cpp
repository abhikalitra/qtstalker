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

#include "HT_PHASOR.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"

#include <QtDebug>

HT_PHASOR::HT_PHASOR ()
{
}

int HT_PHASOR::command (Command *command)
{
  // HT_PHASOR,<INPUT>,<PHASE NAME>,<QUAD NAME>
  //     0        1          2           3

  if (command->count() != 4)
  {
    qDebug() << "HT_PHASOR::command: invalid settings count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "HT_PHASOR::command: no indicator";
    return 1;
  }

  int pos = 1;
  Curve *in = i->line(command->parm(pos));
  if (! in)
  {
    qDebug() << "HT_PHASOR::command: input missing" << command->parm(pos);
    return 1;
  }

  pos++;  
  QString pname = command->parm(pos);
  Curve *line = i->line(pname);
  if (line)
  {
    qDebug() << "HT_PHASOR::command: duplicate phase name" << pname;
    return 1;
  }

  pos++;
  QString qname = command->parm(pos);
  line = i->line(qname);
  if (line)
  {
    qDebug() << "HT_PHASOR::command: duplicate quad name" << qname;
    return 1;
  }

  if (in->count() < 1)
    return 1;

  int size = in->count();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];

  QList<int> keys;
  in->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_HT_PHASOR (0,
                                size - 1,
                                &input[0],
                                &outBeg,
                                &outNb,
                                &out[0],
                                &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << "HT_PHASOR::command: TA-Lib error" << rc;
    return 1;
  }

  Curve *pline = new Curve;
  Curve *qline = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    pline->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    qline->setBar(keys.at(keyLoop), new CurveBar(out2[outLoop]));

    keyLoop--;
    outLoop--;
  }

  pline->setLabel(pname);
  qline->setLabel(qname);

  i->setLine(pname, pline);
  i->setLine(qname, qline);

  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  HT_PHASOR *o = new HT_PHASOR;
  return ((ScriptPlugin *) o);
}
