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

#include "FunctionFI.h"
#include "FunctionMA.h"

#include <QtDebug>

FunctionFI::FunctionFI ()
{
}

int FunctionFI::script (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,FI,<NAME>,<PERIOD>,<MA_TYPE>
  //     0       1    2     3      4         5 

  if (set.count() != 6)
  {
    qDebug() << "FunctionFI::script: invalid parm count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "FunctionFI::script: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int period = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionFI::script: invalid period" << set[4];
    return 1;
  }

  FunctionMA mau;
  int ma = mau.typeFromString(set[5]);
  if (ma == -1)
  {
    qDebug() << "FunctionFI::script: invalid ma" << set[5];
    return 1;
  }

  Curve *line = calculate(data, period, ma);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

Curve * FunctionFI::calculate (BarData &data, int period, int type)
{
  if (data.count() < period)
    return 0;
  
  Curve *line = new Curve;

  int loop = 1;
  double force = 0;
  for (; loop < (int) data.count(); loop++)
  {
    Bar bar = data.getBar(loop);
    Bar pbar = data.getBar(loop - 1);
    double cdiff = bar.getClose() - pbar.getClose();
    force = bar.getVolume() * cdiff;
  
    line->setBar(loop, new CurveBar(force));
  }

  if (period > 1)
  {
    FunctionMA mau;
    Curve *ma = mau.calculate(line, period, type);
    if (! ma)
    {
      delete line;
      return 0;
    }

    delete line;
    line = ma;
  }

  return line;
}

