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
  _plotList << "Candle" << "Dash" << "Dot" << "Histogram" << "Histogram Bar" << "Horizontal" << "Line" << "OHLC";
}

PlotLine * PlotFactory::plot (int type)
{
  QString s = _plotList.at(type);
  return plot(s);
}

PlotLine * PlotFactory::plot (QString &plot)
{
  PlotLine *line = 0;
  
  switch ((PlotType) _plotList.indexOf(plot))
  {
    case PlotTypeCandle:
      line = new Candle;
      break;
    case PlotTypeDash:
      line = new Dash;
      break;
    case PlotTypeDot:
      line = new Dot;
      break;
    case PlotTypeHistogram:
      line = new Histogram;
      break;
    case PlotTypeHistogramBar:
      line = new HistogramBar;
      break;
    case PlotTypeHorizontal:
      line = new Horizontal;
      break;
    case PlotTypeLine:
      line = new Line;
      break;
    case PlotTypeOHLC:
      line = new OHLC;
      break;
    default:
      break;
  }
  
  return line;
}

void PlotFactory::list (QStringList &l, int flag)
{
  l = _plotList;
  if (flag)
  {
    l.removeAll("Candle");
    l.removeAll("OHLC");
  }
}

int PlotFactory::typeFromString (QString &d)
{
  return _plotList.indexOf(d);
}

