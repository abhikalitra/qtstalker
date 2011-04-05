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

#include "FILE_DIALOG.h"
#include "Globals.h"

#include <QtDebug>
#include <QFileDialog>

FILE_DIALOG::FILE_DIALOG ()
{
  _plugin = "FILE_DIALOG";
  _type = "DIALOG";
}

int FILE_DIALOG::command (Command *command)
{
  // PARMS:
  // MODE
  // DIRECTORY
  // TITLE

  _command = command;

  bool ok;
  _mode = command->parm("MODE").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::command: invalid MODE" << command->parm("MODE");
    return 1;
  }
  
  QFileDialog *dialog = new QFileDialog(g_parent);
  dialog->setDirectory(command->parm("DIRECTORY"));
  dialog->setWindowTitle("QtStalker" + g_session + ": " + command->parm("TITLE"));
  connect(dialog, SIGNAL(filesSelected(const QStringList &)), this, SLOT(filesSelected(QStringList)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  connect(dialog, SIGNAL(rejected()), this, SIGNAL(signalResume()));
  connect(this, SIGNAL(signalKill()), dialog, SLOT(reject()));
  dialog->show();

  return 0;
}

void FILE_DIALOG::filesSelected (QStringList l)
{
  if (_mode)
    _command->setReturnData(_plugin + "_FILE", l.join(";"));
  else
    _command->setReturnData(_plugin + "_FILE", l.at(0));

  _command->setReturnCode("0");

  emit signalResume();
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  FILE_DIALOG *o = new FILE_DIALOG;
  return ((Plugin *) o);
}
