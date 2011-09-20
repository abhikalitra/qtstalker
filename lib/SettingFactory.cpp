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

#include "SettingFactory.h"
#include "SettingBool.h"
#include "SettingColor.h"
#include "SettingDateTime.h"
#include "SettingDouble.h"
#include "SettingFile.h"
#include "SettingFont.h"
#include "SettingInteger.h"
#include "SettingList.h"
#include "SettingString.h"

#include <QDebug>

SettingFactory::SettingFactory ()
{
  _types << "BOOL" << "COLOR" << "DATETIME" << "DOUBLE" << "FILE" << "FONT" << "INTEGER" << "LIST" << "STRING";
}

Setting * SettingFactory::setting (QString d)
{
  return setting(d.toInt());
}

Setting * SettingFactory::setting (int type)
{
  Setting *set = 0;

  switch ((Type) type)
  {
    case _BOOL:
      set = new SettingBool;
      break;
    case _COLOR:
      set = new SettingColor;
      break;
    case _DATETIME:
      set = new SettingDateTime;
      break;
    case _DOUBLE:
      set = new SettingDouble;
      break;
    case _FILE:
      set = new SettingFile;
      break;
    case _FONT:
      set = new SettingFont;
      break;
    case _INTEGER:
      set = new SettingInteger;
      break;
    case _LIST:
      set = new SettingList;
      break;
    case _STRING:
      set = new SettingString;
      break;
    default:
      break;
  }

  return set;
}

int SettingFactory::stringToType (QString d)
{
  return _types.indexOf(d);
}
