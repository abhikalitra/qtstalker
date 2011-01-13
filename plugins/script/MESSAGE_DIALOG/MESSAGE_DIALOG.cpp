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

#include "MESSAGE_DIALOG.h"
#include "MessageDialog.h"

#include <QtDebug>
#include <QDialog>

MESSAGE_DIALOG::MESSAGE_DIALOG ()
{
  _type = _DIALOG;
}

int MESSAGE_DIALOG::command (Command *command)
{
  // MESSAGE_DIALOG,TITLE,MESSAGE
  //        0         1      2

  if (command->count() != 3)
  {
    qDebug() << "MESSAGE_DIALOG::command: invalid parm count" << command->count();
    return 1;
  }

  MessageDialog *dialog = new MessageDialog(command);
  connect(dialog, SIGNAL(finished(int)), this, SIGNAL(signalResume()));
  dialog->show();

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  MESSAGE_DIALOG *o = new MESSAGE_DIALOG;
  return ((ScriptPlugin *) o);
}
