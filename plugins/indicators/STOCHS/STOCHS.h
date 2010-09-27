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

#ifndef STOCHS_HPP
#define STOCHS_HPP

#include "IndicatorPlugin.h"

class STOCHS : public IndicatorPlugin
{
  Q_OBJECT

  public:
    enum Parm
    {
      SlowKColor = 10,
      SlowDColor = 20,
      Ref1Color = 30,
      Ref2Color = 40,
      SlowKPlot = 50,
      SlowDPlot = 60,
      SlowKLabel = 70,
      SlowDLabel = 80,
      FastKPeriod = 90,
      SlowKPeriod = 100,
      SlowDPeriod = 110,
      SlowKMA = 120,
      SlowDMA = 130,
      Ref1 = 140,
      Ref2 = 150
    };

    STOCHS ();
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
