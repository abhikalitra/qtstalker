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

#include "CommandSZ.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "DataDouble.h"

#include <QtDebug>

CommandSZ::CommandSZ (QObject *p) : Command (p)
{
  _name = "SZ";
  _method << "LONG" << "SHORT";
}

int CommandSZ::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // OUTPUT
  QString name;
  QString s = sg->value("OUTPUT");
  if (vdi.toString(script, s, name))
  {
    qDebug() << "CommandSZ::runScript: invalid OUTPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // HIGH
  s = sg->value("HIGH");
  Data *ihigh = vdi.toCurve(script, s);
  if (! ihigh)
  {
    qDebug() << "CommandSZ::runScript: invalid HIGH" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // LOW
  s = sg->value("LOW");
  Data *ilow = vdi.toCurve(script, s);
  if (! ilow)
  {
    qDebug() << "CommandSZ::runScript: invalid LOW" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // METHOD
  s = sg->value("METHOD");
  int method = _method.indexOf(s);
  if (method == -1)
  {
    qDebug() << "CommandSZ::runScript: invalid METHOD, using default" << s;
    method = 0;
  }

  // PERIOD
  int period = 10;
  s = sg->value("PERIOD");
  if (vdi.toInteger(script, s, period))
  {
    qDebug() << "CommandSZ::runScript: invalid PERIOD, using default" << s;
    period = 10;
  }

  // PERIOD_NO_DECLINE
  int ndperiod = 2;
  s = sg->value("PERIOD_NO_DECLINE");
  if (vdi.toInteger(script, s, ndperiod))
  {
    qDebug() << "CommandSZ::runScript: invalid PERIOD_NO_DECLINE, using default" << s;
    ndperiod = 2;
  }

  // COEFFICIENT
  double coeff = 2;
  s = sg->value("COEFFICIENT");
  if (vdi.toDouble(script, s, coeff))
  {
    qDebug() << "CommandSZ::runScript: invalid COEFFICIENT, using default" << s;
    coeff = 2;
  }

  QList<Data *> list;
  list << ihigh << ilow;

  Data *pl = getSZ(list, method, period, ndperiod, coeff);
  if (! pl)
  {
    emit signalResume((void *) this);
    return _ERROR;
  }

  script->setData(name, pl);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}

Data * CommandSZ::getSZ (QList<Data *> &list, int method, int period, int no_decline_period, double coefficient)
{
  if (list.count() != 2)
    return 0;

  VerifyDataInput vdi;
  QList<int> keys;
  if (vdi.curveKeys(list, keys))
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

  Data *sz_uptrend = new CurveData;
  Data *sz_dntrend = new CurveData;

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

  Data *ihigh = list.at(0);
  Data *ilow = list.at(1);
  int ipos = period + 1;
  int start = ipos;
  for (; ipos < keys.count(); ipos++)
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
      Data *hbar = ihigh->toData(keys.at(lbloop));
      if (! hbar)
        continue;

      Data *phbar = ihigh->toData(keys.at(lbloop - 1));
      if (! phbar)
        continue;

      Data *lbar = ilow->toData(keys.at(lbloop));
      if (! lbar)
        continue;

      Data *plbar = ilow->toData(keys.at(lbloop - 1));
      if (! plbar)
        continue;

      double lo_curr = lbar->toData(CurveBar::_VALUE)->toDouble();
      double lo_last = plbar->toData(CurveBar::_VALUE)->toDouble();
      double hi_curr = hbar->toData(CurveBar::_VALUE)->toDouble();
      double hi_last = phbar->toData(CurveBar::_VALUE)->toDouble();
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

    Data *phbar = ihigh->toData(keys.at(ipos - 1));
    if (! phbar)
      continue;

    Data *plbar = ilow->toData(keys.at(ipos - 1));
    if (! plbar)
      continue;

    double lo_last = plbar->toData(CurveBar::_VALUE)->toDouble();
    double hi_last = phbar->toData(CurveBar::_VALUE)->toDouble();
    uptrend_stop = lo_last - coefficient * uptrend_noise_avg;
    dntrend_stop = hi_last + coefficient * dntrend_noise_avg;

    double adjusted_uptrend_stop = uptrend_stop;
    double adjusted_dntrend_stop = dntrend_stop;

    int backloop;
    for (backloop = no_decline_period - 1; backloop >= 0; backloop--)
    {
      if (ipos - backloop > start)
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

    Data *b = new CurveBar;
    b->set(CurveBar::_VALUE, new DataDouble(adjusted_uptrend_stop));
    sz_uptrend->set(keys.at(ipos), b);

    b = new CurveBar;
    b->set(CurveBar::_VALUE, new DataDouble(adjusted_dntrend_stop));
    sz_dntrend->set(keys.at(ipos), b);
  }

  Data *pl = 0;
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
