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

#include "DEMA.h"
#include "EMA.h"
#include "PlotFactory.h"
#include "PlotLineBar.h"

#include <QList>
#include <QtDebug>

DEMA::DEMA ()
{
}

PlotLine * DEMA::dema (PlotLine *in, int period, int lineType, QColor &color)
{
  EMA ema;
  PlotLine *ema1 = ema.ema(in, period, lineType, color);
  if (! ema1)
    return 0;

  PlotLine *ema2 = ema.ema(ema1, period, lineType, color);
  if (! ema2)
  {
    delete ema1;
    return 0;
  }

  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
  {
    delete ema1;
    delete ema2;
    return 0;
  }

  QList<int> keys;
  ema2->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar *bar = ema1->data(keys.at(loop));
    if (! bar)
      continue;

    PlotLineBar *bar2 = ema2->data(keys.at(loop));

    line->setData(keys.at(loop), new PlotLineBar(color, (bar->data() * 2) - bar2->data()));
  }

  delete ema1;
  delete ema2;

  return line;
}

