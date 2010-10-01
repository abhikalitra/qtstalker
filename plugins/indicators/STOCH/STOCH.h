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

#ifndef STOCH_HPP
#define STOCH_HPP

#include "IndicatorPlugin.h"

class STOCH : public IndicatorPlugin
{
  Q_OBJECT

  public:
    enum Parm
    {
      _FastKColor = 10,
      _FastDColor = 20,
      _Ref1Color = 30,
      _Ref2Color = 40,
      _FastKPlot = 50,
      _FastDPlot = 60,
      _FastKLabel = 70,
      _FastDLabel = 80,
      _FastKPeriod = 90,
      _FastDPeriod = 100,
      _FastDMA = 110,
      _Ref1 = 120,
      _Ref2 = 130
    };

    STOCH ();
    int getIndicator (Indicator &ind, BarData &);
    int getCUS (QStringList &, Indicator &, BarData &);
    IndicatorPluginDialog * dialog (Indicator &);
    void defaults (Indicator &);
    void plotNames (Indicator &, QStringList &);
    int calculate (int kperiod, int dperiod, int ma, QList<Curve *> &pl, BarData &);
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}

#endif
