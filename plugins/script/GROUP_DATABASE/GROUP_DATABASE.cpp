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
  _method << "LOAD" << "SAVE" << "SAVE_ALL" << "DELETE" << "GROUPS";
}

int GROUP_DATABASE::command (Command *command)
{
  // GROUP_DATABASE,<METHOD>
  //         0          1
  
  if (command->count() < 2)
  {
    qDebug() << "GROUP_DATABASE::command: invalid parm count" << command->count();
    return 1;
  }

  switch ((Method) _method.indexOf(command->parm(1)))
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
  // GROUP_DATABASE,<LOAD>,<NAME>
  //       0          1       2

  if (command->count() != 3)
  {
    qDebug() << "GROUP_DATABASE::load: invalid parm count" << command->count();
    return 1;
  }

  QString name = command->parm(2);
  
  QStringList l;
  if (_db.load(name, l))
  {
    qDebug() << "GROUP_DATABASE::load: GroupDataBase error";
    return 1;
  }
  
  command->setReturnData(l.join(","));

  return 0;
}

int GROUP_DATABASE::save (Command *command)
{
  // GROUP_DATABASE,<SAVE>,<NAME>,*
  //        0          1       2    *

  if (command->count() < 3)
  {
    qDebug() << "GROUP_DATABASE::save: invalid parm count" << command->count();
    return 1;
  }

  QString name = command->parm(2);

  QStringList l;
  int loop = 3;
  for (; loop < command->count(); loop++)
    l << command->parm(loop);

  if (_db.save(name, l))
  {
    qDebug() << "GROUP_DATABASE::save: GroupDataBase error";
    return 1;
  }
  
  command->setReturnData("0");

  return 0;
}

int GROUP_DATABASE::saveAll (Command *command)
{
  // GROUP_DATABASE,SAVE_ALL,<NAME>,*
  //        0          1       2    *

  if (command->count() < 3)
  {
    qDebug() << "GROUP_DATABASE::saveAll: invalid parm count" << command->count();
    return 1;
  }

  QString name = command->parm(2);

  QStringList l;
  int loop = 3;
  for (; loop < command->count(); loop++)
    l << command->parm(loop);

  if (_db.saveAll(name, l))
  {
    qDebug() << "GROUP_DATABASE::saveAll: GroupDataBase error";
    return 1;
  }

  command->setReturnData("0");

  return 0;
}

int GROUP_DATABASE::deleteGroup (Command *command)
{
  // GROUP_DATABASE,<DELETE>,<NAME>,*
  //        0          1       2    *

  if (command->count() < 3)
  {
    qDebug() << "GROUP_DATABASE::deleteGroup: invalid parm count" << command->count();
    return 1;
  }

  QStringList l;
  int loop = 2;
  for (; loop < command->count(); loop++)
    l << command->parm(loop);

  if (_db.deleteGroup(l))
  {
    qDebug() << "GROUP_DATABASE::deleteGroup: GroupDataBase error";
    return 1;
  }
  
  command->setReturnData("0");
  
  return 0;
}

int GROUP_DATABASE::groups (Command *command)
{
  // GROUP_DATABASE,<GROUPS>
  //        0          1
  
  QStringList l;
  if (_db.groups(l))
  {
    qDebug() << "GROUP_DATABASE::groups: GroupDataBase error";
    return 1;
  }

  command->setReturnData(l.join(","));

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
