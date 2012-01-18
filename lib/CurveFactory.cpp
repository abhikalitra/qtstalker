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
#include "CurveType.h"
#include "CurveOHLC.h"
#include "CurveHistogram.h"
#include "CurveLine.h"

#include <QDebug>

CurveFactory::CurveFactory ()
{
}

Curve * CurveFactory::curve (QString type)
{
  Curve *curve = 0;
  CurveType ct;

  switch ((CurveType::Type) ct.stringToIndex(type))
  {
    case CurveType::_LINE:
      curve = new CurveLine;
      break;
    case CurveType::_HISTOGRAM:
      curve = new CurveHistogram;
      break;
    case CurveType::_OHLC:
      curve = new CurveOHLC;
      break;
    default:
      break;
  }

  return curve;
}
