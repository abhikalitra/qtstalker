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

#include "CommandLoad.h"
#include "Script.h"
#include "EAVDataBase.h"

#include <QtDebug>

CommandLoad::CommandLoad ()
{
  _name = "LOAD";
}

QString CommandLoad::run (CommandParse &sg, void *scr)
{
  // PARM1 = object
  // PARM2 = database table name
  
  if (sg.values() != 2)
  {
    qDebug() << "CommandLoad::run: invalid number of values";
    return _returnCode;
  }
  
  Script *script = (Script *) scr;
  int pos = 0;
  QString name = sg.value(pos++);
  Command *c = script->scriptCommand(name);
  if (! c)
  {
    qDebug() << "CommandLoad::run: invalid object name" << name;
    return _returnCode;
  }

  QString table = sg.value(pos++);
  
  Entity settings;
  settings.setName(name);

  EAVDataBase db(table);
  if (db.get(settings))
  {
//    qDebug() << "CommandLoad::run: EAVDataBase error";
    _returnCode = "OK";
    return _returnCode;
  }

  c->merge(settings);
  
  _returnCode = "OK";
  return _returnCode;
}
