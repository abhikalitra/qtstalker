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

#include <QtDebug>

FI::FI ()
{
}

int FI::command (Command &command)
{
  // FI,<NAME>,<PERIOD>,<MA_TYPE>
  //  0    1      2         3

  if (command.count() != 4)
  {
    qDebug() << "FI::command: invalid settings count" << command.count();
    return 1;
  }

  BarData *data = command.barData();
  if (! data)
  {
    qDebug() << "FI::command: no bars";
    return 1;
  }

  if (data->count() < 1)
    return 1;

  Indicator *i = command.indicator();
  if (! i)
  {
    qDebug() << "FI::command: no indicator";
    return 1;
  }

  int pos = 1;
  QString name = command.parm(pos);

  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << "FI::command: duplicate name" << name;
    return 1;
  }

  pos++;
  bool ok;
  int period = command.parm(pos).toInt(&ok);
  if (! ok)
  {
    qDebug() << "FI::command: invalid period" << command.parm(pos);
    return 1;
  }

  pos++;
  FunctionMA fma;
  int type = fma.typeFromString(command.parm(pos));
  if (type == -1)
  {
    qDebug() << "FI::command: invalid ma type" << command.parm(pos);
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

  command.setReturnData("0");

  emit signalDone();

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
