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
#include "CurveData.h"
#include "CurveBar.h"
#include "VerifyDataInput.h"
#include "SettingFactory.h"
#include "SettingDouble.h"

#include <QtDebug>
#include <cmath>

CommandVFI::CommandVFI (QObject *p) : Command (p)
{
  _name = "VFI";
}

int CommandVFI::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;
  QString s = sg->value("OUTPUT");
  if (s.isEmpty())
  {
    _message << "invalid OUTPUT";
    emit signalResume((void *) this);
    return _ERROR;
  }
  Setting *name = vdi.setting(SettingFactory::_STRING, script, s);
  if (! name)
  {
    _message << "invalid OUTPUT " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("HIGH");
  Data *ihigh = vdi.curve(script, s);
  if (! ihigh)
  {
    _message << "invalid HIGH " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("LOW");
  Data *ilow = vdi.curve(script, s);
  if (! ilow)
  {
    _message << "invalid LOW " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("CLOSE");
  Data *iclose = vdi.curve(script, s);
  if (! iclose)
  {
    _message << "invalid CLOSE " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("VOLUME");
  Data *ivol = vdi.curve(script, s);
  if (! ivol)
  {
    _message << "invalid VOLUME " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  s = sg->value("PERIOD");
  Setting *period = vdi.setting(SettingFactory::_INTEGER, script, s);
  if (! period)
  {
    _message << "invalid PERIOD " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  QList<Data *> list;
  list << ihigh << ilow << iclose << ivol;

  Data *line = getVFI(list, period->toInteger());
  if (! line)
  {
    emit signalResume((void *) this);
    return _ERROR;
  }

  script->setData(name->toString(), line);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}

Data * CommandVFI::getVFI (QList<Data *> &list, int period)
{
  if (list.count() != 4)
    return 0;

  VerifyDataInput vdi;
  QList<int> keys;
  if (vdi.curveKeys(list, keys))
    return 0;

  Data *vfi = new CurveData;
  Data *ihigh = list.at(0);
  Data *ilow = list.at(1);
  Data *iclose = list.at(2);
  Data *ivol = list.at(3);
  int loop = period;
  for (; loop < keys.count(); loop++)
  {
    Data *hbar = ihigh->getData(keys.at(loop - period));
    if (! hbar)
      continue;

    Data *lbar = ilow->getData(keys.at(loop - period));
    if (! lbar)
      continue;

    Data *cbar = iclose->getData(keys.at(loop - period));
    if (! cbar)
      continue;

    double inter = 0.0;
    double sma_vol = 0.0;
    int i;
    double close = cbar->get(CurveBar::_VALUE)->toDouble();
    double high = hbar->get(CurveBar::_VALUE)->toDouble();
    double low = lbar->get(CurveBar::_VALUE)->toDouble();
    double typical = (high + low + close) / 3.0;
    for (i = loop - period + 1; i <= loop; i++)
    {
      hbar = ihigh->getData(keys.at(i));
      if (! hbar)
        continue;

      lbar = ilow->getData(keys.at(i));
      if (! lbar)
        continue;

      cbar = iclose->getData(keys.at(i));
      if (! cbar)
        continue;

      Data *vbar = ivol->getData(keys.at(i));
      if (! vbar)
        continue;

      double ytypical = typical;
      close = cbar->get(CurveBar::_VALUE)->toDouble();
      high = hbar->get(CurveBar::_VALUE)->toDouble();
      low = lbar->get(CurveBar::_VALUE)->toDouble();
      typical = (high + low + close) / 3.0;
      double delta = (log(typical) - log(ytypical));
      inter += delta * delta;
      sma_vol += vbar->get(CurveBar::_VALUE)->toDouble();
    }
    inter = 0.2 * sqrt(inter / (double) period) * typical;
    sma_vol /= (double) period;

    hbar = ihigh->getData(keys.at(loop - period));
    if (! hbar)
      continue;

    lbar = ilow->getData(keys.at(loop - period));
    if (! lbar)
      continue;

    cbar = iclose->getData(keys.at(loop - period));
    if (! cbar)
      continue;

    close = cbar->get(CurveBar::_VALUE)->toDouble();
    high = hbar->get(CurveBar::_VALUE)->toDouble();
    low = lbar->get(CurveBar::_VALUE)->toDouble();
    typical = (high + low + close) / 3.0;
    double t = 0;
    for (i = loop - period + 1; i <= loop; i++)
    {
      hbar = ihigh->getData(keys.at(i));
      if (! hbar)
        continue;

      lbar = ilow->getData(keys.at(i));
      if (! lbar)
        continue;

      cbar = iclose->getData(keys.at(i));
      if (! cbar)
        continue;

      Data *vbar = ivol->getData(keys.at(i));
      if (! vbar)
        continue;

      double ytypical = typical;
      double volume = vbar->get(CurveBar::_VALUE)->toDouble();
      close = cbar->get(CurveBar::_VALUE)->toDouble();
      high = hbar->get(CurveBar::_VALUE)->toDouble();
      low = lbar->get(CurveBar::_VALUE)->toDouble();
      typical = (high + low + close) / 3.0;

      if (typical > ytypical + inter)
        t = t + log (1.0 + volume / sma_vol);
      else
      {
        if (typical < ytypical - inter)
          t = t - log (1.0 + volume / sma_vol);
      }
    }

    Data *b = new CurveBar;
    b->set(CurveBar::_VALUE, new SettingDouble(t));
    vfi->set(keys.at(loop), b);
  }

  return vfi;
}
