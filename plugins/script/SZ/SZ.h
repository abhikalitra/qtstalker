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

#ifndef SZ_HPP
#define SZ_HPP

#include <QStringList>

#include "Plugin.h"
#include "Curve.h"

class SZ : public Plugin
{
  Q_OBJECT

  public:
    enum Method
    {
      _LONG,
      _SHORT
    };

    enum Parm
    {
      _COLOR = 10,
      _STYLE = 20,
      _LABEL = 30,
      _PERIOD = 40,
      _PERIOD_NO_DECLINE = 50,
      _COEFFICIENT = 60,
      _METHOD = 70,
      _COLOR_BARS_UP = 80,
      _COLOR_BARS_DOWN = 90,
      _COLOR_BARS_NEUTRAL = 100,
      _STYLE_BARS = 110
    };

    SZ ();
    int command (Command *);
    int calculate (BarData *, Indicator *);
    void defaults (Setting *);
    void dialog (QWidget *, Indicator *);
    QStringList method ();
    Curve * getSZ (Curve *ihigh, Curve *ilow, int type, int period, int ndp, double coeff);

  private:
    QStringList _method;
};

extern "C"
{
  Plugin * createPlugin ();
}

#endif
