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

#include "SCRIPT_RETURN_DATA.h"
#include "Globals.h"

#include <QtDebug>

SCRIPT_RETURN_DATA::SCRIPT_RETURN_DATA ()
{
  _plugin = "SCRIPT_RETURN_DATA";
}

int SCRIPT_RETURN_DATA::command (Command *command)
{
  // PARMS:
  // KEY
  
  QString s = command->returnData(command->parm("KEY"));
  command->setReturnCode(s);

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  SCRIPT_RETURN_DATA *o = new SCRIPT_RETURN_DATA;
  return ((Plugin *) o);
}
