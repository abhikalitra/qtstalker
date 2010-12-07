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

#include "MESSAGE.h"
#include "Message.h"

#include <QtDebug>

MESSAGE::MESSAGE ()
{
}

int MESSAGE::command (Command *command)
{
  // MESSAGE,<TYPE>
  //    0       1

  if (command->count() != 2)
  {
    qDebug() << "MESSAGE::command: invalid parm count " << command->count();
    return 1;
  }

  Message mess;
  int type = mess.stringToType(command->parm(1));
  if (type == -1)
  {
    qDebug() << "MESSAGE::command: invalid type" << command->parm(1);
    return 1;
  }

  mess.sendMessage((Message::Type) type);
  
  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  MESSAGE *o = new MESSAGE;
  return ((ScriptPlugin *) o);
}
