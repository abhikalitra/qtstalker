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

#include "FunctionSTOCH.h"
#include "FunctionMA.h"
#include "ta_libc.h"
#include "Globals.h"

#include <QtDebug>

FunctionSTOCH::FunctionSTOCH ()
{
}

int FunctionSTOCH::script (QStringList &set, Indicator &ind)
{
  // INDICATOR,PLUGIN,STOCH,<NAME FASTK>,<NAME FASTD>,<FASTK PERIOD>,<FASTD PERIOD>,<FASTD MA TYPE>
  //     0        1    2         3            4              5             6              7 

  if (set.count() != 8)
  {
    qDebug() << "FunctionSTOCH::script: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "FunctionSTOCH::script: duplicate fastk name" << set[3];
    return 1;
  }

  tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "FunctionSTOCH::script: duplicate fastd name" << set[4];
    return 1;
  }

  bool ok;
  int fkp = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionSTOCH::script: invalid fastk period" << set[5];
    return 1;
  }

  int fdp = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionSTOCH::script: invalid fastd period" << set[6];
    return 1;
  }

  FunctionMA mau;
  int ma = mau.typeFromString(set[7]);
  if (ma == -1)
  {
    qDebug() << "FunctionSTOCH::script: invalid fastd ma" << set[7];
    return 1;
  }

  QList<Curve *> pl;
  if (calculate(fkp, fdp, ma, pl))
    return 1;

  Curve *line = pl.at(0);
  line->setLabel(set[3]);
  ind.setLine(set[3], line);

  line = pl.at(1);
  line->setLabel(set[4]);
  ind.setLine(set[4], line);

  return 0;
}

int FunctionSTOCH::calculate (int kperiod, int dperiod, int ma, QList<Curve *> &pl)
{
  int size = g_barData.count();
  
  if (size < kperiod || size < dperiod)
    return 1;

  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TA_RetCode rc = TA_STOCHF(0,
                            size - 1,
                            g_barData.getTAData(BarData::High),
                            g_barData.getTAData(BarData::Low),
                            g_barData.getTAData(BarData::Close),
                            kperiod,
                            dperiod,
                            (TA_MAType) ma,
                            &outBeg,
                            &outNb,
                            &out[0],
                            &out2[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionSTOCH::calculate: TA-Lib error" << rc;
    return 1;
  }

  Curve *kline = new Curve;
  Curve *dline = new Curve;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    kline->setBar(dataLoop, new CurveBar(out[outLoop]));
    dline->setBar(dataLoop, new CurveBar(out2[outLoop]));
    
    dataLoop--;
    outLoop--;
  }

  pl.append(kline);
  pl.append(dline);

  return 0;
}

