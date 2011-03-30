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

#ifndef PLUGIN_MAMA_HPP
#define PLUGIN_MAMA_HPP

#include "Plugin.h"

class MAMA : public Plugin
{
  Q_OBJECT

  public:
    enum Parm
    {
      _INPUT = 10,
      _COLOR_MAMA = 20,
      _COLOR_FAMA = 30,
      _STYLE_MAMA = 40,
      _STYLE_FAMA = 50,
      _LABEL_MAMA = 60,
      _LABEL_FAMA = 70,
      _LIMIT_FAST = 80,
      _LIMIT_SLOW = 90,
      _STYLE_BARS = 100,
      _COLOR_BARS_UP = 110,
      _COLOR_BARS_DOWN = 120,
      _COLOR_BARS_NEUTRAL = 130
    };

    MAMA ();
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
