/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

void Candle::draw (PlotLine *line, QPixmap &buffer, int startX, int pixelspace, int pos, Scaler &scaler)
{
  QPainter painter;
  painter.begin(&buffer);

  int loop = pos;
  int x = startX;
  QColor c;
  bool ff = FALSE;

  while ((x < buffer.width()) && (loop < (int) line->count()))
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
        painter.drawLine (x, xh, x, xc);
        painter.drawLine (x, xo, x, xl);

        if (xc == xo)
          painter.drawLine (x - 2, xo, x + 2, xo);
        else
          painter.drawRect(x - 2, xc, 5, xo - xc);
      }
      else
      {
        painter.drawLine (x, xh, x, xl);

        if (xc == xo)
          painter.drawLine (x - 2, xo, x + 2, xo);
        else
          painter.fillRect(x - 2, xo, 5, xc - xo, c);
      }
    }

    x = x + pixelspace;
    loop++;
  }

  painter.end();
}

//*************************************************************
//*************************************************************
//*************************************************************

PlotPlugin * createPlotPlugin ()
{
  Candle *o = new Candle;
  return ((PlotPlugin *) o);
}

