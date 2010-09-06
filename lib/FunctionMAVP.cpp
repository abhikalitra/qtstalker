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

#include "FunctionMAVP.h"
#include "ta_libc.h"
#include "FunctionMA.h"

#include <QtDebug>

FunctionMAVP::FunctionMAVP ()
{
}

int FunctionMAVP::script (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,MAVP,<NAME>,<INPUT_1>,<INPUT_2>,<MIN_PERIOD>,<MAX_PERIOD>,<MA_TYPE>
  //     0       1     2     3        4        5           6            7          8 

  if (set.count() != 9)
  {
    qDebug() << "FunctionMAVP::script: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "FunctionMAVP::script: duplicate name" << set[3];
    return 1;
  }

  Curve *in = ind.line(set[4]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[4]));
    if (! in)
    {
      qDebug() << "FunctionMAVP::script: input not found" << set[4];
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
      qDebug() << "FunctionMAVP::script: input not found" << set[5];
      return 1;
    }

    ind.setLine(set[5], in2);
  }

  bool ok;
  int min = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionMAVP::script: invalid min period settings" << set[6];
    return 1;
  }

  int max = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionMAVP::script: invalid max period settings" << set[7];
    return 1;
  }

  FunctionMA mau;
  int ma = mau.typeFromString(set[8]);
  if (ma == -1)
  {
    qDebug() << "FunctionMAVP::script: invalid ma settings" << set[8];
    return 1;
  }

  Curve *line = calculate(in, in2, min, max, ma);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

Curve * FunctionMAVP::calculate (Curve *in, Curve *in2, int min, int max, int ma)
{
  if (in->count() < min || in->count() < max)
    return 0;

  int flag = 0;
  int size = in->count();
  if (in2->count() < size)
  {
    size = in2->count();
    flag = 1;
  }

  QList<int> keys;
  in->keys(keys);
  QList<int> keys2;
  in2->keys(keys2);

  TA_Real input[size];
  TA_Real input2[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = keys.count() - 1;
  int loop2 = keys2.count() - 1;
  while (loop > -1 && loop2 > -1)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    CurveBar *bar2 = in2->bar(keys2.at(loop2));
    input[loop] = (TA_Real) bar->data();
    input2[loop2] = (TA_Real) bar2->data();

    loop--;
    loop2--;
  }

  TA_RetCode rc = TA_MAVP(0,
                          size - 1,
                          &input[0],
                          &input2[0],
                          min,
                          max,
                          (TA_MAType) ma,
                          &outBeg,
                          &outNb,
                          &out[0]);
                          
  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionMAVP::calculate: TA-Lib error" << rc;
    return 0;
  }

  Curve *line = new Curve;

  if (! flag)
  {
    int keyLoop = keys.count() - 1;
    int outLoop = outNb - 1;
    while (keyLoop > -1 && outLoop > -1)
    {
      line->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
      keyLoop--;
      outLoop--;
    }
  }
  else
  {
    int keyLoop = keys2.count() - 1;
    int outLoop = outNb - 1;
    while (keyLoop > -1 && outLoop > -1)
    {
      line->setBar(keys2.at(keyLoop), new CurveBar(out[outLoop]));
      keyLoop--;
      outLoop--;
    }
  }

  return line;
}

