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

#include "FunctionBBANDS.h"
#include "FunctionMA.h"
#include "ta_libc.h"
#include "Globals.h"

#include <QtDebug>

FunctionBBANDS::FunctionBBANDS ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("FunctionBBANDS::error on TA_Initialize");
}

int FunctionBBANDS::script (QStringList &set, Indicator &ind)
{
  // INDICATOR,PLUGIN,BBANDS,<INPUT>,<NAME UPPER>,<NAME MIDDLE>,<NAME LOWER>,<PERIOD>,<MA_TYPE>,<UP DEV>,<LOW DEV>
  //     0       1      2       3         4             5            6          7         8        9        10

  if (set.count() != 11)
  {
    qDebug() << "FunctionBBANDS::script: invalid settings count" << set.count();
    return 1;
  }

  Curve *in = ind.line(set[3]);
  if (! in)
  {
    in = g_barData.getInput(g_barData.getInputType(set[3]));
    if (! in)
    {
      qDebug() << "FunctionBBANDS::script: input not found" << set[3];
      return 1;
    }

    ind.setLine(set[3], in);
  }

  Curve *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "FunctionBBANDS::script: duplicate upper name" << set[4];
    return 1;
  }

  tl = ind.line(set[5]);
  if (tl)
  {
    qDebug() << "FunctionBBANDS::script: duplicate middle name" << set[5];
    return 1;
  }

  tl = ind.line(set[6]);
  if (tl)
  {
    qDebug() << "FunctionBBANDS::script: duplicate lower name" << set[6];
    return 1;
  }

  bool ok;
  int period = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionBBANDS::script: invalid period" << set[7];
    return 1;
  }

  FunctionMA mau;
  int ma = mau.typeFromString(set[8]);
  if (ma == -1)
  {
    qDebug() << "FunctionBBANDS::script: invalid ma type" << set[8];
    return 1;
  }

  double udev = set[9].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "FunctionBBANDS::script: invalid upper deviation" << set[9];
    return 1;
  }

  double ldev = set[10].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "FunctionBBANDS::script: invalid lower deviation" << set[10];
    return 1;
  }

  QList<Curve *> pl;
  if (calculate(in, period, udev, ldev, ma, pl))
    return 1;
  
  pl.at(0)->setLabel(set[4]);
  pl.at(1)->setLabel(set[5]);
  pl.at(2)->setLabel(set[6]);

  ind.setLine(set[4], pl.at(0));
  ind.setLine(set[5], pl.at(1));
  ind.setLine(set[6], pl.at(2));

  return 0;
}

int FunctionBBANDS::calculate (Curve *in, int period, double udev, double ddev, int maType, QList<Curve *> &rl)
{
  if (in->count() < period)
    return 1;

  int size = in->count();  
  TA_Real input[size];
  TA_Real uout[size];
  TA_Real mout[size];
  TA_Real lout[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  QList<int> keys;
  in->keys(keys);
  
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_BBANDS(0,
                            keys.count() - 1,
                            &input[0],
                            period,
                            udev,
                            ddev,
                            (TA_MAType) maType,
                            &outBeg,
                            &outNb,
                            &uout[0],
                            &mout[0],
                            &lout[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionBBANDS::getBBANDS: TA-Lib error" << rc;
    return 1;
  }

  Curve *upper = new Curve;
  Curve *middle = new Curve;
  Curve *lower = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    upper->setBar(keys.at(keyLoop), new CurveBar(uout[outLoop]));
    middle->setBar(keys.at(keyLoop), new CurveBar(mout[outLoop]));
    lower->setBar(keys.at(keyLoop), new CurveBar(lout[outLoop]));

    keyLoop--;
    outLoop--;
  }

  rl.append(upper);
  rl.append(middle);
  rl.append(lower);
  
  return 0;
}

