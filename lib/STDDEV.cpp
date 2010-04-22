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

#include "STDDEV.h"
#include "MAUtils.h"

#include <QtDebug>
#include <QObject>
#include <cmath>


STDDEV::STDDEV ()
{
  indicator = "STDDEV";
  deleteFlag = TRUE;
}

int STDDEV::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,STDDEV,METHOD,<NAME>,<INPUT>,<PERIOD>,<DEVIATION>
  //     0      1       2     3       4     5        6          7

  if (set.count() != 8)
  {
    qDebug() << indicator << "::getCUS: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[4]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS: duplicate name" << set[4];
    return 1;
  }

  PlotLine *in = tlines.value(set[5]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[5]));
    if (! in)
    {
      qDebug() << indicator << "::getCUS: input not found" << set[5];
      return 1;
    }

    tlines.insert(set[5], in);
  }

  bool ok;
  int period = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid period" << set[6];
    return 1;
  }

  double dev= set[7].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid deviation" << set[7];
    return 1;
  }

  PlotLine *line = getSTDDEV(in, period, dev);
  if (! line)
    return 1;

  tlines.insert(set[4], line);

  return 0;
}

PlotLine * STDDEV::getSTDDEV (PlotLine *in, int period, double dev)
{
  if (in->count() < period)
    return 0;

  MAUtils mau;
  PlotLine *sma = mau.getMA(in, period, MAUtils::SMA);
  if (! sma)
    return 0;
  
  PlotLine *line = new PlotLine;
  int inLoop = in->count() - 1;
  int smaLoop = sma->count() - 1;
  int start = period - 1;

  while (inLoop >= start && smaLoop > -1)
  {
    double total = 0;
    int count = 0;
    for (count = 0; count < period; count++)
    {
      double t = in->getData(inLoop - count) - sma->getData(smaLoop);
      total += t * t;
    }

    double var = total / (double) period;
    line->prepend(sqrt(var) * dev);

    inLoop--;
    smaLoop--;
  }

  delete sma;
  
  return line;
}

