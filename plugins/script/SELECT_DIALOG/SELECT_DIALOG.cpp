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

#include "SELECT_DIALOG.h"
#include "SelectDialog.h"

#include <QtDebug>
#include <QDialog>

SELECT_DIALOG::SELECT_DIALOG ()
{
  _plugin = "SELECT_DIALOG";
  _type = _DIALOG;
}

int SELECT_DIALOG::command (Command *command)
{
  // PARMS:
  // MODE
  // TITLE
  // ITEMS - semicolon delimited list


  bool ok;
  command->parm("MODE").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid mode" << command->parm("MODE");
    return 1;
  }

  QString s = command->parm("TITLE");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::command: invalid TITLE" << s;
    return 1;
  }

  SelectDialog *dialog = new SelectDialog(_parent, command);
  connect(dialog, SIGNAL(finished(int)), this, SIGNAL(signalResume()));
  connect(this, SIGNAL(signalKill()), dialog, SLOT(reject()));
  dialog->show();

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  SELECT_DIALOG *o = new SELECT_DIALOG;
  return ((ScriptPlugin *) o);
}
