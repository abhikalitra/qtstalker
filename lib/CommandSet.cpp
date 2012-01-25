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

#include "CommandSet.h"
#include "Script.h"

#include <QtDebug>

CommandSet::CommandSet ()
{
  _name = "SET";
}

QString CommandSet::run (CommandParse &sg, void *scr)
{
  Script *script = (Script *) scr;
  int pos = 0;
  QString name = sg.value(pos);
  Command *c = script->scriptCommand(name);
  if (! c)
  {
    qDebug() << "CommandSet::run: invalid object name" << name;
    return _returnCode;
  }

  if (c->setData(sg))
    return _returnCode;
  
  _returnCode = "OK";
  return _returnCode;
}

/*
QString CommandSet::run (CommandParse &sg, void *scr)
{
  Script *script = (Script *) scr;
  int loop = 0;
  for (; loop < sg.values(); loop += 3)
  {
    int pos = loop;
    QString name = sg.value(pos++);
    Command *c = script->scriptCommand(name);
    if (! c)
    {
      qDebug() << "CommandSet::run: invalid object name" << name;
      return _returnCode;
    }

    QString settingName = sg.value(pos++);
    Data setting;
    if (c->toData(settingName, setting))
    {
      qDebug() << "CommandSet::run: invalid setting name" << settingName;
      return _returnCode;
    }
  
    QString newValue = sg.value(pos++);
    
    switch ((TypeData::Key) setting.type())
    {
//      case TypeData::_LIST:
      case TypeData::_FILE:
	setting.set(newValue.split(";"));
        break;
      default:
        if (setting.set(newValue, setting.type()))
        {
          qDebug() << "CommandSet::run: invalid value";
          return _returnCode;
        }
        break;
    }
    
    c->set(settingName, setting);
  }

  _returnCode = "OK";
  return _returnCode;
}
*/