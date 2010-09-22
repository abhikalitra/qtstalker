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
#include "Globals.h"

#include <QtDebug>

FunctionBOP::FunctionBOP ()
{
}

int FunctionBOP::script (QStringList &set, Indicator &ind)
{
  // INDICATOR,PLUGIN,BOP,<NAME>,<SMOOTHING_PERIOD>,<SMOOTHING_TYPE>
  //     0       1     2    3             4                 5 

  if (set.count() != 6)
  {
    qDebug() << "FunctionBOP::script: invalid parm count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
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

  Curve *line = calculate(smoothing, ma);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

Curve * FunctionBOP::calculate (int smoothing, int type)
{
  int size = g_barData.count();
  
  if (size < 1 || size < smoothing)
    return 0;

  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TA_RetCode rc = TA_BOP(0,
                         size - 1,
                         g_barData.getTAData(BarData::Open),
                         g_barData.getTAData(BarData::High),
                         g_barData.getTAData(BarData::Low),
                         g_barData.getTAData(BarData::Close),
                         &outBeg,
                         &outNb,
                         &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionBOP::calculate: TA-Lib error" << rc;
    return 0;
  }

  Curve *line = new Curve;

  int loop = 0;
  for (; loop < size; loop++)
    line->setBar(loop, new CurveBar(out[loop]));

  if (smoothing > 1)
  {
    FunctionMA mau;
    Curve *ma = mau.calculate(line, smoothing, type);
    delete line;
    line = ma;
  }

  return line;
}

