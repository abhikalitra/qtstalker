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

#ifndef AD_HPP
#define AD_HPP

#include "IndicatorPlugin.h"

class AD : public IndicatorPlugin
{
  Q_OBJECT
  
  public:
    enum Method
    {
      _AD,
      _ADOSC
    };

    enum Parm
    {
      _Method = 10,
      _FastPeriod = 20,
      _SlowPeriod = 30,
      _ADColor = 40,
      _ADPlot = 50,
      _ADLabel = 60,
      _OSCColor = 70,
      _OSCPlot = 80,
      _OSCLabel = 90
    };

    AD ();
    int getIndicator (Indicator &ind, BarData &);
    int getCUS (QStringList &, Indicator &, BarData &);
    IndicatorPluginDialog * dialog (Indicator &);
    void defaults (Indicator &);
    void plotNames (Indicator &, QStringList &);
    int CUSAD (QStringList &, Indicator &, BarData &);
    int CUSADOSC (QStringList &, Indicator &, BarData &);
    Curve * getAD (BarData &);
    Curve * getADOSC (int fast, int slow, BarData &);

    QStringList & list();

  private:
    QStringList _methodList;
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}

#endif
