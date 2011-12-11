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

/* The "Market Thermometer" _indicator is described in
   Dr. Alexander Elder's book _Come Into My Trading Room_, p.162 */

/* Alert Notes
1) enter when therm falls below MA
2) exit when threshold is triggered
3) explosive move expected when therm stays below MA for 5-7 days
*/

#include "CommandTHERM.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "DataDouble.h"
#include "DataString.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>
#include <cmath>

CommandTHERM::CommandTHERM (QObject *p) : Command (p)
{
  _name = "THERM";

  _values.insert(_ParmTypeOutput, new DataString());
  _values.insert(_ParmTypeHigh, new DataString());
  _values.insert(_ParmTypeLow, new DataString());
}

void CommandTHERM::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandTHERM::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  Data *ihigh = svc.toCurve(script, _values.value(_ParmTypeHigh)->toString(), toffset);
  if (! ihigh)
  {
    Command::error("CommandTHERM::runScript: invalid High");
    return;
  }

  Data *ilow = svc.toCurve(script, _values.value(_ParmTypeLow)->toString(), toffset);
  if (! ilow)
  {
    Command::error("CommandTHERM::runScript: invalid Low");
    return;
  }

  QList<Data *> list;
  list << ihigh << ilow;

  Data *line = getTHERM(list);
  if (! line)
  {
    Command::error("CommandTHERM::runScript: getTHERM error");
    return;
  }

  script->setData(_values.value(_ParmTypeOutput)->toString(), line);

  Command::done(QString());
}

Data * CommandTHERM::getTHERM (QList<Data *> &list)
{
  if (list.count() != 2)
    return 0;

  ScriptVerifyCurveKeys svck;
  QList<int> keys;
  if (svck.keys(list, keys))
    return 0;

  Data *line = new CurveData;
  Data *ihigh = list.at(0);
  Data *ilow = list.at(1);
  double thermometer = 0;
  int loop = 1;
  for (; loop < keys.count(); loop++)
  {
    Data *hbar = ihigh->toData(keys.at(loop));
    if (! hbar)
      continue;

    Data *phbar = ihigh->toData(keys.at(loop - 1));
    if (! phbar)
      continue;

    Data *lbar = ilow->toData(keys.at(loop));
    if (! lbar)
      continue;

    Data *plbar = ilow->toData(keys.at(loop - 1));
    if (! plbar)
      continue;

    double high = fabs(hbar->toData(CurveBar::_VALUE)->toDouble() - phbar->toData(CurveBar::_VALUE)->toDouble());
    double lo = fabs(plbar->toData(CurveBar::_VALUE)->toDouble() - lbar->toData(CurveBar::_VALUE)->toDouble());

    if (high > lo)
      thermometer = high;
    else
      thermometer = lo;

    Data *b = new CurveBar;
    b->set(CurveBar::_VALUE, new DataDouble(thermometer));
    line->set(keys.at(loop), b);
  }

  return line;
}
