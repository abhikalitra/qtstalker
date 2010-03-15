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

#include "OHLC.h"

OHLC::OHLC ()
{
}

void OHLC::draw (PlotLine *line, QPixmap &buffer, int startX, int pixelspace, int pos, Scaler &scaler)
{
  QPainter painter;
  painter.begin(&buffer);

  int loop = pos;
  int x = startX;
  QColor c;

  while ((x < buffer.width()) && (loop < (int) line->count()))
  {
    if (loop > -1)
    {
      PlotLineBar bar;
      line->getData(loop, bar);

      bar.getColor(c);
      painter.setPen(c);

      int y = scaler.convertToY(bar.getData(0));
      painter.drawLine (x - 2, y, x, y);

      y = scaler.convertToY(bar.getData(3));
      painter.drawLine (x + 2, y, x, y);

      y = scaler.convertToY(bar.getData(1));
      int y2 = scaler.convertToY(bar.getData(2));
      painter.drawLine (x, y, x, y2);
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
  OHLC *o = new OHLC;
  return ((PlotPlugin *) o);
}

