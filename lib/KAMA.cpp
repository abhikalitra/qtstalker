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

#include "KAMA.h"
#include "PlotFactory.h"
#include "PlotLineBar.h"

#include <QList>
#include <QtDebug>
#include <cmath>

KAMA::KAMA ()
{
}

PlotLine * KAMA::kama (PlotLine *in, int period, int lineType, QColor &color)
{
  if (in->count() < period)
    return 0;

  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
    return 0;

  QList<int> keys;
  in->keys(keys);

  double fast = 0.6667;
  double slow = 0.0645;
  int loop = period;

  PlotLineBar *bar = in->data(keys.at(loop));
  double prevAMA = bar->data();

  for (; loop < keys.count(); loop++)
  {
    // get direction
    bar = in->data(keys.at(loop));
    PlotLineBar *pbar = in->data(keys.at(loop - period));

    double direction = bar->data() - pbar->data();

    // get the er
    int count = 0;
    int loop2 = loop;
    double t = 0;
    for (; count < period; count++, loop2--)
    {
      PlotLineBar *bar = in->data(keys.at(loop2));
      PlotLineBar *pbar = in->data(keys.at(loop2 - 1));
      t = t + fabs(bar->data() - pbar->data());
    }
    double er = fabs(direction / t);

    double ssc = er * (fast - slow) + slow;
    double c = ssc * ssc;

    double ama = c * (bar->data() - prevAMA) + prevAMA;
    prevAMA = ama;
    line->setData(keys.at(loop), new PlotLineBar(color, ama));
  }

  return line;
}

