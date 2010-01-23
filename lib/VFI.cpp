/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "VFI.h"

#include <QtDebug>
#include <math.h>


VFI::VFI ()
{
}

int VFI::calculate (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,VFI,<NAME>,<PERIOD>

  if (set.count() != 4)
  {
    qDebug() << "VFI::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << set[1] << "::calculate: duplicate name" << set[2];
    return 1;
  }

  bool ok;
  int period = set[3].toInt(&ok);
  if (! ok)
  {
    qDebug() << "VFI::calculate: invalid period" << set[3];
    return 1;
  }

  PlotLine *vfi = new PlotLine();
  int loop;
  for (loop = period; loop < (int) data->count(); loop++)
  {
    double inter = 0.0;
    double sma_vol = 0.0;
    int i;
    double close = data->getClose(loop-period);
    double high = data->getHigh(loop-period);
    double low = data->getLow(loop-period);
    double typical = (high+low+close) / 3.0;
    for (i = loop - period + 1; i <= loop; i++)
    {
      double ytypical = typical;
      close = data->getClose(i);
      high = data->getHigh(i);
      low = data->getLow(i);
      typical = (high + low + close) / 3.0;
      double delta = (log(typical) - log(ytypical));
      inter += delta * delta;
      sma_vol += data->getVolume(i);
    }
    inter = 0.2 * sqrt(inter / (double) period) * typical;
    sma_vol /= (double) period;

    close = data->getClose(loop - period);
    high = data->getHigh(loop - period);
    low = data->getLow(loop - period);
    typical = (high + low + close) / 3.0;
    double t = 0;
    for (i = loop - period + 1; i <= loop; i++)
    {
      double ytypical = typical;
      double volume = data->getVolume(i);
      close = data->getClose(i);
      high = data->getHigh(i);
      low = data->getLow(i);
      typical = (high + low + close) / 3.0;

      if (typical > ytypical + inter)
        t = t + log (1.0 + volume / sma_vol);
      else
      {
        if (typical < ytypical - inter)
          t = t - log (1.0 + volume / sma_vol);
      }
    }

    vfi->append(t);
  }

  tlines.insert(set[2], vfi);

  return 0;
}

