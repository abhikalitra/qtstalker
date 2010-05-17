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

#include "SMA.h"
#include "PlotFactory.h"
#include "PlotLineBar.h"
#include "Accumulation.h"

#include <QList>
#include <QtDebug>

SMA::SMA ()
{
}

PlotLine * SMA::sma (PlotLine *in, int period, int lineType, QColor &color)
{
  if (in->count() < period)
    return 0;

  Accumulation accum;
  PlotLine *line = accum.accumulation(in, period, lineType, color);
  if (! line)
    return 0;

  QList<int> keys;
  line->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar *bar = line->data(keys.at(loop));
    bar->setData(bar->data() / period);
  }

  return line;
}

