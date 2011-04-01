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

#include "SCRIPT_EDIT_DIALOG.h"
#include "ScriptEditDialog.h"
#include "Globals.h"

#include <QtDebug>
#include <QDialog>

SCRIPT_EDIT_DIALOG::SCRIPT_EDIT_DIALOG ()
{
  _plugin = "SCRIPT_EDIT_DIALOG";
  _type = _DIALOG;
}

int SCRIPT_EDIT_DIALOG::command (Command *command)
{
  // PARMS:
  // NAME

  QString name = command->parm("NAME");
  if (name.isEmpty())
  {
    qDebug() << _plugin << "::command: invalid NAME" << name;
    return 1;
  }

  ScriptEditDialog *dialog = new ScriptEditDialog(g_parent, name);
  connect(this, SIGNAL(signalKill()), dialog, SLOT(reject()));
  connect(dialog, SIGNAL(rejected()), this, SIGNAL(signalResume()));
  connect(dialog, SIGNAL(accepted()), this, SLOT(command2()));
  dialog->show();

  return 0;
}

void SCRIPT_EDIT_DIALOG::command2 ()
{
  _command->setReturnCode("0");
  emit signalResume();
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  SCRIPT_EDIT_DIALOG *o = new SCRIPT_EDIT_DIALOG;
  return ((Plugin *) o);
}
