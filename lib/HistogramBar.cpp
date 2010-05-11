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

#include "HistogramBar.h"

HistogramBar::HistogramBar ()
{
}

void HistogramBar::draw (PlotLine *line, PlotData &pd, Scaler &scaler)
{
  QPainter painter;
  painter.begin(&pd.buffer);

  int x = 0;
  int zero = 0;
  Scaler scale;
  if (line->getScaleFlag())
  {
    scale.set(scaler.height(),
  	      line->getHigh(),
	      line->getLow(),
	      scaler.logScaleHigh(),
	      scaler.logRange(),
	      scaler.logFlag());
    zero = scale.convertToY(0);
  }
  else
    zero = scaler.convertToY(0);

  int loop = pd.pos;
  while ((x < pd.buffer.width() - pd.scaleWidth) && (loop < (int) line->count()))
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

      painter.fillRect(x, y, pd.barSpacing - 1, zero - y, color);
    }

    x += pd.barSpacing;
    loop++;
  }

  painter.end();
}

