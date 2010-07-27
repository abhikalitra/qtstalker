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

/* The "Market Thermometer" _indicator is described in
   Dr. Alexander Elder's book _Come Into My Trading Room_, p.162 */

/* Alert Notes
1) enter when therm falls below MA
2) exit when threshold is triggered
3) explosive move expected when therm stays below MA for 5-7 days
*/

#include "FunctionTHERM.h"
#include "FunctionMA.h"
#include "PlotStyleFactory.h"

#include <QtDebug>
#include <cmath>

FunctionTHERM::FunctionTHERM ()
{
}

int FunctionTHERM::script (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,THERM,<NAME>,<SMOOTHING_PERIOD>,<SMOOTHING_TYPE>
  //     0       1     2      3             4               5

  if (set.count() != 6)
  {
    qDebug() << "FunctionTHERM::script: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "FunctionTHERM::script: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int smoothing = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionTHERM::script: invalid smoothing" << set[4];
    return 1;
  }

  FunctionMA mau;
  int type = mau.typeFromString(set[5]);
  if (type == -1)
  {
    qDebug() << "FunctionTHERM::script: invalid smoothing type" << set[5];
    return 1;
  }

  PlotLine *line = calculate(data, smoothing, type);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

PlotLine * FunctionTHERM::calculate (BarData &data, int smoothing, int type)
{
  if (data.count() < smoothing || data.count() < 2)
    return 0;

  PlotLine *line = new PlotLine;

  int loop = 1;
  double thermometer = 0;
  for (; loop < (int) data.count(); loop++)
  {
    Bar bar = data.getBar(loop);
    Bar pbar = data.getBar(loop - 1);
    double high = fabs(bar.getHigh() - pbar.getHigh());
    double lo = fabs(pbar.getLow() - bar.getLow());

    if (high > lo)
      thermometer = high;
    else
      thermometer = lo;

    line->setData(loop, thermometer);
  }

  if (smoothing > 1)
  {
    FunctionMA mau;
    PlotLine *ma = mau.calculate(line, smoothing, type);
    if (! ma)
    {
      delete line;
      return 0;
    }

    delete line;
    line = ma;
  }

  return line;
}

