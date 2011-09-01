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

#include "DataFactory.h"
#include "ChartObjectData.h"
#include "CurveBar.h"
#include "CurveData.h"
#include "Symbol.h"

#include <QDebug>

DataFactory::DataFactory ()
{
  _types << "CHART_OBJECT" << "CURVE" << "CURVE_BAR" << "DATA" << "SYMBOL";
}

Data * DataFactory::data (QString type)
{
  Data *data = 0;

  switch ((Type) _types.indexOf(type))
  {
    case _CHART_OBJECT:
      data = new ChartObjectData;
      break;
    case _CURVE:
      data = new CurveData;
      break;
    case _CURVE_BAR:
      data = new CurveBar;
      break;
    case _DATA:
      data = new Data;
      break;
    case _SYMBOL:
      data = new Symbol;
      break;
    default:
      break;
  }

  return data;
}

QStringList DataFactory::list ()
{
  return _types;
}

int DataFactory::stringToType (QString d)
{
  return _types.indexOf(d);
}
