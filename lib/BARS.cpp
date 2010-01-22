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

#include "BARS.h"

#include <QtDebug>


BARS::BARS ()
{
}

int BARS::calculate (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,BARS,<NAME>,<BAR_UP_COLOR>,<BAR_DOWN_COLOR>,<BAR_NEUTRAL_COLOR>

  if (set.count() != 6)
  {
    qDebug() << "BARS::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << set[1] << "::calculate: duplicate name" << set[2];
    return 1;
  }

  QColor barUpColor(set[3]);
  if (! barUpColor.isValid())
  {
    qDebug() << "BARS::calculate: invalid bar up color" << set[3];
    return 1;
  }

  QColor barDownColor(set[4]);
  if (! barDownColor.isValid())
  {
    qDebug() << "BARS::calculate: invalid bar down color" << set[4];
    return 1;
  }

  QColor barNeutralColor(set[5]);
  if (! barNeutralColor.isValid())
  {
    qDebug() << "BARS::calculate: invalid bar neutral color" << set[5];
    return 1;
  }

  int size = data->count();

  PlotLine *line = new PlotLine;
  int loop;
  for (loop = 0; loop < size; loop++)
  {
    if (loop > 0)
    {
      if (data->getClose(loop) > data->getClose(loop - 1))
        line->append(barUpColor, data->getOpen(loop), data->getHigh(loop), data->getLow(loop), data->getClose(loop), FALSE);
      else
      {
        if (data->getClose(loop) < data->getClose(loop - 1))
          line->append(barDownColor, data->getOpen(loop), data->getHigh(loop), data->getLow(loop), data->getClose(loop), FALSE);
        else
          line->append(barNeutralColor, data->getOpen(loop), data->getHigh(loop), data->getLow(loop), data->getClose(loop), FALSE);
      }
    }
    else
      line->append(barNeutralColor, data->getOpen(loop), data->getHigh(loop), data->getLow(loop), data->getClose(loop), FALSE);

//    QDateTime dt;
//    data->getDate(loop, dt);
//    line->append(dt);
  }

  tlines.insert(set[2], line);

  return 0;
}

