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

#ifndef BBANDS_HPP
#define BBANDS_HPP

#include "IndicatorPlugin.h"

#include <QList>

class BBANDS : public IndicatorPlugin
{
  Q_OBJECT

  public:
    enum Parm
    {
      _UpColor = 10,
      _DownColor = 20,
      _MidColor = 30,
      _UpPlot = 40,
      _DownPlot = 50,
      _MidPlot = 60,
      _UpLabel = 70,
      _DownLabel = 80,
      _MidLabel = 90,
      _Period = 100,
      _UpDeviation = 110,
      _DownDeviation = 120,
      _Input = 130,
      _MAType = 140,
      _BarsUpColor = 150,
      _BarsDownColor = 160,
      _BarsNeutralColor = 170,
      _BarsLabel = 180
    };

    BBANDS ();
    int getIndicator (Indicator &ind, BarData &);
    int getCUS (QStringList &, Indicator &, BarData &);
    IndicatorPluginDialog * dialog (Indicator &);
    void defaults (Indicator &);
    void plotNames (Indicator &, QStringList &);
    int calculate (Curve *in, int period, double udev, double ddev, int maType, QList<Curve *> &rl);
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}

#endif
