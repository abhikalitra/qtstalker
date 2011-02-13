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
#include "FunctionMA.h"
#include "Globals.h"

#include <QtDebug>

FI::FI ()
{
  _plugin = "FI";
}

int FI::command (Command *command)
{
  // PARMS:
  // NAME
  // PERIOD
  // MA_TYPE

  BarData *data = g_barData;
  if (! data)
  {
    qDebug() << _plugin << "::command: no bars";
    return 1;
  }

  if (data->count() < 1)
    return 1;

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
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

  FunctionMA fma;
  int type = fma.typeFromString(command->parm("MA_TYPE"));
  if (type == -1)
  {
    qDebug() << _plugin << "::command: invalid ma type" << command->parm("MA_TYPE");
    return 1;
  }

  if (data->count() < period)
    return 0;

  line = new Curve;

  int loop = 1;
  double force = 0;
  for (; loop < data->count(); loop++)
  {
    Bar *bar = data->bar(loop);
    Bar *pbar = data->bar(loop - 1);
    double cdiff = bar->close() - pbar->close();
    force = bar->volume() * cdiff;

    line->setBar(loop, new CurveBar(force));
  }

  if (period > 1)
  {
    Curve *ma = fma.calculate(line, period, type);
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

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  FI *o = new FI;
  return ((ScriptPlugin *) o);
}
