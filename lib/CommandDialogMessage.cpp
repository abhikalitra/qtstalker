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

#include "CommandDialogMessage.h"
#include "ThreadMessage.h"
#include "TypeThreadMessage.h"

#include <QtDebug>

CommandDialogMessage::CommandDialogMessage ()
{
  _name = "DIALOG_MESSAGE";

  Data td;
  Entity::set(QString("TEXT"), td);
}

QString CommandDialogMessage::run (CommandParse &, void *d)
{
  Data td;
  Entity::toData(QString("TEXT"), td);
  
  Entity e;
  e.set(QString("MESSAGE"), Data(TypeThreadMessage::_DIALOG_MESSAGE));
  e.set(QString("TEXT"), td);
  
  Script *script = (Script *) d;
  
  ThreadMessage tm;
  tm.sendMessage(e, script);
  
  _returnCode = "OK";
  return _returnCode;
}
