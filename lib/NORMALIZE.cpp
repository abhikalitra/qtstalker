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
#include "PlotLineBar.h"
#include "PlotFactory.h"

#include <QtDebug>
#include <cmath>

NORMALIZE::NORMALIZE ()
{
  indicator = "NORMALIZE";
  deleteFlag = TRUE;
}

int NORMALIZE::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,NORMALIZE,<NAME>,<INPUT>,<LINE_TYPE>,<COLOR>
  //     0        1       2       3       4         5         6

  if (set.count() != 7)
  {
    qDebug() << indicator << "::getCUS: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS: duplicate name" << set[3];
    return 1;
  }

  PlotLine *in = tlines.value(set[4]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[4]));
    if (! in)
    {
      qDebug() << indicator << "::getCUS: input not found" << set[4];
      return 1;
    }

    tlines.insert(set[4], in);
  }

  PlotFactory fac;
  QStringList pl;
  fac.list(pl, 1);
  int lineType = pl.indexOf(set[5]);
  if (lineType == -1)
  {
    qDebug() << indicator << "::getCUS: invalid plot type" << set[5];
    return 1;
  }

  QColor color(set[6]);
  if (! color.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid color" << set[6];
    return 1;
  }
  
  PlotLine *line = normalize(in, lineType, color);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * NORMALIZE::normalize (PlotLine *in, int lineType, QColor &color)
{
  if (in->count() < 1)
    return 0;
  
  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
    return 0;

  double max = in->high();
  double min = in->low();

  double range = fabs(max) + fabs(min);

  QList<int> keys;
  in->keys(keys);
  
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar *bar = in->data(keys.at(loop));
    line->setData(keys.at(loop), new PlotLineBar(color, ((bar->data() - min) / range) * 100));
  }

  return line;
}

