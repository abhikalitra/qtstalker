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

#include <QtDebug>
#include <cmath>

FunctionNORMALIZE::FunctionNORMALIZE ()
{
}

int FunctionNORMALIZE::script (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,NORMALIZE,<NAME>,<INPUT>
  //     0        1       2       3       4 

  if (set.count() != 5)
  {
    qDebug() << "FunctionNORMALIZE::script: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "FunctionNORMALIZE::script: duplicate name" << set[3];
    return 1;
  }

  PlotLine *in = ind.line(set[4]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[4]));
    if (! in)
    {
      qDebug() << "FunctionNORMALIZE::script: input not found" << set[4];
      return 1;
    }

    ind.setLine(set[4], in);
  }

  PlotLine *line = calculate(in);
  if (! line)
    return 1;

  ind.setLine(set[3], line);

  return 0;
}

PlotLine * FunctionNORMALIZE::calculate (PlotLine *in)
{
  if (in->count() < 1)
    return 0;
  
  PlotLine *line = new PlotLine;
  double max = 0;
  double min = 0;
  QList<int> keys;
  in->keys(keys);
  in->highLowRange(keys.at(0), keys.count() - 1, max, min);

  double range = fabs(max) + fabs(min);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar *bar = in->data(keys.at(loop));
    line->setData(keys.at(loop), ((bar->data() - min) / range) * 100);
  }

  return line;
}

