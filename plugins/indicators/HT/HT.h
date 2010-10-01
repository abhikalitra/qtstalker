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

#ifndef HT_HPP
#define HT_HPP

#include "IndicatorPlugin.h"

class HT : public IndicatorPlugin
{
  Q_OBJECT

  public:
    enum Method
    {
      _DCPERIOD,
      _DCPHASE,
      _TRENDLINE,
      _TRENDMODE,
      _PHASOR,
      _SINE
    };

    enum Parm
    {
      _Method = 10,
      _Input = 20,
      _Color = 30,
      _PhaseColor = 40,
      _QuadColor = 50,
      _SineColor = 60,
      _LeadColor = 70,
      _Plot = 80,
      _PhasePlot = 90,
      _QuadPlot = 100,
      _SinePlot = 110,
      _LeadPlot = 120,
      _Label = 130,
      _PhaseLabel = 140,
      _QuadLabel = 150,
      _SineLabel = 160,
      _LeadLabel = 170
    };

    HT ();
    int getIndicator (Indicator &ind, BarData &);
    int getCUS (QStringList &, Indicator &, BarData &);
    IndicatorPluginDialog * dialog (Indicator &);
    void defaults (Indicator &);
    void plotNames (Indicator &, QStringList &);
    
    int scriptPHASE (QStringList &set, Indicator &, BarData &);
    int scriptSINE (QStringList &set, Indicator &, BarData &);
    int scriptHT (QStringList &set, Indicator &, BarData &);
    Curve * getHT (Curve *in, int method);
    int getPHASE (Curve *in, QList<Curve *> &pl);
    int getSINE (Curve *in, QList<Curve *> &pl);
    QStringList & list ();

  protected:
    QStringList _methodList;
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}

#endif
