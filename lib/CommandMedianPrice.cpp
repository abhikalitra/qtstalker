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

#include "CommandMedianPrice.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "DataDouble.h"
#include "DataString.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>

CommandMedianPrice::CommandMedianPrice (QObject *p) : Command (p)
{
  _name = "MEDIAN_PRICE";

  _values.insert(_ParmTypeOutput, new DataString());
  _values.insert(_ParmTypeInput, new DataString());
  _values.insert(_ParmTypeInput2, new DataString());
}

void CommandMedianPrice::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandMedianPrice::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  Data *in = svc.toCurve(script, _values.value(_ParmTypeInput)->toString(), toffset);
  if (! in)
  {
    Command::error("CommandMAMA::runScript: invalid Input");
    return;
  }

  Data *in2 = svc.toCurve(script, _values.value(_ParmTypeInput2)->toString(), toffset);
  if (! in2)
  {
    Command::error("CommandMAMA::runScript: invalid Input2");
    return;
  }

  QList<Data *> list;
  list << in << in2;

  Data *line = getMP(list);
  if (! line)
  {
    Command::error("CommandMAMA::runScript: getMP error");
    return;
  }

  script->setData(_values.value(_ParmTypeOutput)->toString(), line);

  Command::done(QString());
}

Data * CommandMedianPrice::getMP (QList<Data *> &list)
{
  ScriptVerifyCurveKeys svck;
  QList<int> keys;
  if (svck.keys(list, keys))
    return 0;

  Data *line = new CurveData;
  Data *in = list.at(0);
  Data *in2 = list.at(1);
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *bar = in->toData(keys.at(loop));
    if (! bar)
      continue;

    Data *bar2 = in2->toData(keys.at(loop));
    if (! bar2)
      continue;

    double t = (bar->toData(CurveBar::_VALUE)->toDouble() + bar2->toData(CurveBar::_VALUE)->toDouble()) / 2.0;
    Data *b = new CurveBar;
    b->set(CurveBar::_VALUE, new DataDouble(t));
    line->set(keys.at(loop), b);
  }

  return line;
}
