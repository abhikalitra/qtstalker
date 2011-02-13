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

#include "GROUP_DATABASE_SAVE_ALL.h"
#include "GroupDataBase.h"

#include <QtDebug>

GROUP_DATABASE_SAVE_ALL::GROUP_DATABASE_SAVE_ALL ()
{
  _plugin = "GROUP_DATABASE_SAVE_ALL";
}

int GROUP_DATABASE_SAVE_ALL::command (Command *command)
{
  // PARMS:
  // NAME
  // ITEMS -  - semicolon delimited string

  QString name = command->parm("NAME");

  QStringList l = command->parm("ITEMS").split(";", QString::SkipEmptyParts);

  GroupDataBase db;
  if (db.saveAll(name, l))
  {
    qDebug() << _plugin << "::saveAll: GroupDataBase error";
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
  GROUP_DATABASE_SAVE_ALL *o = new GROUP_DATABASE_SAVE_ALL;
  return ((ScriptPlugin *) o);
}
