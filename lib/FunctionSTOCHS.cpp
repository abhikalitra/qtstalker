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

#include "FunctionSTOCHS.h"
#include "FunctionMA.h"
#include "ta_libc.h"

#include <QtDebug>

FunctionSTOCHS::FunctionSTOCHS ()
{
}

int FunctionSTOCHS::script (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,STOCHS,<NAME SLOWK>,<NAME SLOWD>,<FASTK PERIOD>,<SLOWK PERIOD>,<SLOWK MA TYPE>,<SLOWD PERIOD>,<SLOWD MA TYPE>
  //     0       1      2         3            4             5              6               7              8              9 

  if (set.count() != 10)
  {
    qDebug() << "FunctionSTOCHS::script: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "FunctionSTOCHS::script: duplicate name" << set[3];
    return 1;
  }

  tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "FunctionSTOCHS::script: duplicate name" << set[4];
    return 1;
  }

  bool ok;
  int fkp = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionSTOCHS::script: invalid fastk period" << set[5];
    return 1;
  }

  int skp = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionSTOCHS::script: invalid slowk period" << set[6];
    return 1;
  }

  FunctionMA mau;
  int kma = mau.typeFromString(set[7]);
  if (kma == -1)
  {
    qDebug() << "FunctionSTOCHS::script: invalid slowk ma" << set[7];
    return 1;
  }

  int sdp = set[8].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionSTOCHS::script: invalid slowd period" << set[8];
    return 1;
  }

  int dma = mau.typeFromString(set[9]);
  if (dma == -1)
  {
    qDebug() << "FunctionSTOCHS::script: invalid slowd ma" << set[9];
    return 1;
  }

  QList<PlotLine *> pl;
  if (calculate(data, fkp, skp, sdp, kma, dma, pl))
    return 1;

  PlotLine *line = pl.at(0);
  line->setLabel(set[3]);
  ind.setLine(set[3], line);

  line = pl.at(1);
  line->setLabel(set[4]);
  ind.setLine(set[4], line);

  return 0;
}

int FunctionSTOCHS::calculate (BarData &data, int fkperiod, int skperiod, int sdperiod, int kma, int dma, QList<PlotLine *> &pl)
{
  if (data.count() < fkperiod || data.count() < skperiod)
    return 1;

  int size = data.count();
  TA_Real high[size];
  TA_Real low[size];
  TA_Real close[size];
  TA_Real out[size];
  TA_Real out2[size];
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

  TA_RetCode rc = TA_STOCH(0,
                           size - 1,
                           &high[0],
                           &low[0],
                           &close[0],
                           fkperiod,
                           skperiod,
                           (TA_MAType) kma,
                           sdperiod,
                           (TA_MAType) dma,
                           &outBeg,
                           &outNb,
                           &out[0],
                           &out2[0]);
                           
  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionSTOCHS::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *kline = new PlotLine;
  PlotLine *dline = new PlotLine;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    kline->setData(dataLoop, out[outLoop]);
    dline->setData(dataLoop, out2[outLoop]);
    
    dataLoop--;
    outLoop--;
  }

  pl.append(kline);
  pl.append(dline);

  return 0;
}

