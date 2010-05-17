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
#include "SMA.h"
#include "PlotLineBar.h"
#include "PlotFactory.h"

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
  // INDICATOR,PLUGIN,STDDEV,<NAME>,<INPUT>,<PERIOD>,<DEVIATION>,<LINE TYPE>,<COLOR>
  //     0      1       2      3       4       5         6            7         8

  if (set.count() != 9)
  {
    qDebug() << indicator << "::getCUS: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS: duplicate name" << set[3];
    return 1;
  }

  PlotLine *in = tlines.value(set[4]);
  if (! in)
  {
    in = data->getInput(data->getInputType(set[4]));
    if (! in)
    {
      qDebug() << indicator << "::getCUS: input not found" << set[4];
      return 1;
    }

    tlines.insert(set[4], in);
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid period" << set[5];
    return 1;
  }

  double dev= set[6].toDouble(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid deviation" << set[6];
    return 1;
  }

  PlotFactory fac;
  QStringList pl;
  fac.list(pl, 1);
  int lineType = pl.indexOf(set[7]);
  if (lineType == -1)
  {
    qDebug() << indicator << "::getCUS: invalid plot type" << set[7];
    return 1;
  }

  QColor color(set[8]);
  if (! color.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid color" << set[8];
    return 1;
  }

  PlotLine *line = stddev(in, period, dev, lineType, color);
  if (! line)
    return 1;

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * STDDEV::stddev (PlotLine *in, int period, double dev, int lineType, QColor &color)
{
  if (in->count() < period)
    return 0;

  SMA ma;
  PlotLine *sma = ma.sma(in, period, lineType, color);
  if (! sma)
    return 0;
  
  PlotFactory fac;
  PlotLine *line = fac.plot(lineType);
  if (! line)
  {
    delete sma;
    return 0;
  }

  QList<int> keys;
  in->keys(keys);

  int loop = period - 1;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar *smabar = sma->data(keys.at(loop));
    if (! smabar)
      continue;
    
    double total = 0;
    int count = 0;
    for (count = 0; count < period; count++)
    {
      PlotLineBar *bar = in->data(keys.at(loop - count));
      double t = bar->data() - smabar->data();
      total += t * t;
    }

    double var = total / (double) period;
    line->setData(keys.at(loop), new PlotLineBar(color, sqrt(var) * dev));
  }

  delete sma;
  
  return line;
}

