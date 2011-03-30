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

#ifndef PLUGIN_BBANDS_HPP
#define PLUGIN_BBANDS_HPP

#include "Plugin.h"

class BBANDS : public Plugin
{
  Q_OBJECT

  public:
    enum Parm
    {
      _COLOR_UP = 10,
      _COLOR_MID = 20,
      _COLOR_DOWN = 30,
      _STYLE_UP = 40,
      _STYLE_MID = 50,
      _STYLE_DOWN = 60,
      _LABEL_UP = 70,
      _LABEL_MID = 80,
      _LABEL_DOWN = 90,
      _DEVIATION_UP = 100,
      _DEVIATION_DOWN = 110,
      _INPUT = 120,
      _PERIOD = 130,
      _MA_TYPE = 140,
      _COLOR_BARS_UP = 150,
      _COLOR_BARS_DOWN = 160,
      _COLOR_BARS_NEUTRAL = 170,
      _STYLE_BARS = 180
    };

    BBANDS ();
    int command (Command *);
    int calculate (BarData *, Indicator *);
    void defaults (Setting *);
    void dialog (QWidget *, Indicator *);
};

extern "C"
{
  Plugin * createPlugin ();
}

#endif
