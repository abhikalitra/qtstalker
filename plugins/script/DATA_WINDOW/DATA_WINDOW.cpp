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

#include "DATA_WINDOW.h"
#include "Globals.h"
#include "DataWindow.h"

#include <QtDebug>

DATA_WINDOW::DATA_WINDOW ()
{
  _type = _DIALOG;
}

int DATA_WINDOW::command (Command *command)
{
  // DATA_WINDOW
  //     0

  DataWindow *dialog = new DataWindow;
  connect(dialog, SIGNAL(signalResume()), this, SIGNAL(signalResume()));
  dialog->setData();
  dialog->show();
  dialog->scrollToBottom();

  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  DATA_WINDOW *o = new DATA_WINDOW;
  return ((ScriptPlugin *) o);
}
