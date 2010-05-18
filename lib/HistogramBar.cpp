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
#include "Utils.h"

#include <QPainter>

HistogramBar::HistogramBar ()
{
  _type = "Histogram Bar";
}

void HistogramBar::draw (PlotData &pd, Scaler &scaler)
{
  QPainter painter;
  painter.begin(&pd.buffer);

  int x = 0;
  int zero = scaler.convertToY(0);
  int loop = pd.startIndex;

  for (; loop <= pd.endIndex; loop++, x += pd.barSpacing)
  {
    PlotLineBar *bar = data(loop);
    if (! bar)
      continue;

    int y = scaler.convertToY(bar->data());

    painter.fillRect(x, y, pd.barSpacing - 1, zero - y, bar->color());
  }

  painter.end();
}

void HistogramBar::info (int i, Setting *set)
{
  PlotLineBar *bar = data(i);
  if (! bar)
    return;

  QString d;
  Utils util;
  util.strip(bar->data(), 4, d);
  set->setData(_label, d);
}

