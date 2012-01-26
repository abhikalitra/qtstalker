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

#include "Command.h"
#include "TypeEntity.h"
#include "TypeData.h"

#include <QtDebug>

Command::Command ()
{
  _type = TypeEntity::_COMMAND;
  _returnCode = "ERROR";
}

QString Command::run (CommandParse &, void *)
{
  return _returnCode;
}

int Command::setData (CommandParse &sg)
{
  // format object, setting, value, setting, value...
  
  if (sg.values() < 3)
  {
    qDebug() << "Command::set: invalid number of parms";
    return 1;
  }
  
  int rem = (sg.values() - 1) % 2;
  if (rem != 0)
  {
    qDebug() << "Command::set: invalid number of parms";
    return 1;
  }

  int loop = 1; // skip past object name
  for (; loop < sg.values(); loop += 2)
  {
    int pos = loop;
    QString settingName = sg.value(pos++);
    Data setting;
    if (toData(settingName, setting))
    {
      qDebug() << "Command::set: invalid setting name" << settingName;
      return 1;
    }
  
    QString newValue = sg.value(pos);
    
    switch ((TypeData::Key) setting.type())
    {
      case TypeData::_FILE:
	setting.set(newValue.split(";"));
        break;
      default:
        if (setting.set(newValue, setting.type()))
        {
          qDebug() << "Command::set: invalid value";
          return 1;
        }
        break;
    }
    
    set(settingName, setting);
  }

  return 0;
}
