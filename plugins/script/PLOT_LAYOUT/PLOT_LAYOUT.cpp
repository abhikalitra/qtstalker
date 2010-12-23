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

#include "PLOT_LAYOUT.h"
#include "Globals.h"

#include <QtDebug>

PLOT_LAYOUT::PLOT_LAYOUT ()
{
  _type = _SERIAL;
  
  _method << "NEW" << "DELETE";
}

int PLOT_LAYOUT::command (Command *command)
{
  // PLOT_LAYOUT,<METHOD>
  //   0     1 
  
  if (command->count() < 2)
  {
    qDebug() << "PLOT_LAYOUT::command: invalid parm count" << command->count();
    return 1;
  }

  switch ((Method) _method.indexOf(command->parm(1)))
  {
    case __NEW:
      return newPlot(command);
      break;
    case _DELETE:
      return deletePlot(command);
      break;
    default:
      break;
  }

  return 0;
}

int PLOT_LAYOUT::newPlot (Command *command)
{
  // PLOT_LAYOUT,NEW,<NAME>
  //       0      1     2

  if (command->count() != 3)
  {
    qDebug() << "PLOT_LAYOUT::newPlot: invalid parm count" << command->count();
    return 1;
  }

  g_middleMan->indicatorNew(command->parm(2));
  
  command->setReturnData("0");

  return 0;
}

int PLOT_LAYOUT::deletePlot (Command *command)
{
  // PLOT_LAYOUT,DELETE,<NAME>,*
  //      0        1      2

  if (command->count() < 3)
  {
    qDebug() << "PLOT_LAYOUT::deletePlot: invalid parm count" << command->count();
    return 1;
  }

  QStringList l;
  int pos = 2;
  for (; pos < command->count(); pos++)
    l << command->parm(pos);
  
  g_middleMan->indicatorDelete(l);

  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  PLOT_LAYOUT *o = new PLOT_LAYOUT;
  return ((ScriptPlugin *) o);
}
