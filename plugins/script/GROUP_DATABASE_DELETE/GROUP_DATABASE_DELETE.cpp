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

#include "GROUP_DATABASE_DELETE.h"
#include "GroupDataBase.h"

#include <QtDebug>

GROUP_DATABASE_DELETE::GROUP_DATABASE_DELETE ()
{
  _plugin = "GROUP_DATABASE_DELETE";
}

int GROUP_DATABASE_DELETE::command (Command *command)
{
  // PARMS:
  // NAME - semicolon delimited string

  QStringList l = command->parm("NAME").split(";", QString::SkipEmptyParts);

  GroupDataBase db;
  if (db.deleteGroup(l))
  {
    qDebug() << _plugin << "::command: GroupDataBase error";
    return 1;
  }
  
  command->setReturnCode("0");
  
  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  GROUP_DATABASE_DELETE *o = new GROUP_DATABASE_DELETE;
  return ((ScriptPlugin *) o);
}
