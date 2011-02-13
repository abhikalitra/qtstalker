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

#include "SCRIPT_PANEL_RUN.h"
#include "Globals.h"

#include <QtDebug>

SCRIPT_PANEL_RUN::SCRIPT_PANEL_RUN ()
{
  _plugin = "SCRIPT_PANEL_RUN";
}

int SCRIPT_PANEL_RUN::command (Command *command)
{
  // PARMS:
  // NAME

  g_middleMan->scriptRun(command->parm("NAME"));
  
  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  SCRIPT_PANEL_RUN *o = new SCRIPT_PANEL_RUN;
  return ((ScriptPlugin *) o);
}
