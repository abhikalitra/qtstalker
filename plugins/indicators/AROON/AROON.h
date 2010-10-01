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
  Q_OBJECT

  public:
    enum Method
    {
      _AROON,
      _AROONOSC
    };

    enum Parm
    {
      _Method = 10,
      _DownColor = 20,
      _UpColor = 30,
      _OSCColor = 40,
      _DownPlot = 50,
      _UpPlot = 60,
      _OSCPlot = 70,
      _DownLabel = 80,
      _UpLabel = 90,
      _OSCLabel = 100,
      _Period = 110
    };

    AROON ();
    int getIndicator (Indicator &ind, BarData &);
    int getCUS (QStringList &, Indicator &, BarData &);
    IndicatorPluginDialog * dialog (Indicator &);
    void defaults (Indicator &);
    void plotNames (Indicator &, QStringList &);
    
    int scriptAROON (QStringList &, Indicator &, BarData &);
    int scriptAROONOSC (QStringList &, Indicator &, BarData &);
    int getAROON (int period, QList<Curve *> &, BarData &);
    Curve * getAROONOSC (int period, BarData &);
    QStringList & list ();

  protected:
    QStringList _methodList;
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}

#endif
