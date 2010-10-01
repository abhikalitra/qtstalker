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

#ifndef ULTOSC_HPP
#define ULTOSC_HPP

#include "IndicatorPlugin.h"

class ULTOSC : public IndicatorPlugin
{
  Q_OBJECT

  public:
    enum Parm
    {
      _Color = 10,
      _Plot = 20,
      _Label = 30,
      _ShortPeriod = 40,
      _MidPeriod = 50,
      _LongPeriod = 60,
      _Ref1Color = 70,
      _Ref2Color = 80,
      _Ref3Color = 90,
      _Ref1 = 100,
      _Ref2 = 110,
      _Ref3 = 120
    };

    ULTOSC ();
    int getIndicator (Indicator &ind, BarData &);
    int getCUS (QStringList &, Indicator &, BarData &);
    IndicatorPluginDialog * dialog (Indicator &);
    void defaults (Indicator &);
    void plotNames (Indicator &, QStringList &);
    Curve * calculate (int sp, int mp, int lp, BarData &);
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}

#endif
