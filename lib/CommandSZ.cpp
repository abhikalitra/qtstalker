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
#include "KeyCurveBar.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>

CommandSZ::CommandSZ ()
{
  _name = "SZ";
  _method << "LONG" << "SHORT";

  Data td;
  td.setLabel(QObject::tr("Output"));
  Entity::set(QString("OUTPUT"), td);
  
  td = Data(QString("high"));
  td.setLabel(QObject::tr("Input High"));
  Entity::set(QString("HIGH"), td);
  
  td = Data(QString("low"));
  td.setLabel(QObject::tr("Input Low"));
  Entity::set(QString("LOW"), td);
  
  td = Data(10);
  td.setLabel(QObject::tr("Period"));
  Entity::set(QString("PERIOD"), td);

  td = Data(_method, _method.at(0));
  td.setLabel(QObject::tr("Method"));
  Entity::set(QString("METHOD"), td);
  
  td = Data(2);
  td.setLabel(QObject::tr("No Decline Period"));
  Entity::set(QString("PERIOD_ND"), td);
  
  td = Data(2.0);
  td.setLabel(QObject::tr("Coefficient"));
  Entity::set(QString("COEFF"), td);
}

QString CommandSZ::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("HIGH"), td);

  ScriptVerifyCurve svc;
  Entity ihigh;
  if (svc.curve(script, td.toString(), ihigh))
  {
    qDebug() << "CommandSZ::run: invalid HIGH" << td.toString();
    return _returnCode;
  }

  Entity::toData(QString("LOW"), td);
  Entity ilow;
  if (svc.curve(script, td.toString(), ilow))
  {
    qDebug() << "CommandSZ::run: invalid LOW" << td.toString();
    return _returnCode;
  }

  Data period, ndp, coeff, method;
  Entity::toData(QString("PERIOD"), period);
  Entity::toData(QString("PERIOD_ND"), ndp);
  Entity::toData(QString("COEFF"), coeff);
  Entity::toData(QString("METHOD"), method);
  
  CurveData line;
  if (getSZ(ihigh, ilow, method.toInteger(), period.toInteger(), ndp.toInteger(), coeff.toDouble(), line))
  {
    qDebug() << "CommandSZ::runScript: getSZ error";
    return _returnCode;
  }

  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  _returnCode = "OK";
  return _returnCode;
}

int CommandSZ::getSZ (Entity &ihigh, Entity &ilow, int method, int period, int no_decline_period,
		      double coefficient, Entity &line)
{
  QList<int> keys;
  ScriptVerifyCurveKeys svck;
  if (svck.keys2(ihigh, ilow, keys))
    return 1;

  int display_uptrend = 0;
  int display_dntrend = 0;
  int position = 1;
  if (method)
    position = 2;
  if (position & 1) // long
    display_uptrend = 1;
  if (position & 2) // short
    display_dntrend = 1;

  CurveData sz_uptrend;
  CurveData sz_dntrend;
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

  KeyCurveBar cbkeys;
  int ipos = period + 1;
  int start = ipos;
  for (; ipos < keys.size(); ipos++)
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
      Entity hbar;
      if (ihigh.toIndex(keys.at(lbloop), hbar))
        continue;
      Data high;
      if (hbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), high))
        continue;
      
      Entity phbar;
      if (ihigh.toIndex(keys.at(lbloop - 1), phbar))
        continue;
      Data phigh;
      if (phbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), phigh))
        continue;

      Entity lbar;
      if (ilow.toIndex(keys.at(lbloop), lbar))
        continue;
      Data low;
      if (lbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), low))
        continue;
      
      Entity plbar;
      if (ilow.toIndex(keys.at(lbloop - 1), plbar))
        continue;
      Data plow;
      if (plbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), plow))
        continue;

      double lo_curr = low.toDouble();
      double lo_last = plow.toDouble();
      double hi_curr = high.toDouble();
      double hi_last = phigh.toDouble();
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

    Entity phbar;
    if (ihigh.toIndex(keys.at(ipos - 1), phbar))
      continue;
    Data phigh;
    if (phbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), phigh))
      continue;

    Entity plbar;
    if (ilow.toIndex(keys.at(ipos - 1), plbar))
      continue;
    Data plow;
    if (plbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), plow))
      continue;

    double lo_last = plow.toDouble();
    double hi_last = phigh.toDouble();
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

    CurveBar b;
    b.set(cbkeys.indexToString(KeyCurveBar::_VALUE), Data(adjusted_uptrend_stop));
    sz_uptrend.setEntity(keys.at(ipos), b);

    b.set(cbkeys.indexToString(KeyCurveBar::_VALUE), Data(adjusted_dntrend_stop));
    sz_dntrend.setEntity(keys.at(ipos), b);
  }

  if (display_uptrend)
    line = sz_uptrend;

  if (display_dntrend)
    line = sz_dntrend;

  return 0;
}
