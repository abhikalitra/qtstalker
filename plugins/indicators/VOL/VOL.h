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

#ifndef VOL_HPP
#define VOL_HPP

#include "IndicatorPlugin.h"
#include "Curve.h"

class VOL : public IndicatorPlugin
{
  Q_OBJECT

  public:
    enum Parm
    {
      UpColor = 10,
      DownColor = 20,
      NeutralColor = 30,
      MAColor = 40,
      Plot = 50,
      MAPlot = 60,
      Label = 70,
      MALabel = 80,
      MAPeriod = 90,
      MAType = 100
    };

    VOL ();
    int getIndicator (Indicator &ind, BarData &);
    Curve * getVOL (QColor &, QColor &, QColor &, BarData &);
    IndicatorPluginDialog * dialog (Indicator &);
    void defaults (Indicator &);
    void plotNames (Indicator &, QStringList &);
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}

#endif
