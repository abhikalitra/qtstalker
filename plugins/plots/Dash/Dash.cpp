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

#include "Dash.h"

Dash::Dash ()
{
}

void Dash::draw (PlotLine *line, BarData *, QPixmap &buffer, int startX, int pixelspace,
		 int pos, Scaler &scaler)
{
  QPainter painter;
  painter.begin(&buffer);

  int x = -1;
  int x2 = startX;
  int y = -1;
  int y2 = -1;
  int loop = pos;

  Scaler scale;
  if (line->getScaleFlag())
  {
    scale.set(scaler.getHeight(),
  	      line->getHigh(),
	      line->getLow(),
	      scaler.getLogScaleHigh(),
	      scaler.getLogRange(),
	      scaler.getLogFlag());
  }

  QPen pen;
  pen.setStyle(Qt::SolidLine);
  painter.setPen(pen);

  while ((x2 < buffer.width()) && (loop < (int) line->count()))
  {
    if (loop > -1)
    {
      QColor color;
      double d = line->getData(loop, color);
      
      if (line->getScaleFlag())
        y2 = scale.convertToY(d);
      else
        y2 = scaler.convertToY(d);

      if (y != -1)
      {
        painter.setPen(color);
        painter.drawLine (x, y, x2, y2);
      }
      
      x = x2;
      y = y2;
    }

    x2 = x2 + pixelspace;
    loop++;
  }

  painter.end();
}

//*************************************************************
//*************************************************************
//*************************************************************

PlotPlugin * createPlotPlugin ()
{
  Dash *o = new Dash;
  return ((PlotPlugin *) o);
}

