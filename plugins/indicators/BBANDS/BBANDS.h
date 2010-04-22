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
  public:
    enum Parm
    {
      UpColor,
      DownColor,
      MidColor,
      UpPlot,
      DownPlot,
      MidPlot,
      UpLabel,
      DownLabel,
      MidLabel,
      Period,
      UpDeviation,
      DownDeviation,
      Input,
      MAType
    };

    enum Method
    {
      Upper,
      Middle,
      Lower
    };

    BBANDS ();
    int getIndicator (Indicator &ind, BarData *data);
    int getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data);
    int dialog (int);

    int getCUSUpper (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data);
    int getCUSMiddle (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data);
    int getCUSLower (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data);
    PlotLine * getUpper (PlotLine *, PlotLine *, int, double);
    PlotLine * getLower (PlotLine *, PlotLine *, int, double);
    PlotLine * getMiddle (PlotLine *, int, int);

  private:
    QStringList methodList;
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}

#endif
