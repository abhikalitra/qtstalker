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

#ifndef PP_HPP
#define PP_HPP

#include "IndicatorPlugin.h"

class PP : public IndicatorPlugin
{
  Q_OBJECT

  public:
    enum Parm
    {
      _R1Color = 10,
      _R2Color = 20,
      _R3Color = 30,
      _S1Color = 40,
      _S2Color = 50,
      _S3Color = 60,
      _R1Label = 70,
      _R2Label = 80,
      _R3Label = 90,
      _S1Label = 100,
      _S2Label = 110,
      _S3Label = 120,
      _R1Show = 130,
      _R2Show = 140,
      _R3Show = 150,
      _S1Show = 160,
      _S2Show = 170,
      _S3Show = 180
    };

    PP ();
    int getIndicator (Indicator &ind, BarData &);
    int getCUS (QStringList &, Indicator &, BarData &);
    IndicatorPluginDialog * dialog (Indicator &);
    void defaults (Indicator &);
    void plotNames (Indicator &, QStringList &);
    Curve * calculate (int point, BarData &);
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}

#endif
