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

#include "ScriptVerifyCurveValue.h"
#include "DataType.h"
#include "CurveBar.h"
#include "EntityType.h"
#include "CurveBarKey.h"

#include <QtDebug>

ScriptVerifyCurveValue::ScriptVerifyCurveValue ()
{
}

int ScriptVerifyCurveValue::getValue (Entity &in, QList<QString> &keys, int index, int offset, double &v)
{
  CurveBarKey cbkeys;
  switch ((EntityType::Type) in.type())
  {
    case EntityType::_SETTING:
    {
      Data td;
      if (in.toData(QString::number(0), td))
	return 1;
      
      if (td.type() != DataType::_DOUBLE)
	return 1;
      
      v = td.toDouble();
      break;
    }
    case EntityType::_CURVE:
    {
      int pos = index;
      pos -= offset;
      if (pos < 0)
        return 1;

      Entity bar;
      if (in.toEntity(keys.at(pos), bar))
        return 1;

      Data td;
      bar.toData(cbkeys.indexToString(CurveBarKey::_VALUE), td);
      v = td.toDouble();
      break;
    }
    default:
      return 1;
      break;
  }

  return 0;
}

int ScriptVerifyCurveValue::setValue (Entity &out, Entity &bar, QString pos)
{
  CurveBarKey keys;  
  switch ((EntityType::Type) out.type())
  {
    case EntityType::_SETTING:
    {
      Data td;
      if (out.toData(QString("0"), td))
	return 1;
      
      switch ((DataType::Type) td.type())
      {
	case DataType::_DOUBLE:
          bar.set(keys.indexToString(CurveBarKey::_VALUE), td);
          break;
	case DataType::_COLOR:
          bar.set(keys.indexToString(CurveBarKey::_COLOR), td);
          break;
	default:
	  return 1;
	  break;
      }
      break;
    }
    case EntityType::_CURVE:
    {
      Entity b;
      if (out.toEntity(pos, b))
        return 1;

      Data td;
      b.toData(keys.indexToString(CurveBarKey::_VALUE), td);
      bar.set(keys.indexToString(CurveBarKey::_VALUE), td);
      break;
    }
    default:
      return 1;
      break;
  }

  return 0;
}
