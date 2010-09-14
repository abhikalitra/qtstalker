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

#include "FunctionAD.h"
#include "ta_libc.h"

#include <QtDebug>

FunctionAD::FunctionAD ()
{
  _methodList << "AD" << "ADOSC";
}

int FunctionAD::script (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,AD,<METHOD>,*
  //      0      1     2    3

  int method = _methodList.indexOf(set[3]);

  int rc = 1;
  switch ((Method) method)
  {
    case _AD:
      rc = scriptAD(set, ind, data);
      break;
    case _ADOSC:
      rc = scriptADOSC(set, ind, data);
      break;
    default:
      break;
  }

  return rc;
}

int FunctionAD::scriptAD (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,AD,AD,<NAME>
  //      0      1    2  3    4

  if (set.count() != 5)
  {
    qDebug() << "FunctionAD::scriptAD: invalid parm count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set.at(4));
  if (tl)
  {
    qDebug() << "FunctionAD::scriptAD: duplicate name" << set.at(4);
    return 1;
  }

  Curve *line = getAD(data);
  if (! line)
    return 1;

  line->setLabel(set[4]);
  
  ind.setLine(set.at(4), line);

  return 0;
}

int FunctionAD::scriptADOSC (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,AD,ADOSC,<NAME>,<FAST_PERIOD>,<SLOW_PERIOD>
  //      0      1    2    3      4        5             6

  if (set.count() != 7)
  {
    qDebug() <<  "FunctionAD::scriptADOSC: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set.at(4));
  if (tl)
  {
    qDebug() << "FunctionAD::scriptADOSC: duplicate name" << set.at(4);
    return 1;
  }

  bool ok;
  int fast = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionAD::scriptADOSC: invalid fast period" << set[5];
    return 1;
  }

  int slow = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionAD::scriptADOSC: invalid slow period" << set[6];
    return 1;
  }

  Curve *line = getADOSC(data, fast, slow);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set.at(4), line);

  return 0;
}

Curve * FunctionAD::getAD (BarData &data)
{
  if (data.count() < 1)
    return 0;

  int size = data.count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real volume[size];
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
    volume[loop] = (TA_Real) bar.getVolume();
  }

  TA_RetCode rc = TA_AD(0,
                        size - 1,
                        &high[0],
                        &low[0],
                        &close[0],
                        &volume[0],
                        &outBeg,
                        &outNb,
                        &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionAD::getAD: TA-Lib error" << rc;
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

Curve * FunctionAD::getADOSC (BarData &data, int fast, int slow)
{
  if (data.count() < 1)
    return 0;

  int size = data.count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real volume[size];
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
    volume[loop] = (TA_Real) bar.getVolume();
  }

  TA_RetCode rc = TA_ADOSC(0,
                           size - 1,
                           &high[0],
                           &low[0],
                           &close[0],
                           &volume[0],
                           fast,
                           slow,
                           &outBeg,
                           &outNb,
                           &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionAD::getADOSC: TA-Lib error" << rc;
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

QStringList & FunctionAD::list ()
{
  return _methodList;
}

