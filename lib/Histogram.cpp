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

#include "Histogram.h"
#include "Utils.h"

#include <QPainter>

Histogram::Histogram ()
{
  _type = "Histogram";
}

void Histogram::draw (PlotData &pd, Scaler &scaler)
{
  QPainter painter;
  painter.begin(&pd.buffer);

  int loop = pd.startIndex;

  QPolygon pa(4);

  int zero = 0;
  Scaler scale;
  if (scaleFlag())
  {
    scale.set(scaler.height(),
  	      high(),
	      low(),
	      scaler.logScaleHigh(),
	      scaler.logRange(),
	      scaler.logFlag());
    zero = scale.convertToY(0);
  }
  else
    zero = scaler.convertToY(0);

  int x = -1;
  int x2 = 0;
  int y = -1;
  int y2 = -1;

  for (; loop <= pd.endIndex; loop++, x2 += pd.barSpacing)
  {
    PlotLineBar *bar = data(loop);
    if (bar)
    {
      if (scaleFlag())
        y2 = scale.convertToY(bar->data());
      else
        y2 = scaler.convertToY(bar->data());

      pa.setPoint(0, x, zero);
      pa.setPoint(1, x, y);
      pa.setPoint(2, x2, y2);
      pa.setPoint(3, x2, zero);

      painter.setPen(bar->color());
      painter.setBrush(bar->color());
      
      if (y != -1)
        painter.drawPolygon(pa, Qt::OddEvenFill);

      x = x2;
      y = y2;
    }
  }

  painter.end();
}

void Histogram::info (int i, Setting *set)
{
  PlotLineBar *bar = data(i);
  if (! bar)
    return;

  QString d;
  Utils util;
  util.strip(bar->data(), 4, d);
  set->setData(_label, d);
}

