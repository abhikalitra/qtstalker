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

#include "AD.h"
#include "Curve.h"
#include "ta_libc.h"
#include "Globals.h"

#include <QtDebug>

AD::AD ()
{
}

int AD::command (Command *command)
{
  // AD,<NAME>
  // 0    1

  if (command->count() != 2)
  {
    qDebug() << "AD::command: invalid parm count" << command->count();
    return 1;
  }

  BarData *data = g_barData;
  if (! data)
  {
    qDebug() << "AD::command: no bars";
    return 1;
  }

  if (data->count() < 1)
    return 1;

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "AD::command: no indicator";
    return 1;
  }

  QString name = command->parm(1);
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << "AD::command: duplicate name" << name;
    return 1;
  }

  int size = data->count();
  
  TA_Real out[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real volume[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < data->count(); loop++)
  {
    Bar *bar = data->bar(loop);
    high[loop] = (TA_Real) bar->high();
    low[loop] = (TA_Real) bar->low();
    close[loop] = (TA_Real) bar->close();
    volume[loop] = (TA_Real) bar->volume();
  }

  TA_RetCode rc = TA_AD(0,
                        data->count() - 1,
                        &high[0],
                        &low[0],
                        &close[0],
                        &volume[0],
                        &outBeg,
                        &outNb,
                        &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "AD::command: TA-Lib error" << rc;
    return 1;
  }

  line = new Curve;

  int dataLoop = data->count() - 1;
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
  AD *o = new AD;
  return ((ScriptPlugin *) o);
}
