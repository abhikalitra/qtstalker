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

#include "CommandDialog.h"
#include "DataDialog.h"
#include "GlobalParent.h"
#include "Global.h"
#include "DataString.h"

#include <QtDebug>
#include <QDialog>

CommandDialog::CommandDialog (QObject *p) : Command (p)
{
  _name = "DIALOG";
  _type = _WAIT;

  _values.insert(_ParmTypeKey, new DataString());
  _values.insert(_ParmTypeTitle, new DataString());
}

void CommandDialog::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandDialog::runScript: parse error");
    return;
  }

  DataDialog *dialog = new DataDialog(g_parent);

  QStringList l;
  l << "QtStalker" + g_session + ":" << _values.value(_ParmTypeTitle)->toString();
  dialog->setWindowTitle(l.join(" "));

  script->setDialog(_values.value(_ParmTypeKey)->toString(), dialog);

  Command::done(QString());
}
