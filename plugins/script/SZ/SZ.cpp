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

#include "SZ.h"
#include "Curve.h"
#include "Globals.h"

#include <QtDebug>

SZ::SZ ()
{
  _plugin = "SZ";
  _method << "LONG" << "SHORT";
}

int SZ::command (Command *command)
{
  // PARMS:
  // INPUT_HIGH
  // INPUT_LOW
  // NAME
  // METHOD
  // PERIOD
  // NO_DECLINE_PERIOD
  // COEFFICIENT

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  Curve *ihigh = i->line(command->parm("INPUT_HIGH"));
  if (! ihigh)
  {
    qDebug() << _plugin << "::command: invalid INPUT_HIGH" << command->parm("INPUT_HIGH");
    return 1;
  }

  Curve *ilow = i->line(command->parm("INPUT_LOW"));
  if (! ilow)
  {
    qDebug() << _plugin << "::command: invalid INPUT_LOW" << command->parm("INPUT_LOW");
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::command: duplicate NAME" << name;
    return 1;
  }

  int method = _method.indexOf(command->parm("METHOD"));
  if (method == -1)
  {
    qDebug() << _plugin << "::command: invalid METHOD" << command->parm("METHOD");
    return 1;
  }

  bool ok;
  int period = command->parm("PERIOD").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid PERIOD" << command->parm("PERIOD");
    return 1;
  }

  int no_decline_period = command->parm("NO_DECLINE_PERIOD").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid NO_DECLINE_PERIOD" << command->parm("NO_DECLINE_PERIOD");
    return 1;
  }

  double coefficient = command->parm("COEFFICIENT").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid COEFFICIENT" << command->parm("COEFFICIENT");
    return 1;
  }

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

  int ipos = 0;
  int end = 0;
  ihigh->keyRange(ipos, end);
  ipos += period + 1;
  int start = ipos;
  for (; ipos <= end; ipos++)
  {
    // calculate downside/upside penetration for lookback period
    int lbloop;
    int lbstart = ipos - period;
    if (lbstart < 2)
      lbstart = 2;
    double uptrend_noise_avg = 0;
    double uptrend_noise_cnt = 0;
    double dntrend_noise_avg = 0;
    double dntrend_noise_cnt = 0;
    for (lbloop = lbstart; lbloop < ipos; lbloop++)
    {
      CurveBar *hbar = ihigh->bar(lbloop);
      if (! hbar)
        continue;

      CurveBar *phbar = ihigh->bar(lbloop - 1);
      if (! phbar)
        continue;

      CurveBar *lbar = ilow->bar(lbloop);
      if (! lbar)
        continue;

      CurveBar *plbar = ilow->bar(lbloop - 1);
      if (! plbar)
        continue;

      double lo_curr = lbar->data();
      double lo_last = plbar->data();
      double hi_curr = hbar->data();
      double hi_last = phbar->data();
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

    CurveBar *phbar = ihigh->bar(ipos - 1);
    if (! phbar)
      continue;

    CurveBar *plbar = ilow->bar(ipos - 1);
    if (! plbar)
      continue;

    double lo_last = plbar->data();
    double hi_last = phbar->data();
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

  pl->setLabel(name);
  i->setLine(name, pl);

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  SZ *o = new SZ;
  return ((ScriptPlugin *) o);
}
