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

#include "CommandNormalize.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "DataDouble.h"
#include "DataString.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>
#include <cmath>

CommandNormalize::CommandNormalize (QObject *p) : Command (p)
{
  _name = "NORMALIZE";

  _values.insert(_ParmTypeOutput, new DataString());
  _values.insert(_ParmTypeInput, new DataString());
}

void CommandNormalize::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandNormalize::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  Data *in = svc.toCurve(script, _values.value(_ParmTypeInput)->toString(), toffset);
  if (! in)
  {
    Command::error("CommandNormalize::runScript: invalid Input");
    return;
  }

  QList<Data *> list;
  list << in;

  Data *line = getNORM(list);
  if (! line)
  {
    Command::error("CommandNormalize::runScript: getNORM error");
    return;
  }

  script->setData(_values.value(_ParmTypeOutput)->toString(), line);

  Command::done(QString());
}

Data * CommandNormalize::getNORM (QList<Data *> &list)
{
  if (! list.count())
    return 0;

  ScriptVerifyCurveKeys svck;
  QList<int> keys;
  if (svck.keys(list, keys))
    return 0;

  Data *in = list.at(0);
  double max = 0;
  double min = 0;
  if (in->highLow(max, min))
    return 0;

  double range = fabs(max) + fabs(min);

  Data *line = new CurveData;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *bar = in->toData(keys.at(loop));
    double t = ((bar->toData(CurveBar::_VALUE)->toDouble() - min) / range) * 100;

    Data *b = new CurveBar;
    b->set(CurveBar::_VALUE, new DataDouble(t));
    line->set(keys.at(loop), b);
  }

  return line;
}
