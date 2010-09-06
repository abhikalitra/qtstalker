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

#include "FunctionCCI.h"
#include "FunctionMA.h"
#include "ta_libc.h"

#include <QtDebug>
#include <cmath>

FunctionCCI::FunctionCCI ()
{
}

int FunctionCCI::script (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,CCI,<NAME>,<PERIOD>,<SMOOTHING_PERIOD>,<SMOOTHING_TYPE>
  //     0       1     2    3       4             5                  6 

  if (set.count() != 7)
  {
    qDebug() << "FunctionCCI::script: invalid parm count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "FunctionCCI::script: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int period = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionCCI::script: invalid period parm" << set[4];
    return 1;
  }

  int smoothing = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionCCI::script: invalid smoothing period" << set[5];
    return 1;
  }

  FunctionMA mau;
  int ma = mau.typeFromString(set[6]);
  if (ma == -1)
  {
    qDebug() << "FunctionCCI::script: invalid smoothing type" << set[6];
    return 1;
  }

  Curve *line = calculate(data, period, smoothing, ma);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

Curve * FunctionCCI::calculate (BarData &data, int period, int smoothing, int type)
{
  int size = data.count();
  if (size < period || size < smoothing)
    return 0;

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
    high[loop] = (TA_Real) bar.getHigh();
    low[loop] = (TA_Real) bar.getLow();
    close[loop] = (TA_Real) bar.getClose();
  }

  TA_RetCode rc = TA_CCI(0,
                         size - 1,
                         &high[0],
                         &low[0],
                         &close[0],
                         period,
                         &outBeg,
                         &outNb,
                         &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionCCI::calculate: TA-Lib error" << rc;
    return 0;
  }

  Curve *cci = new Curve;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (dataLoop > -1 && outLoop > -1)
  {
    cci->setBar(dataLoop, new CurveBar(out[outLoop]));
    dataLoop--;
    outLoop--;
  }

  if (smoothing > 1)
  {
    FunctionMA maf;
    Curve *ma = maf.calculate(cci, smoothing, type);
    delete cci;
    cci = ma;
  }

  return cci;
}

