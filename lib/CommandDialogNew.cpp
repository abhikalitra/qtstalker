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

#include "CommandDialogNew.h"
#include "Script.h"
#include "TypeThreadMessage.h"
#include "ThreadMessageFunctions.h"

#include <QtDebug>
#include <QUuid>

CommandDialogNew::CommandDialogNew ()
{
  _name = "DIALOG_NEW";

  Data td(TypeData::_LIST);
  Entity::set(QString("LIST"), td);

  td = Data(QString("New Item"));
  Entity::set(QString("NAME"), td);
}

QString CommandDialogNew::run (CommandParse &, void *scr)
{
  Script *script = (Script *) scr;
  
  Data list, name;
  Entity::toData(QString("LIST"), list);
  Entity::toData(QString("NAME"), name);
  
  Entity dialog;
  dialog.set(QString("MESSAGE"), Data(TypeThreadMessage::_DIALOG_NEW));
  dialog.set(QString("LIST"), list);
  dialog.set(QString("NAME"), name);
  
  QString id = QUuid::createUuid().toString();
  
  ThreadMessageFunctions tmf;
  if (tmf.sendReturn(id, dialog, script))
    return _returnCode;
  
  dialog.toData(QString("NAME"), name);
  set(QString("NAME"), name);

  _returnCode = "OK";
  return _returnCode;
}
