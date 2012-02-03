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

#include "CommandScript.h"
#include "ThreadMessageFunctions.h"
#include "TypeThreadMessage.h"
#include "KeyScriptDataBase.h"

#include <QtDebug>

CommandScript::CommandScript ()
{
  _name = "SCRIPT";

  KeyScriptDataBase keys;

  Data td(QString("perl"));
  Entity::set(keys.indexToString(KeyScriptDataBase::_COMMAND), td);
  
  td = Data(TypeData::_FILE);
  Entity::set(keys.indexToString(KeyScriptDataBase::_FILE), td);
}

QString CommandScript::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  KeyScriptDataBase keys;
  Data file, command;
  Entity::toData(keys.indexToString(KeyScriptDataBase::_FILE), file);
  Entity::toData(keys.indexToString(KeyScriptDataBase::_COMMAND), command);
  
  Entity e;
  e.set(QString("MESSAGE"), Data(TypeThreadMessage::_SCRIPT));
  e.set(keys.indexToString(KeyScriptDataBase::_FILE), file);
  e.set(keys.indexToString(KeyScriptDataBase::_COMMAND), command);
  
  ThreadMessageFunctions tmf;
  tmf.send(e, script);
  
  _returnCode = "OK";
  return _returnCode;
}
