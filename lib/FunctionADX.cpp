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

#include "FunctionADX.h"
#include "ta_libc.h"

#include <QtDebug>

FunctionADX::FunctionADX ()
{
  _methodList << "ADX" << "ADXR" << "+DI" << "-DI" << "DX";
}

int FunctionADX::script (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,ADX,<METHOD>,<NAME>,<PERIOD>
  //     0       1     2     3       4       5

  if (set.count() != 6)
  {
    qDebug() << "FunctionADX::script: invalid settings count" << set.count();
    return 1;
  }

  int method = _methodList.indexOf(set.at(3));
  if (method == -1)
  {
    qDebug() << "FunctionADX::script: invalid method" << set.at(3);
    return 1;
  }
  
  Curve *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "FunctionADX::script: duplicate name" << set[4];
    return 1;
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionADX::script: invalid period" << set[5];
    return 1;
  }

  Curve *line = calculate(period, method, data);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}

Curve * FunctionADX::calculate (int period, int method, BarData &data)
{
  if (data.count() < period)
    return 0;
  
  int size = data.count();
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TA_RetCode rc = TA_SUCCESS;

  switch ((Method) method)
  {
    case _ADX:
      rc = TA_ADX(0,
                  size - 1,
                  data.getTAData(BarData::High),
                  data.getTAData(BarData::Low),
                  data.getTAData(BarData::Close),
                  period,
                  &outBeg,
                  &outNb,
                  &out[0]);
      break;
    case _ADXR:
      rc = TA_ADXR(0,
                   size - 1,
                   data.getTAData(BarData::High),
                   data.getTAData(BarData::Low),
                   data.getTAData(BarData::Close),
                   period,
                   &outBeg,
                   &outNb,
                   &out[0]);
      break;
    case _PDI:
      rc = TA_PLUS_DI(0,
                      size - 1,
                      data.getTAData(BarData::High),
                      data.getTAData(BarData::Low),
                      data.getTAData(BarData::Close),
                      period,
                      &outBeg,
                      &outNb,
                      &out[0]);
      break;
    case _MDI:
      rc = TA_MINUS_DI(0,
                       size - 1,
                       data.getTAData(BarData::High),
                       data.getTAData(BarData::Low),
                       data.getTAData(BarData::Close),
                       period,
                       &outBeg,
                       &outNb,
                       &out[0]);
      break;
    case _DX:
      rc = TA_DX(0,
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
    qDebug() << "FunctionADX::calculate: TA-Lib error" << rc;
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
