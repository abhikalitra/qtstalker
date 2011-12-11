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
#include "DataDouble.h"
#include "DataString.h"
#include "DataInteger.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>
#include <cmath>

CommandVFI::CommandVFI (QObject *p) : Command (p)
{
  _name = "VFI";

  _values.insert(_ParmTypeOutput, new DataString());
  _values.insert(_ParmTypeHigh, new DataString());
  _values.insert(_ParmTypeLow, new DataString());
  _values.insert(_ParmTypeClose, new DataString());
  _values.insert(_ParmTypeVolume, new DataString());
  _values.insert(_ParmTypePeriod, new DataInteger(10));
}

void CommandVFI::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandVFI::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  Data *ihigh = svc.toCurve(script, _values.value(_ParmTypeHigh)->toString(), toffset);
  if (! ihigh)
  {
    Command::error("CommandVFI::runScript: invalid High");
    return;
  }

  Data *ilow = svc.toCurve(script, _values.value(_ParmTypeLow)->toString(), toffset);
  if (! ilow)
  {
    Command::error("CommandVFI::runScript: invalid Low");
    return;
  }

  Data *iclose = svc.toCurve(script, _values.value(_ParmTypeClose)->toString(), toffset);
  if (! iclose)
  {
    Command::error("CommandVFI::runScript: invalid Close");
    return;
  }

  Data *ivol = svc.toCurve(script, _values.value(_ParmTypeVolume)->toString(), toffset);
  if (! ivol)
  {
    Command::error("CommandVFI::runScript: invalid Volume");
    return;
  }

  QList<Data *> list;
  list << ihigh << ilow << iclose << ivol;

  Data *line = getVFI(list, _values.value(_ParmTypePeriod)->toInteger());
  if (! line)
  {
    Command::error("CommandVFI::runScript: getVFI");
    return;
  }

  script->setData(_values.value(_ParmTypeOutput)->toString(), line);

  Command::done(QString());
}

Data * CommandVFI::getVFI (QList<Data *> &list, int period)
{
  if (list.count() != 4)
    return 0;

  ScriptVerifyCurveKeys svck;
  QList<int> keys;
  if (svck.keys(list, keys))
    return 0;

  Data *vfi = new CurveData;
  Data *ihigh = list.at(0);
  Data *ilow = list.at(1);
  Data *iclose = list.at(2);
  Data *ivol = list.at(3);
  int loop = period;
  for (; loop < keys.count(); loop++)
  {
    Data *hbar = ihigh->toData(keys.at(loop - period));
    if (! hbar)
      continue;

    Data *lbar = ilow->toData(keys.at(loop - period));
    if (! lbar)
      continue;

    Data *cbar = iclose->toData(keys.at(loop - period));
    if (! cbar)
      continue;

    double inter = 0.0;
    double sma_vol = 0.0;
    int i;
    double close = cbar->toData(CurveBar::_VALUE)->toDouble();
    double high = hbar->toData(CurveBar::_VALUE)->toDouble();
    double low = lbar->toData(CurveBar::_VALUE)->toDouble();
    double typical = (high + low + close) / 3.0;
    for (i = loop - period + 1; i <= loop; i++)
    {
      hbar = ihigh->toData(keys.at(i));
      if (! hbar)
        continue;

      lbar = ilow->toData(keys.at(i));
      if (! lbar)
        continue;

      cbar = iclose->toData(keys.at(i));
      if (! cbar)
        continue;

      Data *vbar = ivol->toData(keys.at(i));
      if (! vbar)
        continue;

      double ytypical = typical;
      close = cbar->toData(CurveBar::_VALUE)->toDouble();
      high = hbar->toData(CurveBar::_VALUE)->toDouble();
      low = lbar->toData(CurveBar::_VALUE)->toDouble();
      typical = (high + low + close) / 3.0;
      double delta = (log(typical) - log(ytypical));
      inter += delta * delta;
      sma_vol += vbar->toData(CurveBar::_VALUE)->toDouble();
    }
    inter = 0.2 * sqrt(inter / (double) period) * typical;
    sma_vol /= (double) period;

    hbar = ihigh->toData(keys.at(loop - period));
    if (! hbar)
      continue;

    lbar = ilow->toData(keys.at(loop - period));
    if (! lbar)
      continue;

    cbar = iclose->toData(keys.at(loop - period));
    if (! cbar)
      continue;

    close = cbar->toData(CurveBar::_VALUE)->toDouble();
    high = hbar->toData(CurveBar::_VALUE)->toDouble();
    low = lbar->toData(CurveBar::_VALUE)->toDouble();
    typical = (high + low + close) / 3.0;
    double t = 0;
    for (i = loop - period + 1; i <= loop; i++)
    {
      hbar = ihigh->toData(keys.at(i));
      if (! hbar)
        continue;

      lbar = ilow->toData(keys.at(i));
      if (! lbar)
        continue;

      cbar = iclose->toData(keys.at(i));
      if (! cbar)
        continue;

      Data *vbar = ivol->toData(keys.at(i));
      if (! vbar)
        continue;

      double ytypical = typical;
      double volume = vbar->toData(CurveBar::_VALUE)->toDouble();
      close = cbar->toData(CurveBar::_VALUE)->toDouble();
      high = hbar->toData(CurveBar::_VALUE)->toDouble();
      low = lbar->toData(CurveBar::_VALUE)->toDouble();
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
    b->set(CurveBar::_VALUE, new DataDouble(t));
    vfi->set(keys.at(loop), b);
  }

  return vfi;
}
