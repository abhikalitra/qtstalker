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

#include "CommandMA.h"
#include "MAType.h"
#include "DataString.h"
#include "DataInteger.h"
#include "DataMA.h"
#include "ScriptVerifyCurve.h"

#include <QtDebug>

CommandMA::CommandMA (QObject *p) : Command (p)
{
  _name = "MA";

  _values.insert(_ParmTypeName, new DataString());
  _values.insert(_ParmTypeInput, new DataString());
  _values.insert(_ParmTypePeriod, new DataInteger(10));
  _values.insert(_ParmTypeType, new DataMA("EMA"));
}

void CommandMA::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandMA::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  Data *in = svc.toCurve(script, _values.value(_ParmTypeInput)->toString(), toffset);
  if (! in)
  {
    Command::error("CommandMA::runScript: invalid Input");
    return;
  }

  MAType mat;
  Data *line = mat.getMA(in,
			 _values.value(_ParmTypePeriod)->toInteger(),
			 _values.value(_ParmTypeType)->toInteger());
  if (! line)
  {
    Command::error(QString());
    return;
  }

  script->setData(_values.value(_ParmTypeName)->toString(), line);

  Command::done(QString());
}
