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

#include "WMA.h"
#include "PlotFactory.h"
#include "PlotLineBar.h"

#include <QList>
#include <QtDebug>

WMA::WMA ()
{
}

PlotLine * WMA::wma (PlotLine *in, int period, int lineType, QColor &color)
{
  if (in->count() < period)
    return 0;

  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
    return 0;

  QList<int> keys;
  in->keys(keys);

  int loop = period - 1;
  for (; loop < keys.count(); loop++)
  {
    int loop2 = loop;
    double t = 0;
    double divider = 0;
    int weight = period;
    for (; weight > 0; weight--, loop2--)
    {
      PlotLineBar *bar = in->data(keys.at(loop2));
      t += bar->data() * (double) weight;
      divider += weight;
    }

    line->setData(keys.at(loop), new PlotLineBar(color, t / divider));
  }

  return line;
}

