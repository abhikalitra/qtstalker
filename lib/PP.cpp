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

#include "PP.h"

#include <QtDebug>


PP::PP ()
{
}

int PP::calculate (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PP,<NAME>,<POINT>

  if (set.count() != 4)
  {
    qDebug() << "PP::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << set[1] << "::calculate: duplicate name" << set[2];
    return 1;
  }

  bool ok;
  int point = set[3].toInt(&ok);
  if (! ok)
  {
    qDebug() << "PP::calculate: invalid point" << set[3];
    return 1;
  }
  else
  {
    if (point < 0 || point > 5)
    {
      qDebug() << "PP::calculate: invalid point" << set[3];
      return 1;
    }
  }

  PlotLine *output = new PlotLine;
  double high = data->getHigh(data->count() - 1);
  double low = data->getLow(data->count() - 1);
  double close = data->getClose(data->count() - 1);
  double pp = 0;
  double t = 0;

  switch (point)
  {
    case 0: // first resistance
      pp = (high + low + close) / 3;
      t = (2 * pp) - low;
      output->append(t);
      break;
    case 1: // second resistance
      pp = (high + low + close) / 3;
      t = pp + (high - low);
      output->append(t);
      break;
    case 2: // third resistance
      pp = (high + low + close) / 3;
      t = (2 * pp) + (high - (2 * low));
      output->append(t);
      break;
    case 3: // first support
      pp = (high + low + close) / 3;
      t = (2 * pp) - high;
      output->append(t);
      break;
    case 4: // second support
      pp = (high + low + close) / 3;
      t = pp - (high - low);
      output->append(t);
      break;
    case 5: // third support
      pp = (high + low + close) / 3;
      t = (2 * pp) - ((2 * high) - low);
      output->append(t);
      break;
    default:
      break;
  }

  tlines.insert(set[2], output);

  return 0;
}

