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
      _SlowKColor = 10,
      _SlowDColor = 20,
      _Ref1Color = 30,
      _Ref2Color = 40,
      _SlowKPlot = 50,
      _SlowDPlot = 60,
      _SlowKLabel = 70,
      _SlowDLabel = 80,
      _FastKPeriod = 90,
      _SlowKPeriod = 100,
      _SlowDPeriod = 110,
      _SlowKMA = 120,
      _SlowDMA = 130,
      _Ref1 = 140,
      _Ref2 = 150
    };

    STOCHS ();
    int getIndicator (Indicator &ind, BarData &);
    int getCUS (QStringList &, Indicator &, BarData &);
    IndicatorPluginDialog * dialog (Indicator &);
    void defaults (Indicator &);
    void plotNames (Indicator &, QStringList &);
    int calculate (int fkperiod, int skperiod, int sdperiod, int kma, int dma, QList<Curve *> &pl, BarData &);
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}

#endif
