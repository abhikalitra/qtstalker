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

#ifndef MACD_HPP
#define MACD_HPP

#include "Plugin.h"

class MACD : public Plugin
{
  Q_OBJECT

  public:
    enum Parm
    {
      _INPUT = 10,
      _COLOR_MACD = 20,
      _COLOR_SIG = 30,
      _COLOR_HIST = 40,
      _STYLE_MACD = 50,
      _STYLE_SIG = 60,
      _STYLE_HIST = 70,
      _PERIOD_FAST = 80,
      _PERIOD_SLOW = 90,
      _PERIOD_SIG = 100,
      _MA_TYPE_FAST = 110,
      _MA_TYPE_SLOW = 120,
      _MA_TYPE_SIG = 130,
      _LABEL_MACD = 140,
      _LABEL_SIG = 150,
      _LABEL_HIST = 160
    };

    MACD ();
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
