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
#include "Globals.h"

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
  int mode = command->parm("MODE").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid mode" << command->parm("MODE");
    return 1;
  }

  QString title = command->parm("TITLE");
  if (title.isEmpty())
    title = tr("Items");

  QStringList items = command->parm("ITEMS").split(";", QString::SkipEmptyParts);

  _command = command;

  QStringList l;
  l << "QtStalker" + g_session + ":" << tr("Select");

  SelectDialog *dialog = new SelectDialog(g_parent);
  dialog->setWindowTitle(l.join(" "));
  dialog->setMode(mode);
  dialog->setTitle(title);
  dialog->setItems(items);
  connect(this, SIGNAL(signalKill()), dialog, SLOT(reject()));
  connect(dialog, SIGNAL(rejected()), this, SIGNAL(signalResume()));
  connect(dialog, SIGNAL(signalDone(QStringList)), this, SLOT(command2(QStringList)));
  dialog->show();

  return 0;
}

void SELECT_DIALOG::command2 (QStringList l)
{
  _command->setReturnData(_plugin + "_SELECTED", l.join(";"));
  _command->setReturnCode("0");
  emit signalResume();
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  SELECT_DIALOG *o = new SELECT_DIALOG;
  return ((Plugin *) o);
}
