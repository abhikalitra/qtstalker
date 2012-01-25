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

#include "CommandGet.h"
#include "Script.h"
#include "TypeEntity.h"
#include "KeyCurveBar.h"

#include <QtDebug>

CommandGet::CommandGet ()
{
  _name = "GET";
}

QString CommandGet::run (CommandParse &sg, void *scr)
{
  if (sg.values() != 2)
  {
    qDebug() << "CommandGet::run: invalid number of values";
    return _returnCode;
  }
  
  Script *script = (Script *) scr;
  int pos = 0;
  QString name = sg.value(pos++);
  Command *c = script->scriptCommand(name);
  if (! c)
  {
    if (commandGetValue(sg, scr))
      qDebug() << "CommandGet::run: invalid object/data name" << name;
    
    return _returnCode;
  }

  QString settingName = sg.value(pos++);
  Data setting;
  if (c->toData(settingName, setting))
  {
    qDebug() << "CommandGet::run: invalid setting name" << settingName;
    return _returnCode;
  }

  switch ((TypeData::Key) setting.type())
  {
    case TypeData::_FILE:
      _returnCode = setting.toList().join(";");
      break;
    default:
      _returnCode = setting.toString();
      break;
  }
  
  return _returnCode;
}

int CommandGet::commandGetValue (CommandParse &sg, void *scr)
{
  Script *script = (Script *) scr;
  int pos = 0;
  QString name = sg.value(pos++);
  
  Entity data;
  if (script->data(name, data))
  {
    qDebug() << "CommandGet::getData: invalid data name" << name;
    return 1;
  }
  
  QString settingName = sg.value(pos++);
  
  switch ((TypeEntity::Key) data.type())
  {
    case TypeEntity::_CURVE:
    {
      QStringList tl = settingName.split(".");
      if (tl.size() != 2)
      {
        qDebug() << "CommandGet::getData: invalid setting format, must be INDEX.X or OFFSET.X" << settingName;
        return 1;
      }
      
      QStringList tkeys;
      tkeys << "INDEX" << "OFFSET";
      
      Entity bar;
      switch (tkeys.indexOf(tl.at(0)))
      {
	case 0: // INDEX
          if (data.toEntity(tl.at(1), bar))
          {
            qDebug() << "CommandGet::getData: invalid INDEX value" << settingName;
            return 1;
          }
	  break;
	case 1: // OFFSET
        {
	  bool ok;
	  int offset = tl.at(1).toInt(&ok);
	  if (! ok)
	  {
            qDebug() << "CommandGet::getData: invalid OFFSET value" << settingName;
            return 1;
	  }
	  
	  int start, end;
	  data.ekeyRange(start, end);
	  
	  int index = end - offset;
	  if (index < start)
            return 1;

	  QList<QString> ekeys = data.ekeys();
          if (data.toEntity(ekeys.at(index), bar))
          {
            qDebug() << "CommandGet::getData: invalid setting name" << settingName;
            return 1;
          }
	  break;
	}
	default: // INVALID
          qDebug() << "CommandGet::getData: invalid setting" << settingName;
          return 1;
	  break;
      }
      
      KeyCurveBar keys;
      Data td;
      if (bar.toData(keys.indexToString(KeyCurveBar::_VALUE), td))
      {
        qDebug() << "CommandGet::getData: invalid bar" << settingName;
        return 1;
      }
      
      _returnCode = td.toString();
      break;
    }
    default:
    {
      Data setting;
      if (data.toData(settingName, setting))
      {
        qDebug() << "CommandGet::getData: invalid setting name" << settingName;
        return 1;
      }
      
      _returnCode = setting.toString();
      break;
    }
  }
  
  return 0;
}
