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

#include "CommandVFI.h"
#include "InputType.h"
#include "SettingString.h"
#include "SettingInteger.h"

#include <QtDebug>

CommandVFI::CommandVFI (QObject *p) : Command (p)
{
  _type = "VFI";
}

int CommandVFI::runScript (void *d)
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

  key = sg->get("CLOSE")->getString();
  s = script->setting(key)->getString();
  Curve *iclose = script->curve(s);
  if (! iclose)
  {
    qDebug() << _type << "::runScript: invalid CLOSE" << s;
    return _ERROR;
  }

  key = sg->get("VOLUME")->getString();
  s = script->setting(key)->getString();
  Curve *ivol = script->curve(s);
  if (! ivol)
  {
    qDebug() << _type << "::runScript: invalid VOLUME" << s;
    return _ERROR;
  }

  int period = sg->get("PERIOD")->getInteger();

  QList<Curve *> list;
  list << ihigh << ilow << iclose << ivol;
  line = getVFI(list, period);
  if (! line)
    return _ERROR;

  line->setLabel(name);
  script->setCurve(name, line);

  return _OK;
}

Curve * CommandVFI::getVFI (QList<Curve *> &list, int period)
{
  if (list.count() != 4)
    return 0;

  InputType it;
  QList<int> keys;
  if (it.keys(list, keys))
    return 0;

  Curve *vfi = new Curve;
  Curve *ihigh = list.at(0);
  Curve *ilow = list.at(1);
  Curve *iclose = list.at(2);
  Curve *ivol = list.at(3);
  int loop = period;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *hbar = ihigh->bar(keys.at(loop - period));
    if (! hbar)
      continue;

    CurveBar *lbar = ilow->bar(keys.at(loop - period));
    if (! lbar)
      continue;

    CurveBar *cbar = iclose->bar(keys.at(loop - period));
    if (! cbar)
      continue;

    double inter = 0.0;
    double sma_vol = 0.0;
    int i;
    double close = cbar->data();
    double high = hbar->data();
    double low = lbar->data();
    double typical = (high + low + close) / 3.0;
    for (i = loop - period + 1; i <= loop; i++)
    {
      hbar = ihigh->bar(keys.at(i));
      if (! hbar)
        continue;

      lbar = ilow->bar(keys.at(i));
      if (! lbar)
        continue;

      cbar = iclose->bar(keys.at(i));
      if (! cbar)
        continue;

      CurveBar *vbar = ivol->bar(keys.at(i));
      if (! vbar)
        continue;

      double ytypical = typical;
      close = cbar->data();
      high = hbar->data();
      low = lbar->data();
      typical = (high + low + close) / 3.0;
      double delta = (log(typical) - log(ytypical));
      inter += delta * delta;
      sma_vol += vbar->data();
    }
    inter = 0.2 * sqrt(inter / (double) period) * typical;
    sma_vol /= (double) period;

    hbar = ihigh->bar(keys.at(loop - period));
    if (! hbar)
      continue;

    lbar = ilow->bar(keys.at(loop - period));
    if (! lbar)
      continue;

    cbar = iclose->bar(keys.at(loop - period));
    if (! cbar)
      continue;

    close = cbar->data();
    high = hbar->data();
    low = lbar->data();
    typical = (high + low + close) / 3.0;
    double t = 0;
    for (i = loop - period + 1; i <= loop; i++)
    {
      hbar = ihigh->bar(keys.at(i));
      if (! hbar)
        continue;

      lbar = ilow->bar(keys.at(i));
      if (! lbar)
        continue;

      cbar = iclose->bar(keys.at(i));
      if (! cbar)
        continue;

      CurveBar *vbar = ivol->bar(keys.at(i));
      if (! vbar)
        continue;

      double ytypical = typical;
      double volume = vbar->data();
      close = cbar->data();
      high = hbar->data();
      low = lbar->data();
      typical = (high + low + close) / 3.0;

      if (typical > ytypical + inter)
        t = t + log (1.0 + volume / sma_vol);
      else
      {
        if (typical < ytypical - inter)
          t = t - log (1.0 + volume / sma_vol);
      }
    }

    vfi->setBar(keys.at(loop), new CurveBar(t));
  }

  return vfi;
}

SettingGroup * CommandVFI::settings ()
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

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("CLOSE");
  sg->set(ss);

  ss = new SettingString(Setting::_CURVE, Setting::_NONE, QString());
  ss->setKey("VOLUME");
  sg->set(ss);

  SettingInteger *si = new SettingInteger(0, 0, 10, 9999, 2);
  si->setKey("PERIOD");
  sg->set(si);

  return sg;
}
