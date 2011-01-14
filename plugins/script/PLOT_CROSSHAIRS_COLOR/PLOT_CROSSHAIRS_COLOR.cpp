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
}

int PLOT_CROSSHAIRS_COLOR::command (Command *command)
{
  // PLOT_CROSSHAIRS_COLOR,<COLOR>
  //          0               1

  if (command->count() != 2)
  {
    qDebug() << "PLOT_CROSSHAIRS_COLOR::command: invalid parm count" << command->count();
    return 1;
  }

  QColor c(command->parm(1));
  if (! c.isValid())
  {
    qDebug() << "PLOT_CROSSHAIRS_COLOR::command: invalid color" << command->parm(2);
    return 1;
  }

  g_middleMan->crosshairsColor(c);

  command->setReturnData("0");

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
