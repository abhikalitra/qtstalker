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

#include "FunctionVIDYA.h"
#include "FunctionCMO.h"

#include <cmath>
#include <QVector>
#include <QtDebug>

#define PI 3.14159265

FunctionVIDYA::FunctionVIDYA ()
{
}

int FunctionVIDYA::script (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,VIDYA,<NAME>,<INPUT>,<PERIOD>,<VOLUME_PERIOD>
  //     0       1      2     3       4       5            6 

  if (set.count() != 7)
  {
    qDebug() << "FunctionVIDYA::script: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "FunctionVIDYA::script: duplicate name" << set[3];
    return 1;
  }

  PlotLine *inSignal = ind.line(set[4]);
  if (! inSignal)
  {
    inSignal = data.getInput(data.getInputType(set[4]));
    if (! inSignal)
    {
      qDebug() << "FunctionVIDYA::script: input not found" << set[4];
      return 1;
    }

    ind.setLine(set[4], inSignal);
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionVIDYA::script: invalid fast period settings" << set[5];
    return 1;
  }

  int volPeriod = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionVIDYA::script: invalid fast period settings" << set[6];
    return 1;
  }

  PlotLine *line = calculate(inSignal, period, volPeriod);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

PlotLine * FunctionVIDYA::calculate (PlotLine *inSignal, int period, int volPeriod)
{
  if (inSignal->count() < period || inSignal->count() < volPeriod)
    return 0;

  FunctionCMO f;
  PlotLine *cmo = f.calculate(inSignal, volPeriod);
  if (! cmo)
    return 0;

  PlotLine *out = new PlotLine;

  int size = inSignal->count();
  QVector<double> *inSeries = new QVector<double>(size);
  inSeries->fill(0.0);
  QVector<double> *offset = new QVector<double>(size);
  offset->fill(0.0);
  QVector<double> *absCmo = new QVector<double>(size);
  absCmo->fill(0.0);
  QVector<double> *vidya = new QVector<double>(size);
  vidya->fill(0.0);

  double c = 2 / (double) period + 1;

  QList<int> keys;
  inSignal->keys(keys);
  
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    PlotLineBar *bar = inSignal->data(keys.at(loop));
    (*inSeries)[loop] = bar->data();
  }

  keys.clear();
  cmo->keys(keys);

  int index = inSeries->size() -1;
  loop = keys.count() - 1;
  for (; loop > -1; loop--)
  {
    PlotLineBar *bar = cmo->data(keys.at(loop));
    (*absCmo)[index] = fabs(bar->data() / 100);
    index--;
  }

  loop = volPeriod + period;
  for (; loop < (int) inSeries->size(); loop++)		// period safty
  {
    (*vidya)[loop] = (inSeries->at(loop) * c * absCmo->at(loop)) + ((1 - absCmo->at(loop) * c) * vidya->at(loop - 1));
    //!  (Price*Const*AbsCMO) + ((1-AbsCMO*Const)*FunctionVIDYA[1]),Price);
    
    out->setData(loop, vidya->at(loop));
  }

  delete inSeries;
  delete offset;
  delete absCmo;
  delete vidya;
  delete cmo;

  return out;
}

