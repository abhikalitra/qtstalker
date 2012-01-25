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
#include "EAVDataBase.h"
#include "KeyScriptDataBase.h"

#include <QtDebug>

CommandScriptDataBase::CommandScriptDataBase ()
{
  _name = "SCRIPT_DATABASE";
  _method << "LIST" << "SAVE" << "REMOVE";
  
  Data td(_method, _method.at(0));
  td.setLabel(QObject::tr("Method"));
  Entity::set(QString("METHOD"), td);
  
  td = Data(QString());
  td.setLabel(QObject::tr("Name"));
  Entity::set(QString("NAME"), td);
  
  td = Data(TypeData::_FILE);
  td.setLabel(QObject::tr("File"));
  Entity::set(QString("FILE"), td);
  
  td = Data(FALSE);
  td.setLabel(QObject::tr("Startup"));
  Entity::set(QString("STARTUP"), td);
  
  td = Data(0);
  td.setLabel(QObject::tr("Run Interval"));
  Entity::set(QString("INTERVAL"), td);
  
  td = Data(QString("perl"));
  td.setLabel(QObject::tr("Command"));
  Entity::set(QString("COMMAND"), td);
  
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
  Data file, startup, interval, command, name;
  Entity::toData(QString("NAME"), name);
  Entity::toData(QString("FILE"), file);
  Entity::toData(QString("STARTUP"), startup);
  Entity::toData(QString("INTERVAL"), interval);
  Entity::toData(QString("COMMAND"), command);
  
  EAVDataBase db("scripts");
  db.transaction();

  KeyScriptDataBase skeys;
  Entity data;
  data.setName(name.toString());
  data.set(skeys.indexToString(KeyScriptDataBase::_FILE), file);
  data.set(skeys.indexToString(KeyScriptDataBase::_STARTUP), startup);
  data.set(skeys.indexToString(KeyScriptDataBase::_RUN_INTERVAL), interval);
  data.set(skeys.indexToString(KeyScriptDataBase::_COMMAND), command);
  if (db.set(data))
  {
    qDebug() << "CommandScriptDataBase::save: EAVDataBase error" << name.toString();
    return 1;
  }

  db.commit();
  
  return 0;
}

int CommandScriptDataBase::remove ()
{
  Data name;
  Entity::toData(QString("NAME"), name);

  QStringList tl;
  tl << name.toString();
  
  EAVDataBase db("scripts");
  db.transaction();
  if (db.remove(tl))
  {
    qDebug() << "CommandScriptDataBase::remove: EAVDataBase error" << tl;
    return 1;
  }

  db.commit();
  
  return 0;
}

int CommandScriptDataBase::list ()
{
  QStringList tl;
  EAVDataBase db("scripts");
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
