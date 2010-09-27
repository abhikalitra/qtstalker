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

#include "FunctionPP.h"

#include <QtDebug>

FunctionPP::FunctionPP ()
{
}

int FunctionPP::script (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,PP,<NAME>,<POINT>
  //     0       1     2    3      4 

  if (set.count() != 5)
  {
    qDebug() << "FunctionPP::script: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "FunctionPP::script: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int point = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionPP::script: invalid point" << set[4];
    return 1;
  }
  else
  {
    if (point < 0 || point > 5)
    {
      qDebug() << "FunctionPP::script: invalid point" << set[4];
      return 1;
    }
  }

  Curve *line = calculate(point, data);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

Curve * FunctionPP::calculate (int point, BarData &data)
{
  if (data.count() < 1)
    return 0;

  Curve *output = new Curve(Curve::Horizontal);

  Bar bar = data.getBar(data.count() - 1);
  double high = bar.getHigh();
  double low = bar.getLow();
  double close = bar.getClose();
  double pp = 0;
  double t = 0;

  switch (point)
  {
    case 0: // first resistance
    {
      pp = (high + low + close) / 3;
      t = (2 * pp) - low;
      output->setBar(0, new CurveBar(t));
      break;
    }
    case 1: // second resistance
    {
      pp = (high + low + close) / 3;
      t = pp + (high - low);
      output->setBar(0, new CurveBar(t));
      break;
    }
    case 2: // third resistance
    {
      pp = (high + low + close) / 3;
      t = (2 * pp) + (high - (2 * low));
      output->setBar(0, new CurveBar(t));
      break;
    }
    case 3: // first support
    {
      pp = (high + low + close) / 3;
      t = (2 * pp) - high;
      output->setBar(0, new CurveBar(t));
      break;
    }
    case 4: // second support
    {
      pp = (high + low + close) / 3;
      t = pp - (high - low);
      output->setBar(0, new CurveBar(t));
      break;
    }
    case 5: // third support
    {
      pp = (high + low + close) / 3;
      t = (2 * pp) - ((2 * high) - low);
      output->setBar(0, new CurveBar(t));
      break;
    }
    default:
      break;
  }

  return output;
}

