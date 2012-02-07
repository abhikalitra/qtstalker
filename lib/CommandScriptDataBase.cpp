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

#include "CommandScriptDataBase.h"
#include "KeyScriptDataBase.h"
#include "ScriptFunctions.h"

#include <QtDebug>

CommandScriptDataBase::CommandScriptDataBase ()
{
  _name = "SCRIPT_DATABASE";
  _method << "LIST" << "SAVE" << "REMOVE" << "LOAD";

  KeyScriptDataBase keys;
  
  Data td(_method, _method.at(0));
  td.setLabel(QObject::tr("Method"));
  Entity::set(QString("METHOD"), td);
  
  td = Data(QString());
  td.setLabel(QObject::tr("Name"));
  Entity::set(keys.indexToString(KeyScriptDataBase::_NAME), td);
  
  td = Data(TypeData::_FILE);
  td.setLabel(QObject::tr("File"));
  Entity::set(keys.indexToString(KeyScriptDataBase::_FILE), td);
  
  td = Data(FALSE);
  td.setLabel(QObject::tr("Startup"));
  Entity::set(keys.indexToString(KeyScriptDataBase::_STARTUP), td);
  
  td = Data(0);
  td.setLabel(QObject::tr("Interval"));
  Entity::set(keys.indexToString(KeyScriptDataBase::_INTERVAL), td);
  
  td = Data(QString("perl"));
  td.setLabel(QObject::tr("Command"));
  Entity::set(keys.indexToString(KeyScriptDataBase::_COMMAND), td);
  
  td = Data(QString());
  td.setLabel(QObject::tr("Comment"));
  Entity::set(keys.indexToString(KeyScriptDataBase::_COMMENT), td);
  
  td = Data(TypeData::_LIST);
  Entity::set(QString("LIST"), td);
}

QString CommandScriptDataBase::run (CommandParse &, void *)
{
  Data method;
  Entity::toData(QString("METHOD"), method);
  
  switch (_method.indexOf(method.toString()))
  {
    case 0: // LIST
      if (list())
        return _returnCode;
      break;
    case 1: // SAVE
      if (save())
        return _returnCode;
      break;
    case 2: // REMOVE
      if (remove())
        return _returnCode;
      break;
    case 3: // LOAD
      if (load())
        return _returnCode;
      break;
    default:
      qDebug() << "CommandScriptDataBase::run: invalid method" << method.toString();
      return _returnCode;
      break;
  }
  
  _returnCode = "OK";
  return _returnCode;
}

int CommandScriptDataBase::save ()
{
  KeyScriptDataBase keys;
  Data file, startup, interval, command, name, comment;
  Entity::toData(keys.indexToString(KeyScriptDataBase::_NAME), name);
  Entity::toData(keys.indexToString(KeyScriptDataBase::_FILE), file);
  Entity::toData(keys.indexToString(KeyScriptDataBase::_STARTUP), startup);
  Entity::toData(keys.indexToString(KeyScriptDataBase::_INTERVAL), interval);
  Entity::toData(keys.indexToString(KeyScriptDataBase::_COMMAND), command);
  Entity::toData(keys.indexToString(KeyScriptDataBase::_COMMENT), comment);
  
  ScriptFunctions db;
  Entity data;
  data.setName(name.toString());
  data.set(keys.indexToString(KeyScriptDataBase::_FILE), file);
  data.set(keys.indexToString(KeyScriptDataBase::_STARTUP), startup);
  data.set(keys.indexToString(KeyScriptDataBase::_INTERVAL), interval);
  data.set(keys.indexToString(KeyScriptDataBase::_COMMAND), command);
  data.set(keys.indexToString(KeyScriptDataBase::_COMMENT), comment);
  if (db.save(data))
  {
    qDebug() << "CommandScriptDataBase::save: EAVDataBase error" << name.toString();
    return 1;
  }

  return 0;
}

int CommandScriptDataBase::remove ()
{
  KeyScriptDataBase keys;
  Data name;
  Entity::toData(keys.indexToString(KeyScriptDataBase::_NAME), name);

  QStringList tl;
  tl << name.toString();

  ScriptFunctions db;
  if (db.remove(tl))
  {
    qDebug() << "CommandScriptDataBase::remove: EAVDataBase error" << tl;
    return 1;
  }

  return 0;
}

int CommandScriptDataBase::list ()
{
  QStringList tl;
  ScriptFunctions db;
  if (db.names(tl))
  {
    qDebug() << "CommandScriptDataBase::list: EAVDataBase error";
    return 1;
  }

  Data list;
  Entity::toData(QString("LIST"), list);
  list.set(tl);
  Entity::set(QString("LIST"), list);
  
  return 0;
}

int CommandScriptDataBase::load ()
{
  KeyScriptDataBase keys;
  Data name;
  Entity::toData(keys.indexToString(KeyScriptDataBase::_NAME), name);

  ScriptFunctions db;
  Entity script;
  script.setName(name.toString());
  if (db.load(script))
  {
    qDebug() << "CommandScriptDataBase::load: EAVDataBase error" << name.toString();
    return 1;
  }
  
  Data file, startup, interval, command, comment;
  script.toData(keys.indexToString(KeyScriptDataBase::_FILE), file);
  script.toData(keys.indexToString(KeyScriptDataBase::_STARTUP), startup);
  script.toData(keys.indexToString(KeyScriptDataBase::_INTERVAL), interval);
  script.toData(keys.indexToString(KeyScriptDataBase::_COMMAND), command);
  script.toData(keys.indexToString(KeyScriptDataBase::_COMMENT), comment);
  
  Entity::set(keys.indexToString(KeyScriptDataBase::_FILE), file);
  Entity::set(keys.indexToString(KeyScriptDataBase::_STARTUP), startup);
  Entity::set(keys.indexToString(KeyScriptDataBase::_INTERVAL), interval);
  Entity::set(keys.indexToString(KeyScriptDataBase::_COMMAND), command);
  Entity::set(keys.indexToString(KeyScriptDataBase::_COMMENT), comment);

  return 0;
}
