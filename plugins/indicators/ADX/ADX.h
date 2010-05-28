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
  public:
    enum Parm
    {
      ADXColor,
      ADXRColor,
      PDIColor,
      MDIColor,
      ADXPlot,
      ADXRPlot,
      PDIPlot,
      MDIPlot,
      ADXLabel,
      ADXRLabel,
      PDILabel,
      MDILabel,
      ADXCheck,
      ADXRCheck,
      PDICheck,
      MDICheck,
      Period
    };

    enum Method
    {
      _ADX,
      ADXR,
      PDI,
      MDI,
      DX
    };

    ADX ();
    int getIndicator (Indicator &ind, BarData *data);
    int getCUS (QStringList &, Indicator &, BarData *);
    int dialog (int);
    PlotLine * getLine (BarData *data, int period, int method, int lineType, QColor &color);

  protected:
    QStringList _methodList;
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}

#endif
