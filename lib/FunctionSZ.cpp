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

/* The "SafeZone Stop" _indicator is described in
   Dr. Alexander Elder's book _Come Into My Trading Room_, p.173 */

#include "FunctionSZ.h"
#include "FunctionBARS.h"
#include "Globals.h"

#include <QtDebug>
#include <cmath>

FunctionSZ::FunctionSZ ()
{
  _methodList << "Long" << "Short";
}

int FunctionSZ::script (QStringList &set, Indicator &ind)
{
  // INDICATOR,PLUGIN,SZ,<NAME>,<METHOD>,<PERIOD>,<NO_DECLINE_PERIOD>,<COEFFICIENT>
  //     0       1    2    3       4        5              6               7 

  if (set.count() != 8)
  {
    qDebug() << "FunctionSZ::script: invalid parm count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "FunctionSZ::script: invalid name" << set[3];
    return 1;
  }

  int method = _methodList.indexOf(set.at(4));
  if (method == -1)
  {
    qDebug() << "FunctionSZ::script: invalid method" << set.at(4);
    return 1;
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionSZ::script: invalid period" << set[5];
    return 1;
  }
  if (period < 1)
    period = 1;

  int no_decline_period = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "FunctionSZ::script: invalid no_decline_period" << set[6];
    return 1;
  }

  double coefficient = set[7].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "FunctionSZ::script: invalid coefficient" << set[7];
    return 1;
  }

  Curve *line = calculate(method, period, no_decline_period, coefficient);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

Curve * FunctionSZ::calculate (int method, int period, int no_decline_period, double coefficient)
{
  if (g_barData.count() < period || g_barData.count() < no_decline_period)
    return 0;

  int display_uptrend = 0;
  int display_dntrend = 0;
  int position = 1;
  if (method)
    position = 2;
  if (position & 1) // long
    display_uptrend = 1;
  if (position & 2) // short
    display_dntrend = 1;

  Curve *sz_uptrend = new Curve;
  Curve *sz_dntrend = new Curve;

  double uptrend_stop = 0;
  double dntrend_stop = 0;

  if (no_decline_period < 0)
    no_decline_period = 0;
  if (no_decline_period > 365)
    no_decline_period = 365;

  double old_uptrend_stops[no_decline_period];
  double old_dntrend_stops[no_decline_period];

  int loop;
  for (loop = 0; loop < no_decline_period; loop++)
  {
    old_uptrend_stops[loop] = 0;
    old_dntrend_stops[loop] = 0;
  }

  int start = period + 1;
  for (loop = start; loop < g_barData.count(); loop++)
  {
    // calculate downside/upside penetration for lookback period
    int lbloop;
    int lbstart = loop - period;
    if (lbstart < 2)
      lbstart = 2;
    double uptrend_noise_avg = 0;
    double uptrend_noise_cnt = 0;
    double dntrend_noise_avg = 0;
    double dntrend_noise_cnt = 0;
    for (lbloop = lbstart; lbloop < loop; lbloop++)
    {
      Bar bar = g_barData.getBar(lbloop);
      Bar pbar = g_barData.getBar(lbloop - 1);
      double lo_curr = bar.getLow();
      double lo_last = pbar.getLow();
      double hi_curr = bar.getHigh();
      double hi_last = pbar.getHigh();
      if (lo_last > lo_curr)
      {
	uptrend_noise_avg += lo_last - lo_curr;
	uptrend_noise_cnt++;
      }
      if (hi_last < hi_curr)
      {
	dntrend_noise_avg += hi_curr - hi_last;
	dntrend_noise_cnt++;
      }
    }
    // make *_avg into actual averages
    if (uptrend_noise_cnt > 0)
      uptrend_noise_avg /= uptrend_noise_cnt;
    if (dntrend_noise_cnt > 0)
      dntrend_noise_avg /= dntrend_noise_cnt;

    Bar pbar = g_barData.getBar(loop - 1);
    double lo_last = pbar.getLow();
    double hi_last = pbar.getHigh();
    uptrend_stop = lo_last - coefficient * uptrend_noise_avg;
    dntrend_stop = hi_last + coefficient * dntrend_noise_avg;

    double adjusted_uptrend_stop = uptrend_stop;
    double adjusted_dntrend_stop = dntrend_stop;

    int backloop;
    for (backloop = no_decline_period - 1; backloop >= 0; backloop--)
    {
      if (loop - backloop > start)
      {
	if (old_uptrend_stops[backloop] > adjusted_uptrend_stop)
	  adjusted_uptrend_stop = old_uptrend_stops[backloop];
	if (old_dntrend_stops[backloop] < adjusted_dntrend_stop)
	  adjusted_dntrend_stop = old_dntrend_stops[backloop];
      }
      if (backloop > 0)
      {
	old_uptrend_stops[backloop] = old_uptrend_stops[backloop-1];
	old_dntrend_stops[backloop] = old_dntrend_stops[backloop-1];
      }
    }

    old_uptrend_stops[0] = uptrend_stop;
    old_dntrend_stops[0] = dntrend_stop;

    sz_uptrend->setBar(loop, new CurveBar(adjusted_uptrend_stop));
    sz_dntrend->setBar(loop, new CurveBar(adjusted_dntrend_stop));
  }

  Curve *pl = 0;
  if (display_uptrend)
  {
    pl = sz_uptrend;
    delete sz_dntrend;
  }

  if (display_dntrend)
  {
    pl = sz_dntrend;
    delete sz_uptrend;
  }

  return pl;
}

QStringList & FunctionSZ::list ()
{
  return _methodList;
}

