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

#include "FunctionMA.h"
#include "ta_libc.h"

#include <QtDebug>

FunctionMA::FunctionMA ()
{
  _maList << "EMA" << "DEMA" << "KAMA" << "SMA" << "TEMA" << "TRIMA" << "Wilder" << "WMA";
}

int FunctionMA::script (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,MA,<METHOD>,<NAME>,<INPUT>,<PERIOD>
  //     0       1    2     3       4       5       6

  if (set.count() != 7)
  {
    qDebug() << "FunctionMA::script: invalid parm count" << set.count();
    return 1;
  }

  int method = typeFromString(set[3]);
  if (method == -1)
  {
    qDebug() << "FunctionMA::script: invalid method" << set[3];
    return 1;
  }

  Curve *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "FunctionMA::script: duplicate name" << set[4];
    return 1;
  }

  Curve *in = ind.line(set[5]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[5]));
    if (! in)
    {
      qDebug() << "FunctionMA::script: input not found" << set[5];
      return 1;
    }

    ind.setLine(set[5], in);
  }

  bool ok;
  int period = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionMA::script: invalid period" << set[6];
    return 1;
  }

  Curve *line = calculate(in, period, method);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}

Curve * FunctionMA::calculate (Curve *in, int period, int method)
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

  TA_RetCode rc = TA_SUCCESS;

  switch ((Method) method)
  {
    case _EMA:
      rc = TA_EMA(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _DEMA:
      rc = TA_DEMA(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _KAMA:
      rc = TA_KAMA(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _SMA:
      rc = TA_SMA(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _TEMA:
      rc = TA_TEMA(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _TRIMA:
      rc = TA_TRIMA(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _Wilder:
      return getWilder(in, period);
      break;
    case _WMA:
      rc = TA_WMA(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    default:
      qDebug() << "FunctionMA::calculate: invalid method";
      return 0;
      break;
  }
      
  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionMA::calculate: TA-Lib error" << rc;
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

QStringList & FunctionMA::list ()
{
  return _maList;
}

int FunctionMA::typeFromString (QString &d)
{
  return _maList.indexOf(d);
}

Curve * FunctionMA::getWilder (Curve *in, int period)
{
  if (in->count() < period)
    return 0;

  Curve *line = new Curve;

  QList<int> keys;
  in->keys(keys);

  double t = 0;
  int loop = 0;
  for (; loop < period; loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    t += bar->data();
  }
  double yesterday = t / (double) period;
  line->setBar(keys.at(loop), new CurveBar(yesterday));

  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    double t  = (yesterday * (period - 1) + bar->data()) / (double) period;
    yesterday = t;

    line->setBar(keys.at(loop), new CurveBar(t));
  }

  return line;
}

