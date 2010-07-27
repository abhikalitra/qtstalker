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

#include "FunctionMAMA.h"
#include "ta_libc.h"

#include <QtDebug>

FunctionMAMA::FunctionMAMA ()
{
}

int FunctionMAMA::script (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,MAMA,<INPUT>,<NAME_MAMA>,<NAME_FAMA>,<FAST_LIMIT>,<SLOW_LIMIT>
  //      0       1     2     3         4           5           6            7 

  if (set.count() != 8)
  {
    qDebug() << "FunctionMAMA::script: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *in = ind.line(set[3]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[3]));
    if (! in)
    {
      qDebug() << "FunctionMAMA::script: input not found" << set[3];
      return 1;
    }

    ind.setLine(set[3], in);
  }

  PlotLine *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "FunctionMAMA::script: mama duplicate name" << set[4];
    return 1;
  }

  tl = ind.line(set[5]);
  if (tl)
  {
    qDebug() << "FunctionMAMA::script: fama duplicate name" << set[5];
    return 1;
  }

  bool ok;
  double fast = set[6].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "FunctionMAMA::script: invalid fast limit" << set[6];
    return 1;
  }

  double slow = set[7].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "FunctionMAMA::script: invalid slow limit" << set[7];
    return 1;
  }

  QList<PlotLine *> l;
  if (calculate(in, fast, slow, l))
    return 1;

  l.at(0)->setLabel(set[4]);
  l.at(1)->setLabel(set[5]);

  ind.setLine(set[4], l.at(0));
  ind.setLine(set[5], l.at(1));

  return 0;
}

int FunctionMAMA::calculate (PlotLine *in, double fast, double slow, QList<PlotLine *> &l)
{
  if (in->count() < fast || in->count() < slow)
    return 1;

  int size = in->count();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];

  QList<int> keys;
  in->keys(keys);
  
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar *bar = in->data(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_MAMA(0,
                          size - 1,
                          &input[0],
                          fast,
                          slow,
                          &outBeg,
                          &outNb,
                          &out[0],
                          &out2[0]);
                                  
  if (rc != TA_SUCCESS)
  {
    qDebug() << "FunctionMAMA::calculate: TA-Lib error" << rc;
    return 1;
  }

  PlotLine *mama = new PlotLine;
  PlotLine *fama = new PlotLine;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    mama->setData(keys.at(keyLoop), out[outLoop]);
    fama->setData(keys.at(keyLoop), out2[outLoop]);
    
    keyLoop--;
    outLoop--;
  }

  l.append(mama);
  l.append(fama);

  return 0;
}

