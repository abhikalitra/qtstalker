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

#include "GROUP_PANEL.h"
#include "Globals.h"

#include <QtDebug>

GROUP_PANEL::GROUP_PANEL ()
{
  _method << "REFRESH";
}

int GROUP_PANEL::command (Command *command)
{
  // GROUP_PANEL,<METHOD>
  //         0          1
  
  if (command->count() < 2)
  {
    qDebug() << "GROUP_PANEL::command: invalid parm count" << command->count();
    return 1;
  }

  switch ((Method) _method.indexOf(command->parm(1)))
  {
    case _REFRESH:
      return refresh(command);
      break;
    default:
      break;
  }

  return 0;
}

int GROUP_PANEL::refresh (Command *command)
{
  // GROUP_PANEL,REFRESH
  //      0         1

  g_middleMan->groupPanelRefresh();
  
  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  GROUP_PANEL *o = new GROUP_PANEL;
  return ((ScriptPlugin *) o);
}