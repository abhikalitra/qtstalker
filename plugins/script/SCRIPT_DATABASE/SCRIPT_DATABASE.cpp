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
#include "DataDataBase.h"
#include "Script.h"

#include <QtDebug>

SCRIPT_DATABASE::SCRIPT_DATABASE ()
{
  _plugin = "SCRIPT_DATABASE";
  _method << "DELETE" << "LIST" << "LOAD" << "SAVE";
}

int SCRIPT_DATABASE::command (Command *command)
{
  int rc = 1;
  switch (_method.indexOf(command->parm("METHOD")))
  {
    case 0:
      rc = remove(command);
      break;
    case 1:
      rc = list(command);
      break;
    case 2:
      rc = load(command);
      break;
    case 3:
      rc = save(command);
      break;
    default:
      break;
  }

  return rc;
}

int SCRIPT_DATABASE::remove (Command *command)
{
  // PARMS:
  // NAME - colon delimited list

  QStringList l = command->parm("NAME").split(";", QString::SkipEmptyParts);

  DataDataBase db("scripts");
  db.transaction();
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    if (db.removeName(l.at(loop)))
    {
      qDebug() << _plugin << "::remove: ScriptDataBase error";
      return 1;
    }
  }
  db.commit();

  command->setReturnCode("0");

  return 0;
}

int SCRIPT_DATABASE::list (Command *command)
{
  // PARMS:

  DataDataBase db("scripts");
  QStringList l;
  if (db.names(l))
  {
    qDebug() << _plugin << "::list: ScriptDataBase error";
    return 1;
  }

  command->setReturnData(_plugin + "_SCRIPTS", l.join(";"));

  command->setReturnCode("0");

  return 0;
}

int SCRIPT_DATABASE::load (Command *command)
{
  // PARMS:
  // NAME
  // FIELD

  Script script;
  script.setName(command->parm("NAME"));
  if (script.load())
  {
    qDebug() << _plugin << "::load: script load error" << command->parm("NAME");
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
      qDebug() << _plugin << "::load: invalid FIELD" << command->parm("FIELD");
      break;
  }

  return 1;
}

int SCRIPT_DATABASE::save (Command *command)
{
  // PARMS:
  // NAME
  // FIELD
  // DATA

  Script script;
  script.setName(command->parm("NAME"));
  if (script.load())
  {
    qDebug() << _plugin << "::save: load error" << command->parm("NAME");
    return 1;
  }

  QStringList field;
  field << "MINUTES";

  switch (field.indexOf(command->parm("FIELD")))
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
      qDebug() << _plugin << "::save: invalid FIELD" << command->parm("FIELD");
      return 1;
      break;
  }

  if (script.save())
  {
    qDebug() << _plugin << "::save: save error";
    return 1;
  }

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  SCRIPT_DATABASE *o = new SCRIPT_DATABASE;
  return ((Plugin *) o);
}
