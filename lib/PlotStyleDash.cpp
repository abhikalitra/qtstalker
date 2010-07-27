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

#include "PlotStyleDash.h"

#include <QPainter>

PlotStyleDash::PlotStyleDash ()
{
}

void PlotStyleDash::draw (PlotData &pd, Scaler &scaler, QMap<int, PlotLineBar *> &data)
{
  QPainter painter;
  painter.begin(&pd.buffer);

  int x = -1;
  int x2 = 0;
  int y = -1;
  int y2 = -1;
  int loop = pd.startIndex;

  QPen pen;
  pen.setStyle(Qt::DashLine);
  pen.setJoinStyle(Qt::RoundJoin);
  pen.setCapStyle(Qt::RoundCap);
  pen.setWidth(1);

  for (; loop <= pd.endIndex; loop++, x2 += pd.barSpacing)
  {
    PlotLineBar *bar = data.value(loop);
    if (bar)
    {
      y2 = scaler.convertToY(bar->data());

      if (y != -1)
      {
        pen.setColor(bar->color());
        painter.setPen(pen);
        painter.drawLine (x, y, x2, y2);
      }

      x = x2;
      y = y2;
    }
  }

  painter.end();
}
