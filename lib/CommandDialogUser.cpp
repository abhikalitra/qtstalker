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

#include "CommandDialogUser.h"
#include "TypeData.h"
#include "TypeBarLength.h"
#include "TypeDateRange.h"

#include <QtDebug>

CommandDialogUser::CommandDialogUser ()
{
  _name = "DIALOG_USER";
}

int CommandDialogUser::setData (CommandParse &sg)
{
  // format object, setting, type, value...

  if (sg.values() < 4)
  {
    qDebug() << "CommandDialogUser::setData: invalid number of values" << sg.values();
    return 1;
  }
  
  int ti = sg.values() - 1;
  int rem = ti % 3;
  if (rem != 0)
  {
    qDebug() << "CommandDialogUser::setData: invalid number of values" << sg.values();
    return 1;
  }
  
  QStringList _extraTypes;
  _extraTypes << "LENGTH" << "RANGE";
  
  TypeData keys;
  int loop = 1; // skip past object name
  for (; loop < sg.values(); loop += 3)
  {
    int pos = loop;
    QString type = sg.value(pos++);
    QString settingName = sg.value(pos++);
    QString value = sg.value(pos);

    // intercept a local type
    switch(_extraTypes.indexOf(type))
    {
      case 0: // LENGTH
      {
        TypeBarLength tbl;
        Data setting(tbl.list(), value);
	setting.setLabel(settingName);
        set(settingName, setting);
	continue;
        break;
      }
      case 1: // RANGE
      {
	TypeDateRange tdr;
        Data setting(tdr.list(), value);
	setting.setLabel(settingName);
        set(settingName, setting);
        break;
      }
      default:
      {
        // standard type
        Data setting((TypeData::Key) keys.stringToIndex(type));
	setting.setLabel(settingName);
  
        if (setting.set(value, setting.type()))
        {
          qDebug() << "CommandDialogUser::setData: invalid value" << setting.type() << settingName << type << value;
          return 1;
        }
    
        set(settingName, setting);
	break;
      }
    }
  }

  return 0;
}
