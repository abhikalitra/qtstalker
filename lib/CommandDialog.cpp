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
#include "Script.h"
#include "TypeThreadMessage.h"
#include "ThreadMessageFunctions.h"

#include <QtDebug>
#include <QUuid>

CommandDialog::CommandDialog ()
{
  _name = "DIALOG";
}

QString CommandDialog::run (CommandParse &sg, void *scr)
{
  if (sg.values() != 1)
  {
    qDebug() << "CommandDialog::run: invalid number of values";
    return _returnCode;
  }
  
  Script *script = (Script *) scr;
  
  int pos = 0;
  QString name = sg.value(pos++);
  Command *c = script->scriptCommand(name);
  if (! c)
  {
    qDebug() << "CommandDialog::run: invalid entity" << name;
    return _returnCode;
  }

  Entity dialog;
  dialog.set(QString("MESSAGE"), Data(TypeThreadMessage::_DIALOG));
  
  QList<QString> keys;
  c->dkeys(keys);
  
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Data td;
    c->toData(keys.at(loop), td);
    dialog.set(keys.at(loop), td);
  }
  
  QString id = QUuid::createUuid().toString();
  
  ThreadMessageFunctions tmf;
  if (tmf.sendReturn(id, dialog, script))
    return _returnCode;
  
  // copy new data into entity
  for (loop = 0; loop < keys.size(); loop++)
  {
    Data td;
    dialog.toData(keys.at(loop), td);
    c->set(keys.at(loop), td);
  }

  _returnCode = "OK";
  return _returnCode;
}
