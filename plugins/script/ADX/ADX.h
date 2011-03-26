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

#ifndef ADX_HPP
#define ADX_HPP

#include "Plugin.h"

class ADX : public Plugin
{
  Q_OBJECT

  public:
    enum Parm
    {
      _ADX_COLOR = 10,
      _PDI_COLOR = 20,
      _MDI_COLOR = 30,
      _ADX_STYLE = 40,
      _PDI_STYLE = 50,
      _MDI_STYLE = 60,
      _ADX_LABEL = 70,
      _PDI_LABEL = 80,
      _MDI_LABEL = 90,
      _PERIOD = 100,
      _ADXR_COLOR = 110,
      _ADXR_STYLE = 120,
      _ADXR_LABEL = 130
    };

    ADX ();
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
