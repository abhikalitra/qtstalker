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

#include "Dash.h"
#include "Utils.h"

#include <QPainter>

Dash::Dash ()
{
  _type = "Dash";
}

void Dash::draw (PlotData &pd, Scaler &scaler)
{
  QPainter painter;
  painter.begin(&pd.buffer);

  int x = -1;
  int x2 = 0;
  int y = -1;
  int y2 = -1;
  int loop = pd.startIndex;

  Scaler scale;
  if (scaleFlag())
  {
    scale.set(scaler.height(),
  	      high(),
	      low(),
	      scaler.logScaleHigh(),
	      scaler.logRange(),
	      scaler.logFlag());
  }

  for (; loop <= pd.endIndex; loop++, x2 += pd.barSpacing)
  {
    PlotLineBar *bar = data(loop);
    if (bar)
    {
      if (scaleFlag())
        y2 = scale.convertToY(bar->data());
      else
        y2 = scaler.convertToY(bar->data());

      if (y != -1)
      {
        QPen pen;
        pen.setStyle(Qt::DashLine);
        pen.setWidth(1);
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

void Dash::info (int i, Setting *set)
{
  PlotLineBar *bar = data(i);
  if (! bar)
    return;

  QString d;
  Utils util;
  util.strip(bar->data(), 4, d);
  set->setData(_label, d);
}

