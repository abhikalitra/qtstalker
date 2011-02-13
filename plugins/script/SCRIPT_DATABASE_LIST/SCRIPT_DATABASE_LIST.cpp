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

#include "SCRIPT_DATABASE_LIST.h"
#include "ScriptDataBase.h"

#include <QtDebug>

SCRIPT_DATABASE_LIST::SCRIPT_DATABASE_LIST ()
{
  _plugin = "SCRIPT_DATABASE_LIST";
}

int SCRIPT_DATABASE_LIST::command (Command *command)
{
  // PARMS:

  ScriptDataBase db;
  QStringList l;
  if (db.scripts(l))
  {
    qDebug() << _plugin << "::command: ScriptDataBase error";
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
  SCRIPT_DATABASE_LIST *o = new SCRIPT_DATABASE_LIST;
  return ((ScriptPlugin *) o);
}
