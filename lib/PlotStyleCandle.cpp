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

#include "PlotStyleCandle.h"
#include "Strip.h"

#include <QPainter>

PlotStyleCandle::PlotStyleCandle ()
{
}

void PlotStyleCandle::draw (PlotData &pd, Scaler &scaler, QMap<int, PlotLineBar *> &data)
{
  QPainter painter;
  painter.begin(&pd.buffer);

  int loop = pd.startIndex;
  int x = 0;
  bool ff = FALSE;
  for (; loop <= pd.endIndex; loop++, x += pd.barSpacing)
  {
    PlotLineBar *bar = data.value(loop);
    if (! bar)
      continue;
    
    ff = FALSE;
    if (bar->data(3) < bar->data(0))
      ff = TRUE;

    painter.setPen(bar->color());

    int xh = scaler.convertToY(bar->data(1));
    int xl = scaler.convertToY(bar->data(2));
    int xc = scaler.convertToY(bar->data(3));
    int xo = scaler.convertToY(bar->data(0));

    if (! ff)
    {
      // empty candle
      painter.drawLine (x + 3, xh, x + 3, xc);
      painter.drawLine (x + 3, xo, x + 3, xl);
      painter.drawRect(x, xc, 6, xo - xc);
    }
    else
    {
      // filled candle
      painter.drawLine (x + 2, xh, x + 2, xl);
      painter.fillRect(x, xo, 5, xc - xo, bar->color());
    }
  }

  painter.end();
}

void PlotStyleCandle::info (int i, Setting *set, QMap<int, PlotLineBar *> &data, QString &)
{
  PlotLineBar *bar = data.value(i);
  if (! bar)
    return;

  QString k = "O";
  QString d;

  Strip strip;
  strip.strip(bar->data(0), 4, d);
  set->setData(k, d);

  k = "H";
  strip.strip(bar->data(1), 4, d);
  set->setData(k, d);

  k = "L";
  strip.strip(bar->data(2), 4, d);
  set->setData(k, d);

  k = "C";
  strip.strip(bar->data(3), 4, d);
  set->setData(k, d);
}

