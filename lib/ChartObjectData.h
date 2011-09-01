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

// *************************************************************************************************
// *************************************************************************************************

#ifndef CHART_OBJECT_DATA_HPP
#define CHART_OBJECT_DATA_HPP

#include <QHash>

#include "Data.h"

class ChartObjectData : public Data
{
  public:
    enum Parm
    {
      _ID,
      _DATE,
      _DATE2,
      _PRICE,
      _PRICE2,
      _COLOR,
      _EXTEND,
      _TEXT,
      _FONT,
      _HIGH,
      _LOW,
      _LINE1,
      _LINE2,
      _LINE3,
      _LINE4,
      _LINE5,
      _LINE6,
      _SYMBOL,
      _CHART,
      _TYPE,
      _Z,
      _PEN,
      _RO
    };

    ChartObjectData ();
    void clear ();
    int highLow (double &, double &);
};

#endif
