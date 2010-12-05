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

/* The "Market Thermometer" _indicator is described in
   Dr. Alexander Elder's book _Come Into My Trading Room_, p.162 */

/* Alert Notes
1) enter when therm falls below MA
2) exit when threshold is triggered
3) explosive move expected when therm stays below MA for 5-7 days
*/


#include "THERM.h"
#include "Curve.h"

#include <QtDebug>
#include <cmath>

THERM::THERM ()
{
}

int THERM::command (Command &command)
{
  // THERM,<NAME>
  //   0     1

  if (command.count() != 2)
  {
    qDebug() << "THERM::command: invalid parm count" << command.count();
    return 1;
  }

  BarData *data = command.barData();
  if (! data)
  {
    qDebug() << "THERM::command: no bars";
    return 1;
  }

  Indicator *i = command.indicator();
  if (! i)
  {
    qDebug() << "THERM::command: no indicator";
    return 1;
  }

  QString name = command.parm(1);

  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << "THERM::command: duplicate name" << name;
    return 1;
  }

  if (data->count() < 2)
    return 1;

  line = new Curve;

  int loop = 1;
  double thermometer = 0;
  for (; loop < (int) data->count(); loop++)
  {
    Bar *bar = data->bar(loop);
    Bar *pbar = data->bar(loop - 1);
    double high = fabs(bar->high() - pbar->high());
    double lo = fabs(pbar->low() - bar->low());

    if (high > lo)
      thermometer = high;
    else
      thermometer = lo;

    line->setBar(loop, new CurveBar(thermometer));
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
  THERM *o = new THERM;
  return ((ScriptPlugin *) o);
}
