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
      _ID = -10,
      _DATE = -20,
      _DATE_2 = -30,
      _PRICE = -40,
      _PRICE_2 = -50,
      _COLOR = -60,
      _EXTEND = -70,
      _TEXT = -80,
      _FONT = -90,
      _HIGH = -100,
      _LOW = -110,
      _LEVEL_1 = -120,
      _LEVEL_2 = -130,
      _LEVEL_3 = -140,
      _LEVEL_4 = -150,
      _LEVEL_5 = -160,
      _LEVEL_6 = -170,
      _SYMBOL = -180,
      _CHART = -190,
      _TYPE = -200,
      _Z = -210,
      _PEN = -220,
      _RO = -230
    };

    ChartObjectData ();
    void clear ();
    int highLow (double &, double &);
};

#endif