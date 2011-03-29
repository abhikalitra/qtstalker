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

#ifndef OHLC_HPP
#define OHLC_HPP

#include "Plugin.h"

class OHLC : public Plugin
{
  Q_OBJECT

  public:
    enum Parm
    {
      _COLOR_UP = 10,
      _COLOR_DOWN = 20,
      _COLOR_NEUTRAL = 30,
      _LABEL = 40,
      _STYLE = 50,
      _MA1_TYPE = 60,
      _MA1_STYLE = 70,
      _MA1_LABEL = 80,
      _MA1_COLOR = 90,
      _MA1_PERIOD = 100,
      _MA2_TYPE = 110,
      _MA2_STYLE = 120,
      _MA2_LABEL = 130,
      _MA2_COLOR = 140,
      _MA2_PERIOD = 150,
      _MA3_TYPE = 160,
      _MA3_STYLE = 170,
      _MA3_LABEL = 180,
      _MA3_COLOR = 190,
      _MA3_PERIOD = 200,
      _MA1_CHECK = 210,
      _MA2_CHECK = 220,
      _MA3_CHECK = 230,
      _MA1_INPUT = 240,
      _MA2_INPUT = 250,
      _MA3_INPUT = 260
    };

    OHLC ();
    int command (Command *);
    int calculate (BarData *, Indicator *);
    void defaults (Setting *);
    void dialog (QWidget *, Indicator *);
    void testRuleDialog (QWidget *, Setting *);
};

extern "C"
{
  Plugin * createPlugin ();
}

#endif
