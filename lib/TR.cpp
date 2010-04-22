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

#include "TR.h"

#include <QtDebug>
#include <cmath>


TR::TR ()
{
}

PlotLine * TR::getTR (BarData *data)
{
  PlotLine *line = new PlotLine;
  int size = data->count();
  int loop = 1;
  for (; loop < size; loop++)
  {
    Bar *bar = data->getBar(loop);
    Bar *pbar = data->getBar(loop - 1);
    double tr = fabs(bar->getHigh() - bar->getLow());
    double t = fabs(bar->getHigh() - pbar->getClose());
    if (t > tr)
      tr = t;
    t = fabs(bar->getLow() - pbar->getClose());
    if (t > tr)
      tr = t;
    line->append(tr);
  }

  return line;
}

PlotLine * TR::getNTR (BarData *data)
{
  PlotLine *line = new PlotLine;
  int size = data->count();
  int loop = 1;
  for (; loop < size; loop++)
  {
    Bar *bar = data->getBar(loop);
    Bar *pbar = data->getBar(loop - 1);
    double tr = fabs(bar->getHigh() - bar->getLow());
    double t = fabs(bar->getHigh() - pbar->getClose());
    if (t > tr)
      tr = t;
    t = fabs(bar->getLow() - pbar->getClose());
    if (t > tr)
      tr = t;
    
    line->append((tr / bar->getClose()) * 100);
  }

  return line;
}

