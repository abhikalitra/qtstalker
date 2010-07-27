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

#include "PlotStyleOHLC.h"
#include "Strip.h"

#include <QPainter>

PlotStyleOHLC::PlotStyleOHLC ()
{
}

void PlotStyleOHLC::draw (PlotData &pd, Scaler &scaler, QMap<int, PlotLineBar *> &data)
{
  QPainter painter;
  painter.begin(&pd.buffer);

  int loop = pd.startIndex;
  int x = 0;
  for (; loop <= pd.endIndex; loop++, x += pd.barSpacing)
  {
    PlotLineBar *bar = data.value(loop);
    if (! bar)
      continue;

    painter.setPen(bar->color());

    // draw the open tick
    int y = scaler.convertToY(bar->data(0));
    painter.drawLine (x, y, x + 2, y);

    // draw the close tick
    y = scaler.convertToY(bar->data(3));
    painter.drawLine (x + 2, y, x + 4, y);

    // draw the high/low tick
    y = scaler.convertToY(bar->data(1));
    int y2 = scaler.convertToY(bar->data(2));
    painter.drawLine (x + 2, y, x + 2, y2);
  }

  painter.end();
}

void PlotStyleOHLC::info (int i, Setting *set, QMap<int, PlotLineBar *> &data, QString &)
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

