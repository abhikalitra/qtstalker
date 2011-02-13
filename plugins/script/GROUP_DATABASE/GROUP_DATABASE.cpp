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

#include "GROUP_DATABASE.h"

#include <QtDebug>

GROUP_DATABASE::GROUP_DATABASE ()
{
  _plugin = "GROUP_DATABASE";
  _method << "LOAD" << "SAVE" << "SAVE_ALL" << "DELETE" << "GROUPS";
}

int GROUP_DATABASE::command (Command *command)
{
  // PARMS:
  // METHOD

  switch ((Method) _method.indexOf(command->parm("METHOD")))
  {
    case _LOAD:
      return load(command);
      break;
    case _SAVE:
      return save(command);
      break;
    case _SAVE_ALL:
      return saveAll(command);
      break;
    case _DELETE:
      return deleteGroup(command);
      break;
    case _GROUPS:
      return groups(command);
      break;
    default:
      break;
  }

  return 0;
}

int GROUP_DATABASE::load (Command *command)
{
  // PARMS:
  // METHOD
  // NAME

  QString name = command->parm("NAME");
  
  QStringList l;
  if (_db.load(name, l))
  {
    qDebug() << _plugin << "::load: GroupDataBase error";
    return 1;
  }
  
  command->setReturnData(_plugin + "_GROUP", l.join(";"));

  command->setReturnCode("0");
  
  return 0;
}

int GROUP_DATABASE::save (Command *command)
{
  // PARMS:
  // METHOD
  // NAME
  // ITEMS - semicolon delimited string

  QString name = command->parm("NAME");

  QStringList l = command->parm("ITEMS").split(";", QString::SkipEmptyParts);

  if (_db.save(name, l))
  {
    qDebug() << _plugin << "::save: GroupDataBase error";
    return 1;
  }
  
  command->setReturnCode("0");

  return 0;
}

int GROUP_DATABASE::saveAll (Command *command)
{
  // PARMS:
  // METHOD
  // NAME
  // ITEMS -  - semicolon delimited string

  QString name = command->parm("NAME");

  QStringList l = command->parm("ITEMS").split(";", QString::SkipEmptyParts);

  if (_db.saveAll(name, l))
  {
    qDebug() << _plugin << "::saveAll: GroupDataBase error";
    return 1;
  }

  command->setReturnCode("0");

  return 0;
}

int GROUP_DATABASE::deleteGroup (Command *command)
{
  // PARMS:
  // METHOD
  // NAME - semicolon delimited string

  QStringList l = command->parm("NAME").split(";", QString::SkipEmptyParts);

  if (_db.deleteGroup(l))
  {
    qDebug() << _plugin << "::deleteGroup: GroupDataBase error";
    return 1;
  }
  
  command->setReturnCode("0");
  
  return 0;
}

int GROUP_DATABASE::groups (Command *command)
{
  // PARMS:
  // METHOD
  
  QStringList l;
  if (_db.groups(l))
  {
    qDebug() << _plugin << "::groups: GroupDataBase error";
    return 1;
  }

  command->setReturnData(_plugin + "_GROUPS", l.join(";"));

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  GROUP_DATABASE *o = new GROUP_DATABASE;
  return ((ScriptPlugin *) o);
}
