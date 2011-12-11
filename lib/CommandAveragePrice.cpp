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

#include "CommandAveragePrice.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "DataDouble.h"
#include "DataString.h"
#include "DataInteger.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>

CommandAveragePrice::CommandAveragePrice (QObject *p) : Command (p)
{
  _name = "AVERAGE_PRICE";

  _values.insert(_ParmTypeOutput, new DataString());
  _values.insert(_ParmTypeOpen, new DataString());
  _values.insert(_ParmTypeHigh, new DataString());
  _values.insert(_ParmTypeLow, new DataString());
  _values.insert(_ParmTypeClose, new DataString());
}

void CommandAveragePrice::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandAveragePrice::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  Data *iopen = svc.toCurve(script, _values.value(_ParmTypeOpen)->toString(), toffset);
  if (! iopen)
  {
    Command::error("CommandAveragePrice::runScript: invalid Open");
    return;
  }

  Data *ihigh = svc.toCurve(script, _values.value(_ParmTypeHigh)->toString(), toffset);
  if (! ihigh)
  {
    Command::error("CommandAveragePrice::runScript: invalid High");
    return;
  }

  Data *ilow = svc.toCurve(script, _values.value(_ParmTypeLow)->toString(), toffset);
  if (! ilow)
  {
    Command::error("CommandAveragePrice::runScript: invalid Low");
    return;
  }

  Data *iclose = svc.toCurve(script, _values.value(_ParmTypeClose)->toString(), toffset);
  if (! iclose)
  {
    Command::error("CommandAveragePrice::runScript: invalid Close");
    return;
  }

  QList<Data *> list;
  list << iopen << ihigh << ilow << iclose;

  Data *line = getAP(list);
  if (! line)
  {
    Command::error("CommandAveragePrice::runScript: getAveragePrice error");
    return;
  }

  script->setData(_values.value(_ParmTypeOutput)->toString(), line);

  Command::done(QString());
}

Data * CommandAveragePrice::getAP (QList<Data *> &list)
{
  if (list.count() != 4)
    return 0;

  ScriptVerifyCurveKeys svck;
  QList<int> keys;
  if (svck.keys(list, keys))
    return 0;

  Data *line = new CurveData;
  int loop = 0;
  Data *iopen = list.at(loop++);
  Data *ihigh = list.at(loop++);
  Data *ilow = list.at(loop++);
  Data *iclose = list.at(loop++);
  for (loop = 0; loop < keys.count(); loop++)
  {
    Data *obar = iopen->toData(keys.at(loop));
    if (! obar)
      continue;

    Data *hbar = ihigh->toData(keys.at(loop));
    if (! hbar)
      continue;

    Data *lbar = ilow->toData(keys.at(loop));
    if (! lbar)
      continue;

    Data *cbar = iclose->toData(keys.at(loop));
    if (! cbar)
      continue;

    double t = (obar->toData(CurveBar::_VALUE)->toDouble() + hbar->toData(CurveBar::_VALUE)->toDouble()
                + lbar->toData(CurveBar::_VALUE)->toDouble() + cbar->toData(CurveBar::_VALUE)->toDouble()) / 4.0;
    Data *b = new CurveBar;
    b->set(CurveBar::_VALUE, new DataDouble(t));
    line->set(keys.at(loop), b);
  }

  return line;
}
