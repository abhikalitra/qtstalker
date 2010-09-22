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

#include "FunctionNORMALIZE.h"
#include "Globals.h"

#include <QtDebug>
#include <cmath>

FunctionNORMALIZE::FunctionNORMALIZE ()
{
}

int FunctionNORMALIZE::script (QStringList &set, Indicator &ind)
{
  // INDICATOR,PLUGIN,NORMALIZE,<NAME>,<INPUT>
  //     0        1       2       3       4 

  if (set.count() != 5)
  {
    qDebug() << "FunctionNORMALIZE::script: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set.at(3));
  if (tl)
  {
    qDebug() << "FunctionNORMALIZE::script: duplicate name" << set.at(3);
    return 1;
  }

  Curve *in = ind.line(set.at(4));
  if (! in)
  {
    in = g_barData.getInput(g_barData.getInputType(set[4]));
    if (! in)
    {
      qDebug() << "FunctionNORMALIZE::script: input not found" << set.at(4);
      return 1;
    }

    ind.setLine(set.at(4), in);
  }

  Curve *line = calculate(in);
  if (! line)
    return 1;

  ind.setLine(set.at(3), line);

  return 0;
}

Curve * FunctionNORMALIZE::calculate (Curve *in)
{
  if (in->count() < 1)
    return 0;
  
  Curve *line = new Curve;
  double max = 0;
  double min = 0;
  QList<int> keys;
  in->keys(keys);
  in->highLowRange(keys.at(0), keys.count() - 1, max, min);

  double range = fabs(max) + fabs(min);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    line->setBar(keys.at(loop), new CurveBar(((bar->data() - min) / range) * 100));
  }

  return line;
}

