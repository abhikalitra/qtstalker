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

#include <QtDebug>

STOCHUtils::STOCHUtils ()
{
}

PlotLine * STOCHUtils::getFastK (BarData *data, int period)
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

  PlotLine *k = getFastK(close, high, low, period);
  
  delete close;
  delete high;
  delete low;

  return k;
}

PlotLine * STOCHUtils::getFastK (PlotLine *in, int period)
{
  if (in->count() < period)
    return 0;

  return getFastK(in, in, in, period);
}

PlotLine * STOCHUtils::getFastK (PlotLine *in, PlotLine *high, PlotLine *low, int period)
{
  if (in->count() < period)
    return 0;

  MAX max;
  PlotLine *hh = max.getMAX(high, period);
  if (! hh)
    return 0;

  MIN min;
  PlotLine *ll = min.getMIN(low, period);
  if (! ll)
  {
    delete hh;
    return 0;
  }

  int loop = in->count() - 1;
  int minLoop = ll->count() - 1;
  int maxLoop = hh->count() - 1;
  PlotLine *k = new PlotLine;
  while (loop > -1 && minLoop > -1 && maxLoop > -1)
  {
    double t = in->getData(loop) - ll->getData(minLoop);
    double t2 = hh->getData(maxLoop) - ll->getData(minLoop);
    k->prepend(100 * (t / t2));

    loop--;
    minLoop--;
    maxLoop--;
  }

  delete hh;
  delete ll;

  return k;
}

