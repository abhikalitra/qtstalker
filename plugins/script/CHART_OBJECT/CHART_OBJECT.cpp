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

#include "CHART_OBJECT.h"
#include "Globals.h"

#include <QtDebug>

CHART_OBJECT::CHART_OBJECT ()
{
  _type = _SERIAL;
  
  _method << "DELETE" << "UPDATE";
}

int CHART_OBJECT::command (Command *command)
{
  // CHART_OBJECT,<METHOD>
  //   0     1 
  
  if (command->count() < 2)
  {
    qDebug() << "CHART_OBJECT::command: invalid parm count" << command->count();
    return 1;
  }

  switch ((Method) _method.indexOf(command->parm(1)))
  {
    case _DELETE:
      return remove(command);
      break;
    case _UPDATE:
      return update(command);
      break;
    default:
      break;
  }

  return 0;
}

int CHART_OBJECT::remove (Command *command)
{
  // CHART_OBJECT,DELETE,<NAME>,*
  //      0         1      2    3*

  if (command->count() < 3)
  {
    qDebug() << "CHART_OBJECT::remove: invalid parm count" << command->count();
    return 1;
  }

  QStringList l;
  int pos = 2;
  for (; pos < command->count(); pos++)
    l << command->parm(pos);

  g_middleMan->chartObjectDelete(l);

  command->setReturnData("0");

  return 0;
}

int CHART_OBJECT::update (Command *command)
{
  // CHART_OBJECT,UPDATE,<NAME>
  //       0         1     2

  if (command->count() != 3)
  {
    qDebug() << "CHART_OBJECT::update: invalid parm count" << command->count();
    return 1;
  }

  g_middleMan->chartObjectUpdate(command->parm(2));

  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  CHART_OBJECT *o = new CHART_OBJECT;
  return ((ScriptPlugin *) o);
}
