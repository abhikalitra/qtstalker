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

#include "CANDLES.h"

#include <QtDebug>


CANDLES::CANDLES ()
{
}

int CANDLES::calculate (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,CANDLES,<NAME>,<CANDLE_COLOR>

  if (set.count() != 4)
  {
    qDebug() << "CANDLES::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << set[1] << "::calculate: duplicate name" << set[2];
    return 1;
  }

  QColor color(set[3]);
  if (! color.isValid())
  {
    qDebug() << "CANDLES::calculate: invalid color" << set[3];
    return 1;
  }

  PlotLine *line = new PlotLine;
  int loop;
  int size = data->count();
  for (loop = 0; loop < size; loop++)
  {
    double cl = data->getClose(loop);
    double op = data->getOpen(loop);
    bool fillFlag = FALSE;

    if (op != 0)
    {
      if (cl < op)
        fillFlag = TRUE;
    }

    line->append(color, op, data->getHigh(loop), data->getLow(loop), cl, fillFlag);

//    QDateTime dt;
//    data->getDate(loop, dt);
//    line->append(dt);
  }

  tlines.insert(set[2], line);

  return 0;
}

