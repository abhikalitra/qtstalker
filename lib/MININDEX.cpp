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

#include "MININDEX.h"

#include <QtDebug>

MININDEX::MININDEX ()
{
  indicator = "MININDEX";
  deleteFlag = TRUE;
}

int MININDEX::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,MININDEX,<NAME>,<INPUT>,<PERIOD>
  //     0       1     2    3    4       5       6

  if (set.count() != 6)
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

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid period" << set[5];
    return 1;
  }

  PlotLine *line = getMININDEX(in, period);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * MININDEX::getMININDEX (PlotLine *in, int period)
{
  if (in->count() < period)
    return 0;

  PlotLine *line = new PlotLine;
  int loop = period - 1;
  for (; loop < in->count(); loop++)
  {
    int count = 1;
    double min = in->getData(loop);
    int pos = 0;
    for (; count < period; count++)
    {
      if (in->getData(loop - count) < min)
      {
        pos = count;
	min = in->getData(loop - count);
      }
    }

    line->append(pos);
  }

  return line;
}

