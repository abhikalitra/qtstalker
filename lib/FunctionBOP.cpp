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

#include "FunctionBOP.h"
#include "FunctionMA.h"
#include "ta_libc.h"

#include <QtDebug>

FunctionBOP::FunctionBOP ()
{
}

int FunctionBOP::script (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,BOP,<NAME>,<SMOOTHING_PERIOD>,<SMOOTHING_TYPE>
  //     0       1     2    3             4                 5 

  if (set.count() != 6)
  {
    qDebug() << "FunctionBOP::script: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "FunctionBOP::script: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int smoothing = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionBOP::script: invalid smoothing period" << set[4];
    return 1;
  }

  FunctionMA mau;
  int ma = mau.typeFromString(set[5]);
  if (ma == -1)
  {
    qDebug() << "FunctionBOP::script: invalid smoothing type" << set[5];
    return 1;
  }

  PlotLine *line = calculate(data, smoothing, ma);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

PlotLine * FunctionBOP::calculate (BarData &data, int smoothing, int type)
{
  if (data.count() < 1 || data.count() < smoothing)
    return 0;

  int size = data.count();
  TA_Real open[size];
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    Bar bar = data.getBar(loop);
    open[loop] = (TA_Real) bar.getOpen();
    high[loop] = (TA_Real) bar.getHigh();
    low[loop] = (TA_Real) bar.getLow();
    close[loop] = (TA_Real) bar.getClose();
  }

  TA_RetCode rc = TA_BOP(0,
                         size - 1,
                         &open[0],
                         &high[0],
                         &low[0],
                         &close[0],
                         &outBeg,
                         &outNb,
                         &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionBOP::calculate: TA-Lib error" << rc;
    return 0;
  }

  PlotLine *line = new PlotLine;

  for (loop = 0; loop < size; loop++)
    line->setData(loop, out[loop]);

  if (smoothing > 1)
  {
    FunctionMA mau;
    PlotLine *ma = mau.calculate(line, smoothing, type);
    delete line;
    line = ma;
  }

  return line;
}

