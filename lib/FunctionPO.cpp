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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURFunctionPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#include "FunctionPO.h"
#include "FunctionMA.h"
#include "ta_libc.h"
#include "Globals.h"

#include <QtDebug>

FunctionPO::FunctionPO ()
{
  _methodList << "APO" << "PPO";
}

int FunctionPO::script (QStringList &set, Indicator &ind)
{
  // INDICATOR,PLUGIN,PO,<METHOD>,<NAME>,<INPUT>,<FAST_PERIOD>,<SLOW_PERIOD>,<MA_TYPE>
  //     0       1     2    3       4       5         6             7           8 

  if (set.count() != 9)
  {
    qDebug() << "FunctionPO::script: invalid parm count" << set.count();
    return 1;
  }

  int method = _methodList.indexOf(set[3]);
  if (method == -1)
  {
    qDebug() << "FunctionPO::script: invalid method" << set[3];
    return 1;
  }

  Curve *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "FunctionPO::script: duplicate name" << set[4];
    return 1;
  }

  Curve *in = ind.line(set[5]);
  if (! in)
  {
    in = g_barData.getInput(g_barData.getInputType(set[5]));
    if (! in)
    {
      qDebug() << "FunctionPO::script: input not found" << set[5];
      return 1;
    }

    ind.setLine(set[5], in);
  }

  bool ok;
  int fast = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionPO::script: invalid fast period" << set[6];
    return 1;
  }

  int slow = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionPO::script: invalid slow period" << set[7];
    return 1;
  }

  FunctionMA mau;
  int ma = mau.typeFromString(set[8]);
  if (ma == -1)
  {
    qDebug() << "FunctionPO::script: invalid ma" << set[8];
    return 1;
  }

  Curve *line = calculate(in, fast, slow, ma, method);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}

Curve * FunctionPO::calculate (Curve *in, int fast, int slow, int ma, int method)
{
  if (in->count() < fast || in->count() < slow)
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

  TA_RetCode rc = TA_SUCCESS;

  switch ((Method) method)
  {
    case _APO:
      rc = TA_APO(0, size - 1, &input[0], fast, slow, (TA_MAType) ma, &outBeg, &outNb, &out[0]);
      break;
    case _PPO:
      rc = TA_PPO(0, size - 1, &input[0], fast, slow, (TA_MAType) ma, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionPO::calculate: TA-Lib error" << rc;
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

QStringList & FunctionPO::list ()
{
  return _methodList;
}

