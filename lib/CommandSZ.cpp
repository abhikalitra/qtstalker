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
#include "InputType.h"
#include "SettingString.h"
#include "SettingInteger.h"
#include "SettingDouble.h"
#include "SettingList.h"

#include <QtDebug>

CommandSZ::CommandSZ (QObject *p) : Command (p)
{
  _type = "SZ";
  _method << "LONG" << "SHORT";
}

int CommandSZ::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  QString name = sg->get("NAME")->getString();
  Curve *line = script->curve(name);
  if (line)
  {
    qDebug() << _type << "::runScript: duplicate name" << name;
    return _ERROR;
  }

  QString key = sg->get("HIGH")->getString();
  QString s = script->setting(key)->getString();
  Curve *ihigh = script->curve(s);
  if (! ihigh)
  {
    qDebug() << _type << "::runScript: invalid HIGH" << s;
    return _ERROR;
  }

  key = sg->get("LOW")->getString();
  s = script->setting(key)->getString();
  Curve *ilow = script->curve(s);
  if (! ilow)
  {
    qDebug() << _type << "::runScript: invalid LOW" << s;
    return _ERROR;
  }

  s = sg->get("METHOD")->getString();
  int method = _method.indexOf(s);
  if (method == -1)
  {
    qDebug() << _type << "::runScript: invalid METHOD" << s;
    return _ERROR;
  }

  int period = sg->get("PERIOD")->getInteger();

  int no_decline_period = sg->get("PERIOD_NO_DECLINE")->getInteger();

  double coefficient = sg->get("COEFFICIENT")->getDouble();

  QList<Curve *> list;
  list << ihigh << ilow;
  Curve *pl = getSZ(list, method, period, no_decline_period, coefficient);
  if (! pl)
    return _ERROR;

  pl->setLabel(name);
  script->setCurve(name, pl);

  return _OK;
}

Curve * CommandSZ::getSZ (QList<Curve *> &list, int method, int period, int no_decline_period, double coefficient)
{
  if (list.count() != 2)
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
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

  Curve *ihigh = list.at(0);
  Curve *ilow = list.at(1);
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
      CurveBar *hbar = ihigh->bar(keys.at(lbloop));
      if (! hbar)
        continue;

      CurveBar *phbar = ihigh->bar(keys.at(lbloop - 1));
      if (! phbar)
        continue;

      CurveBar *lbar = ilow->bar(keys.at(lbloop));
      if (! lbar)
        continue;

      CurveBar *plbar = ilow->bar(keys.at(lbloop - 1));
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

    CurveBar *phbar = ihigh->bar(keys.at(ipos - 1));
    if (! phbar)
      continue;

    CurveBar *plbar = ilow->bar(keys.at(ipos - 1));
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

    sz_uptrend->setBar(keys.at(ipos), new CurveBar(adjusted_uptrend_stop));
    sz_dntrend->setBar(keys.at(ipos), new CurveBar(adjusted_dntrend_stop));
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

SettingGroup * CommandSZ::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_NONE, Setting::_CURVE, _type);
  ss->setKey("NAME");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("HIGH");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("LOW");
  sg->set(ss);

  SettingInteger *si = new SettingInteger(0, 0, 10, 9999, 2);
  si->setKey("PERIOD");
  sg->set(si);

  si = new SettingInteger(0, 0, 2, 9999, 1);
  si->setKey("PERIOD_NO_DECLINE");
  sg->set(si);

  SettingDouble *sd = new SettingDouble(0, 0, 2, 9999, -9999);
  sd->setKey("COEFFICIENT");
  sg->set(sd);

  SettingList *sl = new SettingList(_method, _method.at(0));
  sl->setKey("METHOD");
  sg->set(sl);

  return sg;
}
