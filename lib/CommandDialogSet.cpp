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

#include "CommandDialogSet.h"
#include "DataDialog.h"
#include "DataString.h"
#include "DataInteger.h"

#include <QtDebug>

CommandDialogSet::CommandDialogSet (QObject *p) : Command (p)
{
  _name = "DIALOG_SET";
  _type = _WAIT;

  _values.insert(_ParmTypeKey, new DataString());
  _values.insert(_ParmTypeSetting, new DataString());
  _values.insert(_ParmTypeLabel, new DataString());
  _values.insert(_ParmTypeTab, new DataInteger(0));
}

void CommandDialogSet::runScript (CommandParse sg, Script *script)
{
  // we need to verify values manually due to conflict with Data objects returning values
  // we want the object not the value

  if (sg.values() != _values.count())
  {
    qDebug() << "CommandDialogSet::runScript: invalid number of values";
    return;
  }

  // verify dialog key
  QString s = sg.value(_ParmTypeKey);
  DataDialog *dialog = script->dialog(s);
  if (! dialog)
  {
    Command::error("CommandDialogSet::runScript: invalid value pos 0 " + s);
    return;
  }

  // verify setting key
  s = sg.value(_ParmTypeSetting);
  Data *d = script->data(s);
  if (! d)
  {
    Command::error("CommandDialogSet::runScript: invalid value pos 1 " + s);
    return;
  }

  // verify label
  s = sg.value(_ParmTypeLabel);
  d->setLabel(s);

  // verify tab
  s = sg.value(_ParmTypeTab);
  DataInteger di;
  if (di.set(s))
  {
    Command::error("CommandDialogSet::runScript: invalid value pos 3 " + s);
    return;
  }
  d->setTab(di.toInteger());

  dialog->set(d);

  Command::done(QString());
}
