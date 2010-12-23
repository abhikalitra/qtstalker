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

#include "SETTINGS.h"
#include "Globals.h"

#include <QtDebug>
#include <QSettings>

SETTINGS::SETTINGS ()
{
  _method << "LOAD" << "SAVE";
}

int SETTINGS::command (Command *command)
{
  // SETTINGS,<METHOD>,<KEY>
  //     0       1       2
  
  if (command->count() < 3)
  {
    qDebug() << "SETTINGS::command: invalid parm count" << command->count();
    return 1;
  }

  switch ((Method) _method.indexOf(command->parm(1)))
  {
    case _LOAD:
      return load(command);
      break;
    case _SAVE:
      return save(command);
      break;
    default:
      break;
  }

  return 0;
}

int SETTINGS::load (Command *command)
{
  // SETTINGS,<METHOD>,<KEY>
  //     0       1       2

  if (command->count() != 3)
  {
    qDebug() << "SETTINGS::load: invalid parm count" << command->count();
    return 1;
  }

  QSettings set(g_settingsFile);
  command->setReturnData(set.value(command->parm(2)).toString());

  return 0;
}

int SETTINGS::save (Command *command)
{
  // SETTINGS,<METHOD>,<KEY>,<DATA>
  //     0       1       2      3

  if (command->count() != 4)
  {
    qDebug() << "SETTINGS::save: invalid parm count" << command->count();
    return 1;
  }

  QSettings set(g_settingsFile);
  set.setValue(command->parm(2), command->parm(3));
  set.sync();

  command->setReturnData("0");
  
  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  SETTINGS *o = new SETTINGS;
  return ((ScriptPlugin *) o);
}
