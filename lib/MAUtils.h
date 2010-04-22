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

#ifndef MA_UTILS_HPP
#define MA_UTILS_HPP

#include <QStringList>

#include "PlotLine.h"

class MAUtils
{
  public:
    enum Method
    {
      EMA,
      DEMA,
      KAMA,
      SMA,
      TEMA,
      TRIMA,
      Wilder,
      WMA
    };

    MAUtils ();
    void getMAList (QStringList &);
    PlotLine * getMA (PlotLine *, int, int);
    PlotLine * getDEMA(PlotLine *, int);
    PlotLine * getEMA(PlotLine *, int);
    PlotLine * getKAMA(PlotLine *, int);
    PlotLine * getSMA(PlotLine *, int);
    PlotLine * getTEMA(PlotLine *, int);
    PlotLine * getTRIMA(PlotLine *, int);
    PlotLine * getWilder(PlotLine *, int);
    PlotLine * getWMA(PlotLine *, int);
};

#endif
