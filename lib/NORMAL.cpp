/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "NORMAL.h"
#include "math.h" // for fabs

#include <QtDebug>


NORMAL::NORMAL ()
{
}

int NORMAL::calculate (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,NORMAL,<NAME>,<INPUT>

  if (set.count() != 4)
  {
    qDebug() << "NORMAL::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << set[1] << "::calculate: duplicate name" << set[2];
    return 1;
  }

  PlotLine *in = tlines.value(set[3]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[3]));
    if (! in)
    {
      qDebug() << set[1] << "::calculate: input not found" << set[3];
      return 1;
    }

    tlines.insert(set[3], in);
  }

  int loop = 0;
  double range = 0;
  double max = -99999999.0;
  double min = 99999999.0;
  double norm = 0;
  for (loop = 0; loop < in->getSize(); loop++)
  {
    if (in->getData(loop) > max)
      max = in->getData(loop);

    if (in->getData(loop) < min)
      min = in->getData(loop);
  }

  range = fabs(max) + fabs(min);

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < in->getSize(); loop++)
  {
    norm = ((in->getData(loop) - min) / range) * 100;
    line->append(norm);
  }

  tlines.insert(set[2], line);

  return 0;
}

