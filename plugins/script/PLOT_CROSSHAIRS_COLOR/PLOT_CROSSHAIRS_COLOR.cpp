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

#include "PLOT_CROSSHAIRS_COLOR.h"
#include "Globals.h"

#include <QtDebug>

PLOT_CROSSHAIRS_COLOR::PLOT_CROSSHAIRS_COLOR ()
{
  _plugin = "PLOT_CROSSHAIRS_COLOR";
}

int PLOT_CROSSHAIRS_COLOR::command (Command *command)
{
  // PARMS:
  // COLOR

  QColor c(command->parm("COLOR"));
  if (! c.isValid())
  {
    qDebug() << _plugin << "::command: invalid COLOR" << command->parm("COLOR");
    return 1;
  }

  g_middleMan->crosshairsColor(c);

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  PLOT_CROSSHAIRS_COLOR *o = new PLOT_CROSSHAIRS_COLOR;
  return ((ScriptPlugin *) o);
}
