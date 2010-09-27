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

#ifndef CANDLES_HPP
#define CANDLES_HPP

#include "IndicatorPlugin.h"

class CANDLES : public IndicatorPlugin
{
  Q_OBJECT

  public:
    enum Parm
    {
      Method = 10,
      Penetration = 20,
      MethodColor = 30,
      Color = 40,
      Label = 50,
      MAColor = 60,
      MA2Color = 70,
      MA3Color = 80,
      MAPlot = 90,
      MA2Plot = 100,
      MA3Plot = 110,
      MALabel = 120,
      MA2Label = 130,
      MA3Label = 140,
      MAPeriod = 150,
      MA2Period = 160,
      MA3Period = 170,
      MAType = 180,
      MA2Type = 190,
      MA3Type = 200
    };

    CANDLES ();
    int getIndicator (Indicator &ind, BarData &);
    int getCUS (QStringList &, Indicator &, BarData &);
    IndicatorPluginDialog * dialog (Indicator &);
    void defaults (Indicator &);
    void plotNames (Indicator &, QStringList &);
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}

#endif
