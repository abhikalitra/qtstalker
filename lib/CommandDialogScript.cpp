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

#include "CommandDialogScript.h"
#include "Script.h"
#include "TypeThreadMessage.h"
#include "ThreadMessageFunctions.h"
#include "KeyScriptDataBase.h"

#include <QtDebug>
#include <QUuid>

CommandDialogScript::CommandDialogScript ()
{
  _name = "DIALOG_SCRIPT";

  KeyScriptDataBase keys;
  
  Data td(TypeData::_FILE);
  Entity::set(keys.indexToString(KeyScriptDataBase::_FILE), td);

  td = Data(QString("perl"));
  Entity::set(keys.indexToString(KeyScriptDataBase::_COMMAND), td);

  td = Data(FALSE);
  Entity::set(keys.indexToString(KeyScriptDataBase::_STARTUP), td);

  td = Data(0);
  Entity::set(keys.indexToString(KeyScriptDataBase::_RUN_INTERVAL), td);
}

QString CommandDialogScript::run (CommandParse &, void *scr)
{
  Script *script = (Script *) scr;
  
  KeyScriptDataBase keys;
  Data file, command, startup, interval;
  Entity::toData(keys.indexToString(KeyScriptDataBase::_FILE), file);
  Entity::toData(keys.indexToString(KeyScriptDataBase::_COMMAND), command);
  Entity::toData(keys.indexToString(KeyScriptDataBase::_STARTUP), startup);
  Entity::toData(keys.indexToString(KeyScriptDataBase::_RUN_INTERVAL), interval);
  
  Entity dialog;
  dialog.set(QString("MESSAGE"), Data(TypeThreadMessage::_DIALOG_SCRIPT));
  dialog.set(keys.indexToString(KeyScriptDataBase::_FILE), file);
  dialog.set(keys.indexToString(KeyScriptDataBase::_COMMAND), command);
  dialog.set(keys.indexToString(KeyScriptDataBase::_STARTUP), startup);
  dialog.set(keys.indexToString(KeyScriptDataBase::_RUN_INTERVAL), interval);
  
  QString id = QUuid::createUuid().toString();
  
  ThreadMessageFunctions tmf;
  if (tmf.sendReturn(id, dialog, script))
    return _returnCode;
  
  dialog.toData(keys.indexToString(KeyScriptDataBase::_FILE), file);
  dialog.toData(keys.indexToString(KeyScriptDataBase::_COMMAND), command);
  dialog.toData(keys.indexToString(KeyScriptDataBase::_STARTUP), startup);
  dialog.toData(keys.indexToString(KeyScriptDataBase::_RUN_INTERVAL), interval);
  
  Entity::set(keys.indexToString(KeyScriptDataBase::_FILE), file);
  Entity::set(keys.indexToString(KeyScriptDataBase::_COMMAND), command);
  Entity::set(keys.indexToString(KeyScriptDataBase::_STARTUP), startup);
  Entity::set(keys.indexToString(KeyScriptDataBase::_RUN_INTERVAL), interval);
  
  _returnCode = "OK";
  return _returnCode;
}
