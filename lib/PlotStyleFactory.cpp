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

#include "PlotStyleFactory.h"
#include "PlotStyleCandle.h"
#include "PlotStyleDash.h"
#include "PlotStyleDot.h"
#include "PlotStyleHistogram.h"
#include "PlotStyleHistogramBar.h"
#include "PlotStyleHorizontal.h"
#include "PlotStyleLine.h"
#include "PlotStyleOHLC.h"

#include <QtDebug>

PlotStyleFactory::PlotStyleFactory ()
{
  _styleList << "Candle" << "Dash" << "Dot" << "Histogram" << "Histogram Bar" << "Horizontal" << "Line" << "OHLC";
}

PlotStyle * PlotStyleFactory::plotStyle (QString &type)
{
  return plotStyle(_styleList.indexOf(type));
}

PlotStyle * PlotStyleFactory::plotStyle (int type)
{
  PlotStyle *ps = 0;
  
  switch ((Style) type)
  {
    case _Candle:
      ps = new PlotStyleCandle;
      break;
    case _Dash:
      ps = new PlotStyleDash;
      break;
    case _Dot:
      ps = new PlotStyleDot;
      break;
    case _Histogram:
      ps = new PlotStyleHistogram;
      break;
    case _HistogramBar:
      ps = new PlotStyleHistogramBar;
      break;
    case _Horizontal:
      ps = new PlotStyleHorizontal;
      break;
    case _Line:
      ps = new PlotStyleLine;
      break;
    case _OHLC:
      ps = new PlotStyleOHLC;
      break;
    default:
      break;
  }

  return ps;
}

void PlotStyleFactory::list (QStringList &l, int d)
{
  l = _styleList;
  
  if (d)
  {
    l.removeAll(QString("Candle"));
    l.removeAll(QString("OHLC"));
    l.removeAll(QString("Horizontal"));
  }
}

QString PlotStyleFactory::typeToString (int type)
{
  if (type < 0 || type >= _styleList.count())
    return QString();
  
  return _styleList.at(type);
}

