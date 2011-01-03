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

#include <QtDebug>

DATA_WINDOW::DATA_WINDOW ()
{
  _type = _DIALOG;
  _dw = 0;
  
  _method << "START" << "SET" << "END";
}

int DATA_WINDOW::command (Command *command)
{
  // DATA_WINDOW,<METHOD>
  //     0          1

  if (command->count() < 2)
  {
    qDebug() << "DATA_WINDOW::command: invalid parm count" << command->count();
    return 1;
  }

  switch ((Method) _method.indexOf(command->parm(1)))
  {
    case _START:
      return start(command);
      break;
    case _SET:
      return setData(command);
      break;
    case _END:
      return end(command);
      break;
    default:
      break;
  }

  return 0;
}

int DATA_WINDOW::start (Command *command)
{
  // DATA_WINDOW,START
  //      0        1

  if (_dw)
    return 1;
  
  _dw = new DataWindow;

  command->setReturnData("0");

  emit signalResume();

  return 0;
}

int DATA_WINDOW::setData (Command *command)
{
  // DATA_WINDOW,SET
  //     0        1

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "DATA_WINDOW::command: no indicator";
    return 1;
  }

  _dw->setData(i);

  command->setReturnData("0");

  emit signalResume();

  return 0;
}

int DATA_WINDOW::end (Command *command)
{
  // DATA_WINDOW,END
  //     0        1

  _dw->show();
  _dw->scrollToBottom();

  command->setReturnData("0");

  emit signalResume();

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
