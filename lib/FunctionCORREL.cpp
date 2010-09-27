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

#include "FunctionCORREL.h"
#include "ta_libc.h"

#include <QtDebug>

FunctionCORREL::FunctionCORREL ()
{
}

int FunctionCORREL::script (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,CORREL,<NAME>,<INPUT_1>,<INPUT_2>,<PERIOD>
  //      0      1       2     3        4        5          6 

  if (set.count() != 7)
  {
    qDebug() << "FunctionCORREL::script: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "FunctionCORREL::script: duplicate name" << set[3];
    return 1;
  }

  Curve *in = ind.line(set[4]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[4]));
    if (! in)
    {
      qDebug() << "FunctionCORREL::script: input not found" << set[4];
      return 1;
    }

    ind.setLine(set[4], in);
  }

  Curve *in2 = ind.line(set[5]);
  if (! in2)
  {
    in2 = data.getInput(data.getInputType(set[5]));
    if (! in2)
    {
      qDebug() << "FunctionCORREL::script: input not found" << set[5];
      return 1;
    }

    ind.setLine(set[5], in2);
  }

  bool ok;
  int period = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionCORREL::script: invalid period settings" << set[6];
    return 1;
  }

  Curve *line = calculate(in, in2, period);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

Curve * FunctionCORREL::calculate (Curve *in, Curve *in2, int period)
{
  if (in->count() < period || in2->count() < period)
    return 0;

  QList<int> keys;
  int size = in->count();
  if (in2->count() < size)
  {
    size = in2->count();
    in2->keys(keys);
  }
  else
    in->keys(keys);

  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    if (! bar)
      continue;

    CurveBar *bar2 = in2->bar(keys.at(loop));
    if (! bar2)
      continue;

    input[loop] = (TA_Real) bar->data();
    input2[loop] = (TA_Real) bar2->data();
  }

  TA_RetCode rc = TA_CORREL(0,
                            size - 1,
                            &input[0],
                            &input2[0],
                            period,
                            &outBeg,
                            &outNb,
                            &out[0]);
                            
  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionCORREL::calculate: TA-Lib error" << rc;
    return 0;
  }

  Curve *line = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    line->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    keyLoop--;
    outLoop--;
  }

  return line;
}

