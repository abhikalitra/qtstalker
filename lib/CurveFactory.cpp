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

#include "CurveFactory.h"
#include "CurveOHLC.h"
#include "CurveHistogram.h"
#include "CurveCandle.h"
#include "CurveLine.h"

#include <QDebug>

CurveFactory::CurveFactory ()
{
  _types << "Candle" << "Dash" << "Dot" << "Line" << "Histogram" << "Histogram Bar" << "OHLC";
}

Curve * CurveFactory::curve (QString type)
{
  Curve *curve = 0;

  switch ((Type) _types.indexOf(type))
  {
    case _CANDLE:
      curve = new CurveCandle;
      break;
    case _DASH:
      curve = new CurveLine(Qt::DashLine);
      break;
    case _DOT:
      curve = new CurveLine(Qt::DotLine);
      break;
    case _LINE:
      curve = new CurveLine(Qt::SolidLine);
      break;
    case _HISTOGRAM:
      curve = new CurveHistogram("Histogram");
      break;
    case _HISTOGRAM_BAR:
      curve = new CurveHistogram("Histogram Bar");
      break;
    case _OHLC:
      curve = new CurveOHLC;
      break;
    default:
      break;
  }

  return curve;
}

QStringList CurveFactory::list ()
{
  return _types;
}
