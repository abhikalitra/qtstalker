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

#include "IndicatorPlugin.h"

class MACD : public IndicatorPlugin
{
  Q_OBJECT

  public:
    enum Parm
    {
      MACDColor = 10,
      SignalColor = 20,
      HistColor = 30,
      MACDPlot = 40,
      SignalPlot = 50,
      HistPlot = 60,
      MACDLabel = 70,
      SignalLabel = 80,
      HistLabel = 90,
      FastPeriod = 100,
      SlowPeriod = 110,
      SignalPeriod = 120,
      FastMA = 130,
      SlowMA = 140,
      SignalMA = 150,
      Input = 160
    };

    MACD ();
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
