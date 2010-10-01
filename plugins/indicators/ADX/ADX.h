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
    enum Method
    {
      _ADX,
      _ADXR,
      _PDI,
      _MDI,
      _DX
    };

    enum Parm
    {
      _ADXColor = 10,
      _ADXRColor = 20,
      _PDIColor = 30,
      _MDIColor = 40,
      _ADXPlot = 50,
      _ADXRPlot = 60,
      _PDIPlot = 70,
      _MDIPlot = 80,
      _ADXLabel = 90,
      _ADXRLabel = 100,
      _PDILabel = 110,
      _MDILabel = 120,
      _ADXCheck = 130,
      _ADXRCheck = 140,
      _PDICheck = 150,
      _MDICheck = 160,
      _Period = 170
    };

    ADX ();
    int getIndicator (Indicator &ind, BarData &);
    int getCUS (QStringList &, Indicator &, BarData &);
    IndicatorPluginDialog * dialog (Indicator &);
    void defaults (Indicator &);
    void plotNames (Indicator &, QStringList &);
    Curve * calculate (int period, int method, BarData &);

  protected:
    QStringList _methodList;
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}

#endif
