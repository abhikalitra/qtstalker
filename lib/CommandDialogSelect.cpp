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

#include "CommandDialogSelect.h"
#include "Script.h"
#include "TypeThreadMessage.h"
#include "ThreadMessageFunctions.h"

#include <QtDebug>
#include <QUuid>

CommandDialogSelect::CommandDialogSelect ()
{
  _name = "DIALOG_SELECT";

  Data td(TypeData::_LIST);
  Entity::set(QString("LIST"), td);

  td = Data(QString());
  Entity::set(QString("TITLE"), td);

  td = Data(0);
  Entity::set(QString("MODE"), td);
}

QString CommandDialogSelect::run (CommandParse &, void *scr)
{
  Script *script = (Script *) scr;
  
  Data list, title, mode;
  Entity::toData(QString("LIST"), list);
  Entity::toData(QString("TITLE"), title);
  Entity::toData(QString("MODE"), mode);
  
  Entity dialog;
  dialog.set(QString("MESSAGE"), Data(TypeThreadMessage::_DIALOG_SELECT));
  dialog.set(QString("LIST"), list);
  dialog.set(QString("TITLE"), title);
  dialog.set(QString("MODE"), mode);
  
  QString id = QUuid::createUuid().toString();
  
  ThreadMessageFunctions tmf;
  if (tmf.sendReturn(id, dialog, script))
    return _returnCode;
    
  dialog.toData(QString("LIST"), list);
  set(QString("LIST"), list);

  _returnCode = "OK";
  return _returnCode;
}
