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

#include "PlotFactory.h"
#include "Candle.h"
#include "Dash.h"
#include "Dot.h"
#include "Histogram.h"
#include "HistogramBar.h"
#include "Horizontal.h"
#include "Line.h"
#include "OHLC.h"

#include <QDebug>

PlotFactory::PlotFactory ()
{
}

PlotPlugin * PlotFactory::getPlot (QString &plot)
{
  QStringList l;
  getList(l);
  
  PlotPlugin *plug = 0;
  
  switch ((PlotType) l.indexOf(plot))
  {
    case PlotTypeCandle:
      plug = new Candle;
      break;
    case PlotTypeDash:
      plug = new Dash;
      break;
    case PlotTypeDot:
      plug = new Dot;
      break;
    case PlotTypeHistogram:
      plug = new Histogram;
      break;
    case PlotTypeHistogramBar:
      plug = new HistogramBar;
      break;
    case PlotTypeHorizontal:
      plug = new Horizontal;
      break;
    case PlotTypeLine:
      plug = new Line;
      break;
    case PlotTypeOHLC:
      plug = new OHLC;
      break;
    default:
      break;
  }
  
  return plug;
}

void PlotFactory::getList (QStringList &l)
{
  l.clear();
  l << "Candle" << "Dash" << "Dot" << "Histogram" << "Histogram Bar" << "Horizontal" << "Line" << "OHLC";
}


