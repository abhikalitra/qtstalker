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

#ifndef ADX_HPP
#define ADX_HPP

#include "IndicatorPlugin.h"

class ADX : public IndicatorPlugin
{
  Q_OBJECT

  public:
    enum Parm
    {
      ADXColor = 10,
      ADXRColor = 20,
      PDIColor = 30,
      MDIColor = 40,
      ADXPlot = 50,
      ADXRPlot = 60,
      PDIPlot = 70,
      MDIPlot = 80,
      ADXLabel = 90,
      ADXRLabel = 100,
      PDILabel = 110,
      MDILabel = 120,
      ADXCheck = 130,
      ADXRCheck = 140,
      PDICheck = 150,
      MDICheck = 160,
      Period = 170
    };

    ADX ();
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
