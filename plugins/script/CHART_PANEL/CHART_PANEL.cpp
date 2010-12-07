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

#include "CHART_PANEL.h"
#include "Globals.h"

#include <QtDebug>

CHART_PANEL::CHART_PANEL ()
{
  _method << "REFRESH" << "SELECTED" << "SEARCH";
}

int CHART_PANEL::command (Command *command)
{
  // CHART_PANEL,<METHOD>
  //         0          1
  
  if (command->count() < 2)
  {
    qDebug() << "CHART_PANEL::command: invalid parm count" << command->count();
    return 1;
  }

  switch ((Method) _method.indexOf(command->parm(1)))
  {
    case _REFRESH:
      return refresh(command);
      break;
    case _SELECTED:
      return selected(command);
      break;
    case _SEARCH:
      return search(command);
      break;
    default:
      break;
  }

  return 0;
}

int CHART_PANEL::refresh (Command *command)
{
  // CHART_PANEL,REFRESH
  //      0         1

  g_chartPanel->updateList();
  
  command->setReturnData("0");

  return 0;
}

int CHART_PANEL::selected (Command *command)
{
  // CHART_PANEL,SELECTED
  //      0         1

  QStringList l;
  g_chartPanel->selected(l);

  command->setReturnData(l.join(","));

  return 0;
}

int CHART_PANEL::search (Command *command)
{
  // CHART_PANEL,SEARCH,<EXCHANGE>,<SYMBOL>
  //      0        1        2         3

  if (command->count() != 4)
  {
    qDebug() << "GROUP_DATABASE::command: invalid parm count" << command->count();
    return 1;
  }

  g_chartPanel->setSearch(command->parm(2), command->parm(3));

  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  CHART_PANEL *o = new CHART_PANEL;
  return ((ScriptPlugin *) o);
}
