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
#include "DataBarLength.h"
#include "DataBool.h"
#include "DataColor.h"
#include "DataDateRange.h"
#include "DataDateTime.h"
#include "DataDouble.h"
#include "DataFile.h"
#include "DataFont.h"
#include "DataInteger.h"
#include "DataList.h"
#include "DataMA.h"
#include "DataOp.h"
#include "DataString.h"
#include "DataSymbol.h"

#include <QDebug>

DataFactory::DataFactory ()
{
  _types << "BAR_LENGTH" << "BOOL" << "CHART_OBJECT" << "COLOR" << "CURVE" << "CURVE_BAR" << "DATE_RANGE" << "DATETIME";
  _types << "DOUBLE" << "FILE" << "FONT" << "INTEGER" << "LIST" << "MA" << "OP" << "STRING" << "SYMBOL";
}

Data * DataFactory::data (QString d)
{
  return data(d.toInt());
}

Data * DataFactory::data (int type)
{
  Data *data = 0;

  switch ((Type) type)
  {
    case _BAR_LENGTH:
      data = new DataBarLength;
      break;
    case _BOOL:
      data = new DataBool;
      break;
    case _CHART_OBJECT:
      data = new ChartObjectData;
      break;
    case _COLOR:
      data = new DataColor;
      break;
    case _CURVE:
      data = new CurveData;
      break;
    case _CURVE_BAR:
      data = new CurveBar;
      break;
    case _DATE_RANGE:
      data = new DataDateRange;
      break;
    case _DATETIME:
      data = new DataDateTime;
      break;
    case _DOUBLE:
      data = new DataDouble;
      break;
    case _FILE:
      data = new DataFile;
      break;
    case _FONT:
      data = new DataFont;
      break;
    case _INTEGER:
      data = new DataInteger;
      break;
    case _LIST:
      data = new DataList;
      break;
    case _MA:
      data = new DataMA;
      break;
    case _OP:
      data = new DataOp;
      break;
    case _STRING:
      data = new DataString;
      break;
    case _SYMBOL:
      data = new DataSymbol;
      break;
    default:
      break;
  }

  return data;
}

int DataFactory::stringToType (QString d)
{
  return _types.indexOf(d);
}
