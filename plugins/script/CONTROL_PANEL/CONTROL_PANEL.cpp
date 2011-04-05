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

#include "CONTROL_PANEL.h"
#include "Globals.h"

#include <QtDebug>

CONTROL_PANEL::CONTROL_PANEL ()
{
  _plugin = "CONTROL_PANEL";
  _method << "CHART_PANEL" << "GROUP_REFRESH" << "SCRIPT_RUN";
}

int CONTROL_PANEL::command (Command *command)
{
  int rc = 1;
  switch (_method.indexOf(command->parm("METHOD")))
  {
    case 0:
      rc = chartRefresh(command);
      break;
    case 1:
      rc = groupRefresh(command);
      break;
    case 2:
      rc = scriptRun(command);
      break;
    default:
      break;
  }

  return rc;
}

int CONTROL_PANEL::chartRefresh (Command *command)
{
  g_middleMan->chartPanelRefresh();
  
  command->setReturnCode("0");

  return 0;
}

int CONTROL_PANEL::groupRefresh (Command *command)
{
  g_middleMan->groupPanelRefresh();

  command->setReturnCode("0");

  return 0;
}

int CONTROL_PANEL::scriptRun (Command *command)
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

Plugin * createPlugin ()
{
  CONTROL_PANEL *o = new CONTROL_PANEL;
  return ((Plugin *) o);
}
