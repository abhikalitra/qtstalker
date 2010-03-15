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

#include "Dot.h"

Dot::Dot ()
{
}

void Dot::draw (PlotLine *line, QPixmap &buffer, int startX, int pixelspace, int pos, Scaler &scaler)
{
  QPainter painter;
  painter.begin(&buffer);

  int x = startX;
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

      painter.setPen(color);
      painter.drawPoint(x, y);
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
  Dot *o = new Dot;
  return ((PlotPlugin *) o);
}

