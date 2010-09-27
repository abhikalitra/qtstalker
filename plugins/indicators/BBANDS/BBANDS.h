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
      UpColor = 10,
      DownColor = 20,
      MidColor = 30,
      UpPlot = 40,
      DownPlot = 50,
      MidPlot = 60,
      UpLabel = 70,
      DownLabel = 80,
      MidLabel = 90,
      Period = 100,
      UpDeviation = 110,
      DownDeviation = 120,
      Input = 130,
      MAType = 140,
      BarsUpColor = 150,
      BarsDownColor = 160,
      BarsNeutralColor = 170,
      BarsLabel = 180
    };

    BBANDS ();
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
