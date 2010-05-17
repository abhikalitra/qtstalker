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

#include "STOCHUtils.h"
#include "MIN.h"
#include "MAX.h"
#include "PlotLineBar.h"
#include "PlotFactory.h"

#include <QtDebug>

STOCHUtils::STOCHUtils ()
{
}

PlotLine * STOCHUtils::fastK (BarData *data, int period, int lineType, QColor &color)
{
  if (data->count() < period)
    return 0;

  PlotLine *close = data->getInput(BarData::Close);
  if (! close)
  {
    qDebug() << "STOCHUtils::getFastK: close not found";
    return 0;
  }

  PlotLine *high = data->getInput(BarData::High);
  if (! high)
  {
    qDebug() << "STOCHUtils::getFastK: high not found";
    delete close;
    return 0;
  }

  PlotLine *low = data->getInput(BarData::Low);
  if (! low)
  {
    qDebug() << "STOCHUtils::getFastK: low not found";
    delete close;
    delete high;
    return 0;
  }

  PlotLine *k = fastK(close, high, low, period, lineType, color);
  
  delete close;
  delete high;
  delete low;

  return k;
}

PlotLine * STOCHUtils::fastK (PlotLine *in, int period, int lineType, QColor &color)
{
  if (in->count() < period)
    return 0;

  return fastK(in, in, in, period, lineType, color);
}

PlotLine * STOCHUtils::fastK (PlotLine *in, PlotLine *high, PlotLine *low, int period, int lineType, QColor &color)
{
  if (in->count() < period)
    return 0;

  MAX max;
  PlotLine *hh = max.max(high, period, lineType, color);
  if (! hh)
    return 0;

  MIN min;
  PlotLine *ll = min.min(low, period, lineType, color);
  if (! ll)
  {
    delete hh;
    return 0;
  }

  PlotFactory fac;
  PlotLine *k = fac.plot(lineType);
  if (! k)
    return 0;

  QList<int> keys;
  in->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar *bar = in->data(keys.at(loop));
    PlotLineBar *llbar = ll->data(keys.at(loop));
    if (! llbar)
      continue;
    
    PlotLineBar *hhbar = hh->data(keys.at(loop));
    if (! hhbar)
      continue;

    double t = bar->data() - llbar->data();
    double t2 = hhbar->data() - llbar->data();
    k->setData(keys.at(loop), new PlotLineBar(color, 100 * (t / t2)));
  }

  delete hh;
  delete ll;

  return k;
}

