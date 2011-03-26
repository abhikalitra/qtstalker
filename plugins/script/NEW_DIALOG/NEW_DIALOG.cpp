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

#include "NEW_DIALOG.h"
#include "NewDialog.h"
#include "Globals.h"

#include <QtDebug>

NEW_DIALOG::NEW_DIALOG ()
{
  _plugin = "NEW_DIALOG";
  _type = _DIALOG;
}

int NEW_DIALOG::command (Command *command)
{
  // PARMS:
  // TITLE
  // ITEMS

  QString title = command->parm("TITLE");
  if (title.isEmpty())
    title = tr("New item name");

  QStringList items = command->parm("ITEMS").split(";", QString::SkipEmptyParts);

  _command = command;

  QStringList l;
  l << "QtStalker" + g_session + ":" << tr("New");
  
  NewDialog *dialog = new NewDialog(_parent);
  dialog->setWindowTitle(l.join(" "));
  dialog->setTitle(title);
  dialog->setItems(items);
  connect(this, SIGNAL(signalKill()), dialog, SLOT(reject()));
  connect(dialog, SIGNAL(rejected()), this, SIGNAL(signalResume()));
  connect(dialog, SIGNAL(signalDone(QString)), this, SLOT(command2(QString)));
  dialog->show();

  return 0;
}

void NEW_DIALOG::command2 (QString d)
{
  _command->setReturnData(_plugin + "_NAME", d);
  _command->setReturnCode("0");
  emit signalResume();
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  NEW_DIALOG *o = new NEW_DIALOG;
  return ((Plugin *) o);
}
