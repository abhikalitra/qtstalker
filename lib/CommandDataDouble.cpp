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

#include "CommandDataDouble.h"
#include "DataFactory.h"
#include "DataString.h"
#include "DataDouble.h"

#include <QtDebug>

CommandDataDouble::CommandDataDouble (QObject *p) : Command (p)
{
  _name = "DATA_DOUBLE";

  _values.insert(_ParmTypeKey, new DataString());
  _values.insert(_ParmTypeValue, new DataDouble());
}

void CommandDataDouble::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandDataDouble::runScript: parse error");
    return;
  }

  QString key = _values.value(_ParmTypeKey)->toString();

  DataFactory dfac;
  Data *d = dfac.data(DataFactory::_DOUBLE);
  if (! d)
  {
    Command::error("CommandDataDouble::runScript: error creating DataDouble");
    return;
  }

  script->setData(key, d);

  d->set(_values.value(_ParmTypeValue)->toDouble());

  Command::done(QString());
}
