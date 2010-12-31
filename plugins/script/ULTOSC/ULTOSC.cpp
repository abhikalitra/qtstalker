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

#include "ULTOSC.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"

#include <QtDebug>

ULTOSC::ULTOSC ()
{
}

int ULTOSC::command (Command *command)
{
  // ULTOSC,<NAME>,<SHORT PERIOD>,<MED PERIOD>,<LONG PERIOD>
  //   0       1         2             3            4

  if (command->count() != 5)
  {
    qDebug() << "ULTOSC::command: invalid settings count" << command->count();
    return 1;
  }

  BarData *data = g_barData;
  if (! data)
  {
    qDebug() << "ULTOSC::command: no bars";
    return 1;
  }

  if (data->count() < 1)
    return 1;

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "ULTOSC::command: no indicator";
    return 1;
  }

  int pos = 1;
  QString name = command->parm(pos);

  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << "ULTOSC::command: duplicate name" << name;
    return 1;
  }

  pos++;
  bool ok;
  int sp = command->parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "ULTOSC::command: invalid short period" << command->parm(pos);
    return 1;
  }

  pos++;
  int mp = command->parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "ULTOSC::command: invalid medium period" << command->parm(pos);
    return 1;
  }

  pos++;
  int lp = command->parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "ULTOSC::command: invalid long period" << command->parm(pos);
    return 1;
  }

  int size = data->count();

  if (size < sp || size < mp || size < lp)
    return 1;

  TA_Real out[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    Bar *bar = data->bar(loop);
    high[loop] = (TA_Real) bar->high();
    low[loop] = (TA_Real) bar->low();
    close[loop] = (TA_Real) bar->close();
  }

  TA_RetCode rc = TA_ULTOSC(0,
                            size - 1,
                            &high[0],
                            &low[0],
                            &close[0],
                            sp,
                            mp,
                            lp,
                            &outBeg,
                            &outNb,
                            &out[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << "ULTOSC::command: TA-Lib error" << rc;
    return 1;
  }

  line = new Curve;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    line->setBar(dataLoop, new CurveBar(out[outLoop]));
    dataLoop--;
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
  ULTOSC *o = new ULTOSC;
  return ((ScriptPlugin *) o);
}
