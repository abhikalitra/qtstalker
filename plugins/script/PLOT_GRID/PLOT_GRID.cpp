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

#include "PLOT_GRID.h"
#include "Globals.h"

#include <QtDebug>

PLOT_GRID::PLOT_GRID ()
{
  _plugin = "PLOT_GRID";
}

int PLOT_GRID::command (Command *command)
{
  // PARMS:
  // STATUS

  bool ok;
  int status = command->parm("STATUS").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid STATUS" << command->parm("STATUS");
    return 1;
  }

  g_middleMan->grid(status);

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  PLOT_GRID *o = new PLOT_GRID;
  return ((ScriptPlugin *) o);
}
