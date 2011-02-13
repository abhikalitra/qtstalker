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
  _plugin = "SCRIPT_DATABASE";
  _method << "LOAD" << "SAVE" << "DELETE" << "SCRIPTS";
  _field << "MINUTES";
}

int SCRIPT_DATABASE::command (Command *command)
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
  // PARMS:
  // METHOD (LOAD)
  // NAME
  // FIELD

  Script script;
  script.setName(command->parm("NAME"));

  if (_db.load(&script))
  {
    qDebug() << _plugin << "::load: script load error" << command->parm("NAME");
    return 1;
  }
  
  switch (_field.indexOf(command->parm("FIELD")))
  {
    case 0: // MINUTES
      command->setReturnData(_plugin + "_SCRIPT", script.toString());
      command->setReturnCode("0");
      return 0;
      break;
    default:
      qDebug() << _plugin << "::load: invalid FIELD" << command->parm("FIELD");
      break;
  }
  
  return 1;
}

int SCRIPT_DATABASE::save (Command *command)
{
  // PARMS:
  // METHOD (SAVE)
  // NAME
  // FIELD
  // DATA

  Script script;
  script.setName(command->parm("NAME"));
  
  if (_db.load(&script))
  {
    qDebug() << _plugin << "::save: load error" << command->parm("NAME");
    return 1;
  }

  switch (_field.indexOf(command->parm("FIELD")))
  {
    case 0: // MINUTES
    {
      bool ok;
      int minutes = command->parm("DATA").toInt(&ok);
      if (! ok)
      {
        qDebug() << _plugin << "::save: invalid MINUTES" << command->parm("DATA");
        return 1;
      }

      script.setMinutes(minutes);
      break;
    }
    default:
      qDebug() << _plugin << "::load: invalid FIELD" << command->parm("FIELD");
      return 1;
      break;
  }

  if (_db.save(&script))
  {
    qDebug() << _plugin << "::save: save error";
    return 1;
  }

  command->setReturnCode("0");

  return 0;
}

int SCRIPT_DATABASE::deleteScript (Command *command)
{
  // PARMS:
  // METHOD (DELETE)
  // NAME - colon delemited list

  QStringList l = command->parm("NAME").split(";", QString::SkipEmptyParts);

  if (_db.deleteScript(l))
  {
    qDebug() << _plugin << "::deleteScript: ScriptDataBase error";
    return 1;
  }

  command->setReturnCode("0");
  
  return 0;
}

int SCRIPT_DATABASE::scripts (Command *command)
{
  // PARMS:
  // METHOD (SCRIPTS)

  QStringList l;
  if (_db.scripts(l))
  {
    qDebug() << _plugin << "::scripts: ScriptDataBase error";
    return 1;
  }
  
  command->setReturnData(_plugin + "_SCRIPTS", l.join(";"));

  command->setReturnCode("0");

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
