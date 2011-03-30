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

#ifndef PLUGIN_STOCH_FAST_HPP
#define PLUGIN_STOCH_FAST_HPP

#include "Plugin.h"

class STOCH_FAST : public Plugin
{
  Q_OBJECT

  public:
    enum Parm
    {
      _COLOR_K = 10,
      _STYLE_K = 20,
      _LABEL_K = 30,
      _COLOR_D = 40,
      _STYLE_D = 50,
      _LABEL_D = 60,
      _PERIOD_FASTK = 70,
      _PERIOD_FASTD = 80,
      _COLOR_REF1 = 90,
      _COLOR_REF2 = 100,
      _REF1 = 110,
      _REF2 = 120,
      _MA_TYPE_FASTD = 130
    };

    STOCH_FAST ();
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
