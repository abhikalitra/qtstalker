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

#include "Candle.h"

Candle::Candle ()
{
}

void Candle::draw (PlotLine *line, PlotData &pd, Scaler &scaler)
{
  QPainter painter;
  painter.begin(&pd.buffer);

  int loop = pd.pos;
  int x = 0;
  QColor c;
  bool ff = FALSE;

  while ((x < pd.buffer.width() - pd.scaleWidth) && (loop < (int) line->count()))
  {
    if (loop > -1)
    {
      ff = FALSE;
      PlotLineBar bar;
      line->getData(loop, bar);
      
      if (bar.getData(3) < bar.getData(0))
        ff = TRUE;

      bar.getColor(c);
      painter.setPen(c);

      int xh = scaler.convertToY(bar.getData(1));
      int xl = scaler.convertToY(bar.getData(2));
      int xc = scaler.convertToY(bar.getData(3));
      int xo = scaler.convertToY(bar.getData(0));

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
        painter.fillRect(x, xo, 5, xc - xo, c);
      }
    }

    x += pd.barSpacing;
    loop++;
  }

  painter.end();
}

