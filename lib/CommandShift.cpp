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

#include "CommandShift.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "DataDouble.h"
#include "DataString.h"
#include "DataInteger.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>

CommandShift::CommandShift (QObject *p) : Command (p)
{
  _name = "SHIFT";

  _values.insert(_ParmTypeOutput, new DataString());
  _values.insert(_ParmTypeInput, new DataString());
  _values.insert(_ParmTypePeriod, new DataInteger(10));
}

void CommandShift::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandShift::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  Data *in = svc.toCurve(script, _values.value(_ParmTypeInput)->toString(), toffset);
  if (! in)
  {
    Command::error("CommandShift::runScript: invalid Input");
    return;
  }

  Data *line = getShift(in, _values.value(_ParmTypePeriod)->toInteger());
  if (! line)
  {
    Command::error("CommandShift::runScript: getShift error");
    return;
  }

  script->setData(_values.value(_ParmTypeOutput)->toString(), line);

  Command::done(QString());
}

Data * CommandShift::getShift (Data *in, int period)
{
  QList<Data *> list;
  list << in;

  ScriptVerifyCurveKeys svck;
  QList<int> keys;
  if (svck.keys(list, keys))
    return 0;

  Data *line = new CurveData;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *bar = in->toData(keys.at(loop));

    int index = keys.at(loop) + period;
    if (index < 0)
      continue;

    Data *b = new CurveBar;
    b->set(CurveBar::_VALUE, new DataDouble(bar->toData(CurveBar::_VALUE)->toDouble()));
    line->set(keys.at(loop) + period, b);
  }

  return line;
}
