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

#include "FunctionULTOSC.h"
#include "ta_libc.h"

#include <QtDebug>

FunctionULTOSC::FunctionULTOSC ()
{
}

int FunctionULTOSC::script (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,ULTOSC,<NAME>,<SHORT PERIOD>,<MED PERIOD>,<LONG PERIOD>
  //     0       1       2     3          4             5             6

  if (set.count() != 7)
  {
    qDebug() << "FunctionULTOSC::script: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "FunctionULTOSC::script: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int sp = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionULTOSC::script: invalid short period" << set[4];
    return 1;
  }

  int mp = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionULTOSC::script: invalid med period" << set[5];
    return 1;
  }

  int lp = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionULTOSC::script: invalid long period" << set[6];
    return 1;
  }

  Curve *line = calculate(sp, mp, lp, data);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

Curve * FunctionULTOSC::calculate (int sp, int mp, int lp, BarData &data)
{
  int size = data.count();
  
  if (size < sp || size < mp || size < lp)
    return 0;

  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TA_RetCode rc = TA_ULTOSC(0,
                            size - 1,
                            data.getTAData(BarData::High),
                            data.getTAData(BarData::Low),
                            data.getTAData(BarData::Close),
                            sp,
                            mp,
                            lp,
                            &outBeg,
                            &outNb,
                            &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionULTOSC::calculate: TA-Lib error" << rc;
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

