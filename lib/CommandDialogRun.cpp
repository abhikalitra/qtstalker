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

#include "CommandDialogRun.h"
#include "DataDialog.h"
#include "DataString.h"

#include <QtDebug>

CommandDialogRun::CommandDialogRun (QObject *p) : Command (p)
{
  _name = "DIALOG_RUN";
  _type = _WAIT;

  _values.insert(_ParmTypeKey, new DataString());
}

void CommandDialogRun::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandDialogRun::runScript: parse error");
    return;
  }

  QString s = _values.value(_ParmTypeKey)->toString();
  DataDialog *dialog = script->dialog(s);
  if (! dialog)
  {
    Command::error("CommandDialogRun::runScript: invalid value pos 0 " + s);
    return;
  }

  connect(dialog, SIGNAL(accepted()), this, SLOT(dialogAccepted()));
  connect(dialog, SIGNAL(rejected()), this, SLOT(dialogRejected()));

  dialog->show();
}

void CommandDialogRun::dialogAccepted ()
{
  Command::done(QString());
}

void CommandDialogRun::dialogRejected ()
{
  Command::error(QString());
}
