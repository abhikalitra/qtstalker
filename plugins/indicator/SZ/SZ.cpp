/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

/* The "SafeZone Stop" indicator is described in
   Dr. Alexander Elder's book _Come Into My Trading Room_, p.173 */

#include "SZ.h"
#include <math.h>
#include <stdio.h>

SZ::SZ ()
{
  pluginName = "SZ";

  set(tr("Type"), pluginName, Setting::None);
  set(tr("Color"), "white", Setting::Color);
  set(tr("Line Type"), tr("Line"), Setting::LineType);
  set(tr("Coefficient"), "2.5", Setting::Float);
  set(tr("Lookback Period"), "10", Setting::Integer);
  set(tr("No Decline Period"), "2", Setting::Integer);
  set(tr("Plot"), tr("True"), Setting::None);
  set(tr("Alert"), tr("False"), Setting::None);

  QStringList l;
  l.append(tr("Long"));
  l.append(tr("Short"));
  set(tr("Position"), tr("Long"), Setting::List);
  setList(tr("Position"), l);

  about = "SafeZone Stop\n";
}

SZ::~SZ ()
{
}

void SZ::calculate ()
{
  int period = getInt(tr("Lookback Period"));
  if (period < 1)
    period = 1;

  double coefficient = getFloat(tr("Coefficient"));

  int display_uptrend = 0;
  int display_dntrend = 0;
  int position = 1;
  if (! getData(tr("Position")).compare(tr("Long")))
    position = 1;
  else
    position = 2;
  if (position & 1) // long
    display_uptrend = 1;
  if (position & 2) // short
    display_dntrend = 1;

  PlotLine *sz_uptrend = new PlotLine();
  PlotLine *sz_dntrend = new PlotLine();

  double uptrend_stop = 0;
  double dntrend_stop = 0;

  int no_decline_period = getInt(tr("No Decline Period"));
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
  for (loop = start; loop < (int) data->count(); loop++)
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
      double lo_curr = data->getLow(lbloop);
      double lo_last = data->getLow(lbloop - 1);
      double hi_curr = data->getHigh(lbloop);
      double hi_last = data->getHigh(lbloop - 1);
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

    double lo_last = data->getLow(loop - 1);
    double hi_last = data->getHigh(loop - 1);
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

    sz_uptrend->append(adjusted_uptrend_stop);
    sz_dntrend->append(adjusted_dntrend_stop);
  }

  if (display_uptrend)
  {
    sz_uptrend->setColor(getData(tr("Color")));
    sz_uptrend->setType(getData(tr("Line Type")));
    sz_uptrend->setLabel(tr("SZ LONG"));
    output.append(sz_uptrend);
  }

  if (display_dntrend)
  {
    sz_dntrend->setColor(getData(tr("Color")));
    sz_dntrend->setType(getData(tr("Line Type")));
    sz_dntrend->setLabel(tr("SZ SHORT"));
    output.append(sz_dntrend);
  }
}

Plugin * create ()
{
  SZ *o = new SZ;
  return ((Plugin *) o);
}


