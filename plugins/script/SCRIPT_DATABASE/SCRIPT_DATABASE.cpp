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

#include "SCRIPT_DATABASE.h"

#include <QtDebug>

SCRIPT_DATABASE::SCRIPT_DATABASE ()
{
  _method << "LOAD" << "SAVE" << "DELETE" << "SCRIPTS";
}

int SCRIPT_DATABASE::command (Command *command)
{
  // SCRIPT_DATABASE,<METHOD>
  //         0          1
  
  if (command->count() < 2)
  {
    qDebug() << "SCRIPT_DATABASE::command: invalid parm count" << command->count();
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
    case _DELETE:
      return deleteScript(command);
      break;
    case _SCRIPTS:
      return scripts(command);
      break;
    default:
      break;
  }

  return 0;
}

int SCRIPT_DATABASE::load (Command *command)
{
  // SCRIPT_DATABASE,LOAD,<NAME>
  //       0          1      2

  if (command->count() != 3)
  {
    qDebug() << "SCRIPT_DATABASE::load: invalid parm count" << command->count();
    return 1;
  }

  Script script;
  script.setName(command->parm(2));

  if (_db.load(&script))
  {
    qDebug() << "SCRIPT_DATABASE::load: ScriptDataBase error";
    return 1;
  }
  
  command->setReturnData(script.toString());

  return 0;
}

int SCRIPT_DATABASE::save (Command *command)
{
  // SCRIPT_DATABASE,SAVE,<NAME>,<FILE>,<COMMAND>
  //        0         1      2      3       4

  if (command->count() != 5)
  {
    qDebug() << "SCRIPT_DATABASE::save: invalid parm count" << command->count();
    return 1;
  }

  int pos = 2;
  Script script;
  script.setName(command->parm(pos++));
  script.setCommand(command->parm(pos++));
  script.setFile(command->parm(pos++));

  if (_db.save(&script))
  {
    qDebug() << "SCRIPT_DATABASE::save: ScriptDataBase error";
    return 1;
  }

  command->setReturnData("0");

  return 0;
}

int SCRIPT_DATABASE::deleteScript (Command *command)
{
  // SCRIPT_DATABASE,DELETE,<NAME>,*
  //        0           1      2    *

  if (command->count() < 3)
  {
    qDebug() << "SCRIPT_DATABASE::deleteScript: invalid parm count" << command->count();
    return 1;
  }

  QStringList l;
  int loop = 2;
  for (; loop < command->count(); loop++)
    l << command->parm(loop);

  if (_db.deleteScript(l))
  {
    qDebug() << "SCRIPT_DATABASE::deleteScript: ScriptDataBase error";
    return 1;
  }

  command->setReturnData("0");
  
  return 0;
}

int SCRIPT_DATABASE::scripts (Command *command)
{
  // SCRIPT_DATABASE,SCRIPTS
  //        0          1

  QStringList l;
  if (_db.scripts(l))
  {
    qDebug() << "SCRIPT_DATABASE::scripts: ScriptDataBase error";
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
  SCRIPT_DATABASE *o = new SCRIPT_DATABASE;
  return ((ScriptPlugin *) o);
}
