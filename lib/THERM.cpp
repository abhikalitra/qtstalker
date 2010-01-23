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

/* The "Market Thermometer" indicator is described in
   Dr. Alexander Elder's book _Come Into My Trading Room_, p.162 */

/* Alert Notes
1) enter when therm falls below MA
2) exit when threshold is triggered
3) explosive move expected when therm stays below MA for 5-7 days
*/


#include "THERM.h"

#include <QtDebug>
#include <math.h>


THERM::THERM ()
{
}

int THERM::calculate (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,THERM,<NAME>

  if (set.count() != 3)
  {
    qDebug() << "THERM::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << set[1] << "::calculate: duplicate name" << set[2];
    return 1;
  }

  PlotLine *therm = new PlotLine();
  int loop;
  double thermometer = 0;
  for (loop = 1; loop < (int) data->count(); loop++)
  {
    double high = fabs(data->getHigh(loop) - data->getHigh(loop - 1));
    double lo = fabs(data->getLow(loop - 1) - data->getLow(loop));

    if (high > lo)
      thermometer = high;
    else
      thermometer = lo;

    therm->append(thermometer);
  }

  tlines.insert(set[2], therm);

  return 0;
}

