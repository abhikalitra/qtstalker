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

#include "FunctionWILLR.h"
#include "ta_libc.h"
#include "Globals.h"

#include <QtDebug>

FunctionWILLR::FunctionWILLR ()
{
}

int FunctionWILLR::script (QStringList &set, Indicator &ind)
{
  // INDICATOR,PLUGIN,WILLR,<NAME>,<PERIOD>
  //     0       1      2     3       4 

  if (set.count() != 5)
  {
    qDebug() << "FunctionWILLR::script: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "FunctionWILLR::script: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int period = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionWILLR::script: invalid period settings" << set[4];
    return 1;
  }

  Curve *line = calculate(period);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

Curve * FunctionWILLR::calculate (int period)
{
  int size = g_barData.count();

  if (size < period)
    return 0;

  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TA_RetCode rc = TA_WILLR(0,
                           size - 1,
                           g_barData.getTAData(BarData::High),
                           g_barData.getTAData(BarData::Low),
                           g_barData.getTAData(BarData::Close),
                           period,
                           &outBeg,
                           &outNb,
                           &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionWILLR::calculate: TA-Lib error" << rc;
    return 0;
  }

  Curve *line = new Curve;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    line->setBar(dataLoop, new CurveBar(out[outLoop]));
    dataLoop--;
    outLoop--;
  }

  return line;
}

