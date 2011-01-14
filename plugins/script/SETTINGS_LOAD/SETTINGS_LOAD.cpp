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

#include "SETTINGS_LOAD.h"
#include "Globals.h"

#include <QtDebug>
#include <QSettings>

SETTINGS_LOAD::SETTINGS_LOAD ()
{
}

int SETTINGS_LOAD::command (Command *command)
{
  // SETTINGS_LOAD,<KEY>
  //       0         1

  if (command->count() != 2)
  {
    qDebug() << "SETTINGS_LOAD::command: invalid parm count" << command->count();
    return 1;
  }

  QSettings set(g_settingsFile);
  command->setReturnData(set.value(command->parm(1)).toString());

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  SETTINGS_LOAD *o = new SETTINGS_LOAD;
  return ((ScriptPlugin *) o);
}
