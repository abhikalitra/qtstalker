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

#include "SCRIPT_DATABASE_LOAD.h"
#include "ScriptDataBase.h"

#include <QtDebug>

SCRIPT_DATABASE_LOAD::SCRIPT_DATABASE_LOAD ()
{
  _plugin = "SCRIPT_DATABASE_LOAD";
}

int SCRIPT_DATABASE_LOAD::command (Command *command)
{
  // PARMS:
  // NAME
  // FIELD

  Script script(0);
  script.setName(command->parm("NAME"));

  ScriptDataBase db;
  if (db.load(&script))
  {
    qDebug() << _plugin << "::command: script load error" << command->parm("NAME");
    return 1;
  }

  QStringList field;
  field << "MINUTES";
  
  switch (field.indexOf(command->parm("FIELD")))
  {
    case 0: // MINUTES
      command->setReturnData(_plugin + "_FIELD", QString::number(script.minutes()));
      command->setReturnCode("0");
      return 0;
      break;
    default:
      qDebug() << _plugin << "::command: invalid FIELD" << command->parm("FIELD");
      break;
  }
  
  return 1;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  SCRIPT_DATABASE_LOAD *o = new SCRIPT_DATABASE_LOAD;
  return ((ScriptPlugin *) o);
}
