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

#include "TRIMA.h"
#include "SMA.h"
#include "PlotFactory.h"
#include "PlotLineBar.h"

#include <QList>
#include <QtDebug>

TRIMA::TRIMA ()
{
}

PlotLine * TRIMA::trima (PlotLine *in, int period, int lineType, QColor &color)
{
  if (in->count() < period)
    return 0;

  int period1 = 0;
  int period2 = 0;
  if (period % 2 == 0)
  {
    // even period
    period1 = period / 2;
    period2 = period1;
    period2++;
  }
  else
  {
    // ood
    period1 = (period / 2) + 1;
    period2 = period1;
  }

  SMA sma;
  PlotLine *sma1 = sma.sma(in, period1, lineType, color);
  if (! sma1)
    return 0;

  PlotLine *sma2 = sma.sma(sma1, period2, lineType, color);
  if (! sma2)
  {
    delete sma1;
    return 0;
  }

  delete sma1;
  return sma2;
}

