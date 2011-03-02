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

#include "INTEGER_DIALOG.h"

#include <QtDebug>
#include <QInputDialog>

INTEGER_DIALOG::INTEGER_DIALOG ()
{
  _plugin = "INTEGER_DIALOG";
  _type = _DIALOG;
}

int INTEGER_DIALOG::command (Command *command)
{
  // PARMS:
  // LABEL
  // MIN
  // MAX
  // VALUE

  _command = command;

  QInputDialog *dialog = new QInputDialog(_parent);
  dialog->setLabelText(_command->parm("LABEL"));
  
  QString s = _command->parm("MIN");
  if (! s.isEmpty())
    dialog->setIntMinimum(s.toInt());

  s = _command->parm("MAX");
  if (! s.isEmpty())
    dialog->setIntMaximum(s.toInt());
  
  s = _command->parm("VALUE");
  if (! s.isEmpty())
    dialog->setIntValue(s.toInt());

  connect(dialog, SIGNAL(intValueSelected(int)), this, SLOT(integerSelected(int)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  connect(dialog, SIGNAL(rejected()), this, SIGNAL(signalResume()));
  connect(this, SIGNAL(signalKill()), dialog, SLOT(reject()));
  dialog->show();

  return 0;
}

void INTEGER_DIALOG::integerSelected (int d)
{
  _command->setReturnData(_plugin + "_INTEGER", QString::number(d));
  _command->setReturnCode("0");
  emit signalResume();
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  INTEGER_DIALOG *o = new INTEGER_DIALOG;
  return ((ScriptPlugin *) o);
}
