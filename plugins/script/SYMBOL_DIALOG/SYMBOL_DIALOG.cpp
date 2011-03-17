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

#include "SYMBOL_DIALOG.h"
#include "SymbolDialog.h"

#include <QtDebug>
#include <QDialog>

SYMBOL_DIALOG::SYMBOL_DIALOG ()
{
  _plugin = "SYMBOL_DIALOG";
  _type = _DIALOG;
}

int SYMBOL_DIALOG::command (Command *command)
{
  _command = command;

  SymbolDialog *dialog = new SymbolDialog(_parent);
  connect(dialog, SIGNAL(signalDone(QString, QString, QStringList)), this, SLOT(dialogDone(QString, QString, QStringList)));
  connect(this, SIGNAL(signalKill()), dialog, SLOT(reject()));
  dialog->show();

  return 0;
}

void SYMBOL_DIALOG::dialogDone (QString ex, QString sym, QStringList l)
{
  _command->setReturnData("SYMBOL_DIALOG_EXCHANGE", ex);

  if (sym.isEmpty())
    _command->setReturnData("SYMBOL_DIALOG_SYMBOL", "*");
  else
    _command->setReturnData("SYMBOL_DIALOG_SYMBOL", sym);

  _command->setReturnData("SYMBOL_DIALOG_SYMBOLS", l.join(";"));

  _command->setReturnCode("0");

  emit signalResume();
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  SYMBOL_DIALOG *o = new SYMBOL_DIALOG;
  return ((ScriptPlugin *) o);
}
