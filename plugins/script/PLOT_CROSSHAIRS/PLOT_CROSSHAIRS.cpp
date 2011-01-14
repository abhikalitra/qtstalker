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

#include "PLOT_CROSSHAIRS.h"
#include "Globals.h"

#include <QtDebug>

PLOT_CROSSHAIRS::PLOT_CROSSHAIRS ()
{
}

int PLOT_CROSSHAIRS::command (Command *command)
{
  // PLOT_CROSSHAIRS,<STATUS>
  //         0          1

  if (command->count() != 2)
  {
    qDebug() << "PLOT_CROSSHAIRS::command: invalid parm count" << command->count();
    return 1;
  }

  bool ok;
  int status = command->parm(1).toInt(&ok);
  if (! ok)
  {
    qDebug() << "PLOT_CROSSHAIRS::command: invalid status" << command->parm(2);
    return 1;
  }

  g_middleMan->crosshairs(status);

  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  PLOT_CROSSHAIRS *o = new PLOT_CROSSHAIRS;
  return ((ScriptPlugin *) o);
}
