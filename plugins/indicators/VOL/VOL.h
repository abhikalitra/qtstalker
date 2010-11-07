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
      _UpColor = 10,
      _DownColor = 20,
      _NeutralColor = 30,
      _MAColor = 40,
      _Plot = 50,
      _MAPlot = 60,
      _Label = 70,
      _MALabel = 80,
      _MAPeriod = 90,
      _MAType = 100
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
