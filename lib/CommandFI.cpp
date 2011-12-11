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
 *  MERCHANTABILITY or CommandFITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#include "CommandFI.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "DataDouble.h"
#include "DataString.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>

CommandFI::CommandFI (QObject *p) : Command (p)
{
  _name = "FI";

  _values.insert(_ParmTypeOutput, new DataString());
  _values.insert(_ParmTypeClose, new DataString());
  _values.insert(_ParmTypeVolume, new DataString());
}

void CommandFI::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandFI::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  Data *iclose = svc.toCurve(script, _values.value(_ParmTypeClose)->toString(), toffset);
  if (! iclose)
  {
    Command::error("CommandFI::runScript: invalid Close");
    return;
  }

  Data *ivol = svc.toCurve(script, _values.value(_ParmTypeVolume)->toString(), toffset);
  if (! ivol)
  {
    Command::error("CommandFI::runScript: invalid Volume");
    return;
  }

  QList<Data *> list;
  list << iclose << ivol;

  Data *line = getFI(list);
  if (! line)
  {
    Command::error("CommandFI::runScript: getFI error");
    return;
  }

  script->setData(_values.value(_ParmTypeOutput)->toString(), line);

  Command::done(QString());
}

Data * CommandFI::getFI (QList<Data *> &list)
{
  if (list.count() != 2)
    return 0;

  ScriptVerifyCurveKeys svck;
  QList<int> keys;
  if (svck.keys(list, keys))
    return 0;

  Data *line = new CurveData;
  Data *close = list.at(0);
  Data *vol = list.at(1);
  int loop = 1;
  double force = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *cbar = close->toData(keys.at(loop));
    if (! cbar)
      continue;

    Data *ycbar = close->toData(keys.at(loop - 1));
    if (! ycbar)
      continue;

    Data *vbar = vol->toData(keys.at(loop));
    if (! vbar)
      continue;

    double cdiff = cbar->toData(CurveBar::_VALUE)->toDouble() - ycbar->toData(CurveBar::_VALUE)->toDouble();
    force = vbar->toData(CurveBar::_VALUE)->toDouble() * cdiff;

    Data *b = new CurveBar;
    b->set(CurveBar::_VALUE, new DataDouble(force));
    line->set(keys.at(loop), b);
  }

  return line;
}
