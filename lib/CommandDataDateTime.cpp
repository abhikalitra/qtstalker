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

#include "CommandDataDateTime.h"
#include "DataFactory.h"
#include "DataString.h"
#include "DataDateTime.h"

#include <QtDebug>

CommandDataDateTime::CommandDataDateTime (QObject *p) : Command (p)
{
  _name = "DATA_DATE_TIME";

  _values.insert(_ParmTypeKey, new DataString());
  _values.insert(_ParmTypeValue, new DataDateTime());
}

void CommandDataDateTime::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandDataDateTime::runScript: parse error");
    return;
  }

  QString key = _values.value(_ParmTypeKey)->toString();

  DataFactory dfac;
  Data *d = dfac.data(DataFactory::_DATETIME);
  if (! d)
  {
    Command::error("CommandDataDateTime::runScript: error creating DataDateTime");
    return;
  }

  script->setData(key, d);

  d->set(_values.value(_ParmTypeValue)->toDateTime());

  Command::done(QString());
}
