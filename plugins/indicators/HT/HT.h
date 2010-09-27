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

#ifndef HT_HPP
#define HT_HPP

#include "IndicatorPlugin.h"

class HT : public IndicatorPlugin
{
  Q_OBJECT

  public:
    enum Parm
    {
      Method = 10,
      Input = 20,
      Color = 30,
      PhaseColor = 40,
      QuadColor = 50,
      SineColor = 60,
      LeadColor = 70,
      Plot = 80,
      PhasePlot = 90,
      QuadPlot = 100,
      SinePlot = 110,
      LeadPlot = 120,
      Label = 130,
      PhaseLabel = 140,
      QuadLabel = 150,
      SineLabel = 160,
      LeadLabel = 170
    };

    HT ();
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
