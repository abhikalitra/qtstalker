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
  _plugin = "SETTINGS";
  _method << "LOAD" << "SAVE";
}

int SETTINGS::command (Command *command)
{
  int rc = 1;
  switch (_method.indexOf(command->parm("METHOD")))
  {
    case 0:
      rc = load(command);
      break;
    case 1:
      rc = save(command);
      break;
    default:
      break;
  }

  return rc;
}

int SETTINGS::load (Command *command)
{
  // PARMS:
  // KEY

  QSettings set(g_localSettings);
  command->setReturnData(_plugin + "_DATA", set.value(command->parm("KEY")).toString());

  command->setReturnCode("0");

  return 0;
}

int SETTINGS::save (Command *command)
{
  // PARMS:
  // KEY
  // DATA

  QSettings set(g_localSettings);
  set.setValue(command->parm("KEY"), command->parm("DATA"));
  set.sync();

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  SETTINGS *o = new SETTINGS;
  return ((Plugin *) o);
}
