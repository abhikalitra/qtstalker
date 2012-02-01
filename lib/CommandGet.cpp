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
  if (sg.values() != 1)
  {
    qDebug() << "CommandGet::run: invalid number of values";
    return _returnCode;
  }
  
  Script *script = (Script *) scr;
  
  QString s = sg.value(0);
  QStringList l = s.split(".", QString::SkipEmptyParts);
  if (l.size() < 2)
  {
    qDebug() << "CommandGet::run: invalid format" << s;
    return _returnCode;
  }
  
  Command *c = script->scriptCommand(l.at(0));
  if (! c)
  {
    if (commandGetValue(l, scr))
      qDebug() << "CommandGet::run: invalid object/data name" << l.at(0);
    
    return _returnCode;
  }

  Data setting;
  if (c->toData(l.at(1), setting))
  {
    qDebug() << "CommandGet::run: invalid setting name" << l.at(1);
    return _returnCode;
  }

  switch ((TypeData::Key) setting.type())
  {
    case TypeData::_FILE:
    case TypeData::_LIST:
      _returnCode = setting.toList().join(";");
      break;
    default:
      _returnCode = setting.toString();
      break;
  }
  
  return _returnCode;
}

int CommandGet::commandGetValue (QStringList &l, void *scr)
{
  Script *script = (Script *) scr;
  
  Entity data;
  if (script->data(l.at(0), data))
  {
    qDebug() << "CommandGet::commandGetValue: invalid data name" << l.at(0);
    return 1;
  }
  
  switch ((TypeEntity::Key) data.type())
  {
    case TypeEntity::_CURVE:
    {
      if (l.size() != 3)
      {
        qDebug() << "CommandGet::commandGetValue: invalid setting format, must be OBJECT.INDEX.X or OBJECT.OFFSET.X" << l;
        return 1;
      }
      
      QStringList tkeys;
      tkeys << "INDEX" << "OFFSET";
      
      Entity bar;
      switch (tkeys.indexOf(l.at(1)))
      {
	case 0: // INDEX
        {
	  bool ok;
	  int index = l.at(2).toInt(&ok);
	  if (! ok)
	  {
            qDebug() << "CommandGet::commandGetValue: invalid INDEX value" << l.at(2);
            return 1;
	  }

          if (data.toIndex(index, bar))
          {
            qDebug() << "CommandGet::commandGetValue: invalid INDEX value" << l.at(2);
            return 1;
          }
	  break;
	}
	case 1: // OFFSET
        {
	  bool ok;
	  int offset = l.at(2).toInt(&ok);
	  if (! ok)
	  {
            qDebug() << "CommandGet::commandGetValue: invalid OFFSET value" << l.at(2);
            return 1;
	  }
	  
          if (data.toOffset(offset, bar))
          {
            qDebug() << "CommandGet::commandGetValue: invalid setting name" << l;
            return 1;
          }
	  break;
	}
	default: // INVALID
          qDebug() << "CommandGet::commandGetValue: invalid setting" << l;
          return 1;
	  break;
      }
      
      KeyCurveBar keys;
      Data td;
      if (bar.toData(keys.indexToString(KeyCurveBar::_VALUE), td))
      {
        qDebug() << "CommandGet::getData: invalid bar" << l;
        return 1;
      }
      
      _returnCode = td.toString();
      break;
    }
    default:
    {
      Data setting;
      if (data.toData(l.at(1), setting))
      {
        qDebug() << "CommandGet::getData: invalid setting name" << l.at(1);
        return 1;
      }
      
      _returnCode = setting.toString();
      break;
    }
  }
  
  return 0;
}
