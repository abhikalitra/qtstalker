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

#include "EMA.h"
#include "PlotFactory.h"
#include "PlotLineBar.h"

#include <QList>
#include <QtDebug>

EMA::EMA ()
{
}

PlotLine * EMA::ema (PlotLine *in, int period, int lineType, QColor &color)
{
  if (in->count() < period)
    return 0;

  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
    return 0;

  // first calculate the expo seed
  // we just calculate an SMA

  QList<int> keys;
  in->keys(keys);
  
  int loop = period - 1;
  double total = 0;
  int count = loop;
  for (; count > -1; count--)
  {
    PlotLineBar *bar = in->data(keys.at(loop - count));
    total += bar->data();
  }
  double prevMA = total / (double) period;
  line->setData(keys.at(loop++), new PlotLineBar(color, prevMA));

  // now calculate the EMA proper
  double multiplier = 2.0 / (double) (period + 1);
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar *bar = in->data(keys.at(loop));
    double t = ((bar->data() - prevMA) * multiplier) + prevMA;
    prevMA = t;
    line->setData(keys.at(loop), new PlotLineBar(color, t));
  }
  
  return line;
}

