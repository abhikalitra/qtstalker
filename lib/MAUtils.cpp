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

#include "MAUtils.h"

#include <QtDebug>
#include <cmath>

MAUtils::MAUtils ()
{
}

void MAUtils::getMAList (QStringList &l)
{
  l.clear();
  l << "EMA";
  l << "DEMA";
  l << "KAMA";
  l << "SMA";
  l << "TEMA";
  l << "TRIMA";
  l << "Wilder";
  l << "WMA";
}

PlotLine * MAUtils::getMA (PlotLine *in, int period, int method)
{
  PlotLine *line = 0;
  
  switch ((Method) method)
  {
    case EMA:
      line = getEMA(in, period);
    case DEMA:
      line = getDEMA(in, period);
    case KAMA:
      line = getKAMA(in, period);
    case SMA:
      line = getSMA(in, period);
    case TEMA:
      line = getTEMA(in, period);
    case TRIMA:
      line = getTRIMA(in, period);
    case Wilder:
      line = getWilder(in, period);
    case WMA:
      line = getWMA(in, period);
    default:
      break;
  }

  return line;
}

PlotLine * MAUtils::getDEMA (PlotLine *in, int period)
{
  PlotLine *ema1 = getEMA(in, period);
  if (! ema1)
    return 0;

  PlotLine *ema2 = getEMA(ema1, period);
  if (! ema2)
  {
    delete ema1;
    return 0;
  }

  PlotLine *line = new PlotLine;

  int ema1Loop = ema1->count() - 1;  
  int ema2Loop = ema2->count() - 1;

  while (ema1Loop > -1 && ema2Loop > -1)
  {
    line->prepend((ema1->getData(ema1Loop) * 2) - ema2->getData(ema2Loop));
    ema1Loop--;
    ema2Loop--;
  }

  delete ema1;
  delete ema2;
  
  return line;
}

PlotLine * MAUtils::getEMA (PlotLine *in, int period)
{
  if (in->count() < period)
    return 0;

  // first calculate the expo seed
  // we just calculate an SMA
  PlotLine *line = new PlotLine;
  int size = in->count();
  int loop = period - 1;
  double total = 0;
  int count = loop;
  for (; count > -1; count--)
    total = total + in->getData(loop - count);
  double prevMA = total / (double) period;
  line->append(prevMA);

  // now calculate the EMA proper
  double multiplier = 2.0 / (double) (period + 1);
  loop++;
  for (; loop < size; loop++)
  {
    double t = ((in->getData(loop) - prevMA) * multiplier) + prevMA;
    prevMA = t;
    line->append(t);
  }
  
  return line;
}

PlotLine * MAUtils::getKAMA (PlotLine *in, int period)
{
  if (in->count() < period)
    return 0;

  PlotLine *line = new PlotLine;
  int size = in->count();
  double fast = 0.6667;
  double slow = 0.0645;
  int loop = period;
  double prevAMA = in->getData(loop);
  
  for (; loop < size; loop++)
  {
    // get direction
    double direction = in->getData(loop) - in->getData(loop - period);

    // get the er
    int count = 0;
    int loop2 = loop;
    double t = 0;
    for (; count < period; count++, loop2--)
      t = t + fabs(in->getData(loop2) - in->getData(loop2 - 1));
    double er = fabs(direction / t);
    
    double ssc = er * (fast - slow) + slow;
    double c = ssc * ssc;

    double ama = c * (in->getData(loop) - prevAMA) + prevAMA;
    prevAMA = ama;
    line->append(ama);
  }

  return line;
}

PlotLine * MAUtils::getSMA (PlotLine *in, int period)
{
  if (in->count() < period)
    return 0;

  PlotLine *line = new PlotLine;
  int size = in->count();
  int loop = period - 1;
  for (; loop < size; loop++)
  {
    int count = period - 1;
    double total = 0;
    for (; count > -1; count--)
      total = total + in->getData(loop - count);
    line->append(total / (double) period);
  }
  
  return line;
}

PlotLine * MAUtils::getTEMA (PlotLine *in, int period)
{
  PlotLine *ema1 = getEMA(in, period);
  if (! ema1)
    return 0;

  PlotLine *ema2 = getEMA(ema1, period);
  if (! ema2)
  {
    delete ema1;
    return 0;
  }

  PlotLine *ema3 = getEMA(ema2, period);
  if (! ema3)
  {
    delete ema1;
    delete ema2;
    return 0;
  }

  PlotLine *line = new PlotLine;

  int ema1Loop = ema1->count() - 1;
  int ema2Loop = ema2->count() - 1;
  int ema3Loop = ema3->count() - 1;

  while (ema1Loop > -1 && ema2Loop > -1 && ema3Loop > -1)
  {
    line->prepend((ema1->getData(ema1Loop) * 3) - (ema2->getData(ema2Loop) * 3) + ema3->getData(ema3Loop));
    ema1Loop--;
    ema2Loop--;
    ema3Loop--;
  }

  delete ema1;
  delete ema2;
  delete ema3;

  return line;
}

PlotLine * MAUtils::getTRIMA (PlotLine *in, int period)
{
  if (in->count() < period)
    return 0;

  int period1 = 0;
  int period2 = 0;
  if (period % 2 == 0)
  {
    // even period
    period1 = period / 2;
    period2 = period1;
    period2++;
  }
  else
  {
    // ood
    period1 = (period / 2) + 1;
    period2 = period1;
  }

  PlotLine *sma1 = getSMA(in, period1);
  if (! sma1)
    return 0;

  PlotLine *sma2 = getSMA(sma1, period2);
  if (! sma2)
  {
    delete sma1;
    return 0;
  }

  delete sma1;
  return sma2;
}

PlotLine * MAUtils::getWilder (PlotLine *in, int period)
{
  if (in->count() < period)
    return 0;

  PlotLine *line = new PlotLine;
  int size = in->count();
  double t = 0;
  int loop;
  for (loop = 0; loop < period; loop++)
    t = t + in->getData(loop);
  double yesterday = t / (double) period;
  line->append(yesterday);
  
  for (; loop < size; loop++)
  {
    double t  = (yesterday * (period - 1) + in->getData(loop)) / (double) period;
    yesterday = t;
    line->append(t);
  }
  
  return line;
}

PlotLine * MAUtils::getWMA (PlotLine *in, int period)
{
  if (in->count() < period)
    return 0;

  PlotLine *line = new PlotLine;
  int size = in->count();
  int loop = period - 1;
  for (; loop < size; loop++)
  {
    int loop2 = loop;
    double t = 0;
    double divider = 0;
    int weight = period;
    for (; weight > 0; weight--, loop2--)
    {
      t = t + in->getData(loop2) * (double) weight;
      divider = divider + weight;
    }
    line->append(t / divider);
  }
  
  return line;
}

