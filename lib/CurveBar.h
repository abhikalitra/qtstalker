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

#ifndef CURVE_BAR_HPP
#define CURVE_BAR_HPP

#include "Data.h"

class CurveBar : public Data
{
  public:
    enum Parm
    {
      _DATE = -10,
      _OPEN = -20,
      _HIGH = -30,
      _LOW = -40,
      _CLOSE = -50,
      _VOLUME = -60,
      _OI = -70,
      _COLOR = -80,
      _VALUE = -90
    };

    CurveBar ();
    void clear ();
    int highLow (double &, double &);
};

#endif
