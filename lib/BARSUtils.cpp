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

#include "BARSUtils.h"
#include "PlotLineBar.h"

#include <QtDebug>

BARSUtils::BARSUtils ()
{
}

PlotLine * BARSUtils::getBARS (BarData *data, QColor &_up, QColor &_down, QColor &_neutral)
{
  int size = data->count();
  PlotLine *line = new PlotLine;
  
  QString s = "OHLC";
  line->setPlugin(s);
  
  s = QObject::tr("C");
  line->setLabel(s);
  
  int loop;
  for (loop = 0; loop < size; loop++)
  {
    PlotLineBar bar;
    Bar *tbar = data->getBar(loop);
    bar.append(tbar->getOpen());
    bar.append(tbar->getHigh());
    bar.append(tbar->getLow());
    bar.append(tbar->getClose());
    
    if (loop > 0)
    {
      Bar *pbar = data->getBar(loop - 1);
      if (tbar->getClose() > pbar->getClose())
        bar.setColor(_up);
      else
      {
        if (tbar->getClose() < pbar->getClose())
          bar.setColor(_down);
        else
          bar.setColor(_neutral);
      }
    }
    else
      bar.setColor(_neutral);
    
    line->append(bar);
  }

  return line;
}

