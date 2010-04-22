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

#include "RSIUtils.h"

#include <QtDebug>

RSIUtils::RSIUtils ()
{
}

PlotLine * RSIUtils::getRSI (PlotLine *in, int period)
{
  if (in->count() < period)
    return 0;

  int size = in->count();
  PlotLine *line = new PlotLine;
  int loop = period;
  for (; loop < size; loop++)
  {
    double ag = 0;
    double al = 0;
    int count = 0;
    for (; count < period; count++)
    {
      double tp = in->getData(loop - count);
      double yp = in->getData(loop - count - 1);
      if (tp > yp)
	ag += tp - yp;
      else
      {
        if (tp < yp)
	  al += yp - tp;
      }
    }

    ag = ag / (double) period;
    al = al / (double) period;
    double rs = ag / al;
    line->append(100 - (100 / (1 + rs)));
  }

  return line;
}

