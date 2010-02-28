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

#include "HistogramBar.h"

HistogramBar::HistogramBar ()
{
}

void HistogramBar::draw (PlotLine *line, BarData *, QPixmap &buffer, int startX, int pixelspace,
			 int pos, Scaler &scaler)
{
  QPainter painter;
  painter.begin(&buffer);

  int x = startX;
  int zero = 0;
  Scaler scale;
  if (line->getScaleFlag())
  {
    scale.set(scaler.getHeight(),
  	      line->getHigh(),
	      line->getLow(),
	      scaler.getLogScaleHigh(),
	      scaler.getLogRange(),
	      scaler.getLogFlag());
    zero = scale.convertToY(0);
  }
  else
    zero = scaler.convertToY(0);

  int loop = pos;
  while ((x < buffer.width()) && (loop < (int) line->count()))
  {
    if (loop > -1)
    {
      int y;
      QColor color;
      double d = line->getData(loop, color);
      
      if (line->getScaleFlag())
        y = scale.convertToY(d);
      else
        y = scaler.convertToY(d);

      painter.fillRect(x, y, pixelspace - 1, zero - y, color);
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
  HistogramBar *o = new HistogramBar;
  return ((PlotPlugin *) o);
}

