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

#include "GROUP_EDIT_DIALOG.h"
#include "GroupEditDialog.h"

#include <QtDebug>
#include <QDialog>

GROUP_EDIT_DIALOG::GROUP_EDIT_DIALOG ()
{
  _plugin = "GROUP_EDIT_DIALOG";
  _type = _DIALOG;
}

int GROUP_EDIT_DIALOG::command (Command *command)
{
  // PARMS:
  // NAME

  QString name = command->parm("NAME");
  if (name.isEmpty())
  {
    qDebug() << _plugin << "::command: invalid NAME";
    return 1;
  }

  GroupEditDialog *dialog = new GroupEditDialog(command);
  connect(dialog, SIGNAL(finished(int)), this, SIGNAL(signalResume()));
  dialog->show();

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  GROUP_EDIT_DIALOG *o = new GROUP_EDIT_DIALOG;
  return ((ScriptPlugin *) o);
}
