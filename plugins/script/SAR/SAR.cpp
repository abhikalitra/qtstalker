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

#include "SAR.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"

#include <QtDebug>

SAR::SAR ()
{
}

int SAR::command (Command *command)
{
  // SAR,<NAME>,<INITIAL_STEP>,<MAX_STEP>
  //  0     1          2           3

  if (command->count() != 4)
  {
    qDebug() << "SAR::command: invalid settings count" << command->count();
    return 1;
  }

  BarData *data = g_barData;
  if (! data)
  {
    qDebug() << "SAR::command: no bars";
    return 1;
  }

  if (data->count() < 1)
    return 1;

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "SAR::command: no indicator";
    return 1;
  }

  int pos = 1;
  QString name = command->parm(pos);
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << "SAR::command: duplicate name" << name;
    return 1;
  }

  pos++;
  bool ok;
  double init = command->parm(pos).toDouble(&ok);
  if (! ok)
  {
    qDebug() << "SAR::command: initial step" << command->parm(pos);
    return 1;
  }

  pos++;
  double max = command->parm(pos).toDouble(&ok);
  if (! ok)
  {
    qDebug() << "SAR::command: max step" << command->parm(pos);
    return 1;
  }

  int size = data->count();
  TA_Real out[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    Bar *bar = data->bar(loop);
    high[loop] = (TA_Real) bar->high();
    low[loop] = (TA_Real) bar->low();
  }

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
    qDebug() << "SAR::command: TA-Lib error" << rc;
    return 1;
  }

  line = new Curve(Curve::Dot);

  for (loop = 0; loop < outNb; loop++)
    line->setBar(loop + 1, new CurveBar(out[loop]));

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
  SAR *o = new SAR;
  return ((ScriptPlugin *) o);
}
