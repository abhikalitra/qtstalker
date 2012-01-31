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
#include "Script.h"

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

int Command::setData (CommandParse &sg, void *d)
{
  // format 1: object.setting, object.setting
  // format 2: object.setting, text

  if (sg.values() < 2)
  {
    qDebug() << "Command::set: invalid number of parms";
    return 1;
  }
  
  int rem = sg.values() % 2;
  if (rem != 0)
  {
    qDebug() << "Command::set: invalid number of parms";
    return 1;
  }

  Script *script = (Script *) d;
  int loop = 0;
  for (; loop < sg.values(); loop += 2)
  {
    // setting 
    int pos = loop;
    QString ts = sg.value(pos++);
    QStringList l = ts.split(".", QString::SkipEmptyParts);
    if (l.size() != 2)
    {
      qDebug() << "Command::setData: invalid setting" << ts;
      return 1;
    }
 
    Command *c = script->scriptCommand(l.at(0));
    if (! c)
    {
      qDebug() << "Command::setData: invalid object name" << l.at(0);
      return 1;
    }
    
    Data setting;
    if (c->toData(l.at(1), setting))
    {
      qDebug() << "Command::setData: invalid setting name" << l.at(1);
      return 1;
    } 
    
    // value
    ts = sg.value(pos);
    QStringList l2 = ts.split(".", QString::SkipEmptyParts);
    if (l2.size() != 2)
    {
      if (parseValue(setting, ts))
        return 1;
      
      c->set(l.at(1), setting);
      continue;
    }
 
    Command *c2 = script->scriptCommand(l2.at(0));
    if (! c2)
    {
      if (parseValue(setting, ts))
        return 1;
      
      c->set(l.at(1), setting);
      continue;
    }
    
    Data setting2;
    if (c2->toData(l2.at(1), setting2))
    {
      qDebug() << "Command::setData: invalid setting name" << l2.at(1);
      return 1;
    } 
    
    if (setting.type() != setting2.type())
    {
      qDebug() << "Command::setData: setting and value types do not match" << setting.type() << setting2.type();
      return 1;
    }

    if (copySetting(setting, setting2))
      return 1;
      
    c->set(l.at(1), setting);
  }

  return 0;
}

int Command::copySetting (Data &setting, Data &setting2)
{
  switch ((TypeData::Key) setting.type())
  {
    case TypeData::_BOOL:
      setting.set(setting2.toBool());
      break;
    case TypeData::_COLOR:
      setting.set(setting2.toColor());
      break;
    case TypeData::_DATETIME:
      setting.set(setting2.toDateTime());
      break;
    case TypeData::_DOUBLE:
      setting.set(setting2.toDouble());
      break;
    case TypeData::_FONT:
      setting.set(setting2.toFont());
      break;
    case TypeData::_INTEGER:
      setting.set(setting2.toInteger());
      break;
    case TypeData::_STRING:
      setting.set(setting2.toString());
      break;
    case TypeData::_FILE:
      setting.set(setting2.toList());
      break;
    case TypeData::_LIST:
      setting.set(setting2.toList());
      setting.set(setting2.toString());
      break;
    default:
      qDebug() << "Command::copySetting: invalid setting";
      return 1;
      break;
  }
  
  return 0;
}

int Command::parseValue (Data &setting, QString &s)
{
  switch ((TypeData::Key) setting.type())
  {
    case TypeData::_FILE:
      setting.set(s.split(";"));
      break;
    default:
      if (setting.set(s, setting.type()))
      {
        qDebug() << "Command::parseValue: invalid value"  << s;
        return 1;
      }
      break;
  }
  
  return 0;
}
  