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

#include "NORMALIZE.h"

#include <QtDebug>
#include <cmath>

NORMALIZE::NORMALIZE ()
{
  indicator = "NORMALIZE";
  deleteFlag = TRUE;
}

int NORMALIZE::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,NORMALIZE,METHOD,<NAME>,<INPUT>
  //     0        1    2      3      4       5

  if (set.count() != 6)
  {
    qDebug() << indicator << "::NORMALIZE: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::NORMALIZE: duplicate name" << set[4];
    return 1;
  }

  PlotLine *in = tlines.value(set[5]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[5]));
    if (! in)
    {
      qDebug() << indicator << "::NORMALIZE: input not found" << set[5];
      return 1;
    }

    tlines.insert(set[5], in);
  }

  PlotLine *line = getNORMALIZE(in);
  if (! line)
    return 1;

  tlines.insert(set[4], line);

  return 0;
}

PlotLine * NORMALIZE::getNORMALIZE (PlotLine *in)
{
  if (in->count() < 1)
    return 0;
  
  PlotLine *line = new PlotLine;
  int loop = 0;
  int size = in->count();
  double range = 0;
  double max = -99999999.0;
  double min = 99999999.0;
  
  for (loop = 0; loop < size; loop++)
  {
    if (in->getData(loop) > max)
      max = in->getData(loop);
    if (in->getData(loop) < min)
      min = in->getData(loop);
  }
  
  range = fabs(max) + fabs(min);
  
  for (loop = 0; loop < in->count(); loop++)
    line->append(((in->getData(loop) - min) / range) * 100);

  return line;
}

