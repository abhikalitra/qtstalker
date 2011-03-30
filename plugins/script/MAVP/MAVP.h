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

#ifndef PLUGIN_MAVP_HPP
#define PLUGIN_MAVP_HPP

#include "Plugin.h"

class MAVP : public Plugin
{
  Q_OBJECT

  public:
    enum Parm
    {
      _INPUT = 10,
      _INPUT2 = 20,
      _COLOR = 30,
      _STYLE = 40,
      _LABEL = 50,
      _MA_TYPE = 60,
      _PERIOD_MIN = 70,
      _PERIOD_MAX = 80,
      _STYLE_BARS = 90,
      _COLOR_BARS_UP = 100,
      _COLOR_BARS_DOWN = 110,
      _COLOR_BARS_NEUTRAL = 120
    };

    MAVP ();
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
