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

#include "FunctionSAR.h"
#include "ta_libc.h"

#include <QtDebug>

FunctionSAR::FunctionSAR ()
{
}

int FunctionSAR::script (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,SAR,<NAME>,<INITIAL_STEP>,<MAX_STEP>
  //      0       1    2     3         4            5

  if (set.count() != 8)
  {
    qDebug() << "FunctionSAR::script: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "FunctionSAR::script: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  double init = set[4].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "FunctionSAR::script: invalid init" << set[4];
    return 1;
  }

  double max = set[5].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "FunctionSAR::script: invalid max" << set[5];
    return 1;
  }

  PlotLine *line = calculate(data, init, max);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

PlotLine * FunctionSAR::calculate (BarData &data, double _init, double _max)
{
  if (data.count() < 1)
    return 0;

  int size = data.count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real out[size];
  int loop;
  for (loop = 0; loop < size; loop++)
  {
    Bar bar = data.getBar(loop);
    high[loop] = (TA_Real) bar.getHigh();
    low[loop] = (TA_Real) bar.getLow();
  }

  TA_Integer outBeg;
  TA_Integer outNb;
  TA_RetCode rc = TA_SAR(0,
                         size - 1,
                         &high[0],
                         &low[0],
                         _init,
                         _max,
                         &outBeg,
                         &outNb,
                         &out[0]);
                         
  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionSAR::calculate: TA-Lib error" << rc;
    return 0;
  }

  PlotLine *line = new PlotLine;
  QString s("Dot");
  line->setType(s);

  for (loop = 0; loop < outNb; loop++)
    line->setData(loop + 1, out[loop]);

  return line;
}

