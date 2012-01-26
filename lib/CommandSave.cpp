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

#include "CommandSave.h"
#include "Script.h"
#include "EAVDataBase.h"

#include <QtDebug>

CommandSave::CommandSave ()
{
  _name = "SAVE";
}

QString CommandSave::run (CommandParse &sg, void *scr)
{
  // PARM1 = object
  // PARM2 = database table
  
  if (sg.values() != 2)
  {
    qDebug() << "CommandSave::run: invalid number of values";
    return _returnCode;
  }
  
  Script *script = (Script *) scr;
  int pos = 0;
  QString name = sg.value(pos++);
  Command *c = script->scriptCommand(name);
  if (! c)
  {
    qDebug() << "CommandSave::run: invalid object name" << name;
    return _returnCode;
  }

  QString table = sg.value(pos++);
  
  QList<QString> keys;
  c->dkeys(keys);
  
  Entity settings;
  settings.setName(name);
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Data td;
    c->toData(keys.at(loop), td);
    settings.set(keys.at(loop), td);
  }

  EAVDataBase db(table);
  db.transaction();
  if (db.set(settings))
  {
    qDebug() << "CommandSave::run: EAVDataBase error";
    return _returnCode;
  }
  db.commit();

  _returnCode = "OK";
  return _returnCode;
}
