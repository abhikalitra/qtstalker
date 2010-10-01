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

#ifndef BARS_HPP
#define BARS_HPP

#include "IndicatorPlugin.h"

class BARS : public IndicatorPlugin
{
  Q_OBJECT

  public:
    enum Parm
    {
      _UpColor = 10,
      _DownColor = 20,
      _NeutralColor = 30,
      _BarsLabel = 40,
      _MAColor = 50,
      _MA2Color = 60,
      _MA3Color = 70,
      _MAPlot = 80,
      _MA2Plot = 90,
      _MA3Plot = 100,
      _MALabel = 110,
      _MA2Label = 120,
      _MA3Label = 130,
      _MAPeriod = 140,
      _MA2Period = 150,
      _MA3Period = 160,
      _MAType = 170,
      _MA2Type = 180,
      _MA3Type = 190
    };

    BARS ();
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
