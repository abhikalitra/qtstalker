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

#ifndef AROON_HPP
#define AROON_HPP

#include "IndicatorPlugin.h"

#include <QList>

class AROON : public IndicatorPlugin
{
  public:
    enum Parm
    {
      Method,
      DownColor,
      UpColor,
      OSCColor,
      DownPlot,
      UpPlot,
      OSCPlot,
      DownLabel,
      UpLabel,
      OSCLabel,
      Period
    };

    enum _Method
    {
      Up,
      Down,
      OSC
    };

    AROON ();
    int getIndicator (Indicator &, BarData *);
    int getCUS (QStringList &, QHash<QString, PlotLine *> &, BarData *);
    int getCUSUP (QStringList &, QHash<QString, PlotLine *> &, BarData *);
    int getCUSDOWN (QStringList &, QHash<QString, PlotLine *> &, BarData *);
    int getCUSOSC (QStringList &, QHash<QString, PlotLine *> &, BarData *);
    PlotLine * getUP (BarData *, int);
    PlotLine * getDOWN (BarData *, int);
    PlotLine * getOSC (BarData *, int);
    int dialog (int);
    PlotLine * getAROON (BarData *data, int period, int method);

  private:
    QStringList methodList;
    QStringList guiMethodList;
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}

#endif
