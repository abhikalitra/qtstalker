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

#include "GROUP_DATABASE_LOAD.h"
#include "DataDataBase.h"

#include <QtDebug>

GROUP_DATABASE_LOAD::GROUP_DATABASE_LOAD ()
{
  _plugin = "GROUP_DATABASE_LOAD";
}

int GROUP_DATABASE_LOAD::command (Command *command)
{
  // PARMS:
  // NAME

  QString name = command->parm("NAME");

  DataDataBase db("groups");
  QStringList l;
  if (db.load(name, l))
  {
    qDebug() << _plugin << "::command: DataDataBase error";
    return 1;
  }
  
  command->setReturnData(_plugin + "_GROUP", l.join(";"));

  command->setReturnCode("0");
  
  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  GROUP_DATABASE_LOAD *o = new GROUP_DATABASE_LOAD;
  return ((Plugin *) o);
}
