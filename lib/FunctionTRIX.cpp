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

#include "FunctionTRIX.h"
#include "ta_libc.h"
#include "Globals.h"

#include <QtDebug>

FunctionTRIX::FunctionTRIX ()
{
}

int FunctionTRIX::script (QStringList &set, Indicator &ind)
{
  // INDICATOR,PLUGIN,TRIX,<NAME>,<INPUT>,<PERIOD>
  //     0       1     2     3       4       5 

  if (set.count() != 6)
  {
    qDebug() << "FunctionTRIX::script: invalid parm count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "FunctionTRIX::script: duplicate name" << set[3];
    return 1;
  }

  Curve *in = ind.line(set[4]);
  if (! in)
  {
    in = g_barData.getInput(g_barData.getInputType(set[4]));
    if (! in)
    {
      qDebug() << "FunctionTRIX::script: input not found" << set[4];
      return 1;
    }

    ind.setLine(set[4], in);
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionTRIX::script: invalid period parm" << set[5];
    return 1;
  }

  Curve *line = calculate(in, period);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

Curve * FunctionTRIX::calculate (Curve *in, int period)
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
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_TRIX(0,
                          size - 1,
                          &input[0],
                          period,
                          &outBeg,
                          &outNb,
                          &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionTRIX::calculate: TA-Lib error" << rc;
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

