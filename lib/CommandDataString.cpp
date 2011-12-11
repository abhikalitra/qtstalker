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

#include "CommandDataString.h"
#include "DataFactory.h"
#include "DataString.h"

#include <QtDebug>

CommandDataString::CommandDataString (QObject *p) : Command (p)
{
  _name = "DATA_STRING";
  _type = _WAIT;

  _values.insert(_ParmTypeKey, new DataString());
  _values.insert(_ParmTypeValue, new DataString());
}

void CommandDataString::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandDataString::runScript: parse error");
    return;
  }

  DataFactory dfac;
  Data *d = dfac.data(DataFactory::_STRING);
  if (! d)
  {
    Command::error("CommandDataString::runScript: error creating DataString");
    return;
  }

  d->set(_values.value(_ParmTypeValue)->toString());

  script->setData(_values.value(_ParmTypeKey)->toString(), d);

  Command::done(QString());
}
