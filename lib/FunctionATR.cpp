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

#include "FunctionATR.h"
#include "ta_libc.h"

#include <QtDebug>

FunctionATR::FunctionATR ()
{
  _methodList << "ATR" << "NATR";
}

int FunctionATR::script (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,ATR,<METHOD>,<NAME>,<PERIOD>
  //     0       1     2     3       4       5

  if (set.count() != 6)
  {
    qDebug() << "FunctionATR::script: invalid parm count" << set.count();
    return 1;
  }

  int method = _methodList.indexOf(set.at(3));
  if (method == -1)
  {
    qDebug() << "FunctionATR::script: invalid method" << set.at(4);
    return 1;
  }

  Curve *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "FunctionATR::script: duplicate name" << set[4];
    return 1;
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionATR::script: invalid period" << set[5];
    return 1;
  }

  Curve *line = calculate(period, method, data);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}

Curve * FunctionATR::calculate (int period, int method, BarData &data)
{
  int size = data.count();

  if (size < period)
    return 0;

  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TA_RetCode rc = TA_SUCCESS;

  switch ((Method) method)
  {
    case _ATR:
      rc = TA_ATR(0,
                  size - 1,
                  data.getTAData(BarData::High),
                  data.getTAData(BarData::Low),
                  data.getTAData(BarData::Close),
                  period,
                  &outBeg,
                  &outNb,
                  &out[0]);
      break;
    case _NATR:
      rc = TA_NATR(0,
                   size - 1,
                   data.getTAData(BarData::High),
                   data.getTAData(BarData::Low),
                   data.getTAData(BarData::Close),
                   period,
                   &outBeg,
                   &outNb,
                   &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionATR::calculate: TA-Lib error" << rc;
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

QStringList & FunctionATR::list ()
{
  return _methodList;
}

