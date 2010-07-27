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

#include "FunctionT3.h"
#include "ta_libc.h"

#include <QtDebug>

FunctionT3::FunctionT3 ()
{
}

int FunctionT3::script (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,T3,<NAME>,<INPUT>,<PERIOD>,<VFACTOR>
  //     0       1    2    3       4       5         6 

  if (set.count() != 7)
  {
    qDebug() << "FunctionT3::script: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "FunctionT3::script: duplicate name" << set[3];
    return 1;
  }

  PlotLine *in = ind.line(set[4]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[4]));
    if (! in)
    {
      qDebug() << "FunctionT3::script: input not found" << set[4];
      return 1;
    }

    ind.setLine(set[4], in);
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionT3::script: invalid period settings" << set[5];
    return 1;
  }

  double vfactor = set[6].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "FunctionT3::script: invalid vfactor" << set[6];
    return 1;
  }

  PlotLine *line = calculate(in, period, vfactor);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

PlotLine * FunctionT3::calculate (PlotLine *in, int period, double vfactor)
{
  if (in->count() < period)
    return 0;

  QList<int> keys;
  in->keys(keys);
  int size = keys.count();

  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    PlotLineBar *bar = in->data(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_T3(0,
                        size - 1,
                        &input[0],
                        period,
                        vfactor,
                        &outBeg,
                        &outNb,
                        &out[0]);
                        
  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionT3::calculate: TA-Lib error" << rc;
    return 0;
  }

  PlotLine *line = new PlotLine;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    line->setData(keys.at(keyLoop), out[outLoop]);
    keyLoop--;
    outLoop--;
  }

  return line;
}

