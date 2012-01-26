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
#include "TypeData.h"
#include "CurveBar.h"
#include "TypeEntity.h"
#include "KeyCurveBar.h"

#include <QtDebug>

ScriptVerifyCurveValue::ScriptVerifyCurveValue ()
{
}

int ScriptVerifyCurveValue::getValue (Entity &in, QList<int> &keys, int index, int offset, double &v)
{
  KeyCurveBar cbkeys;
  switch ((TypeEntity::Key) in.type())
  {
    case TypeEntity::_SETTING:
    {
      Data td;
      if (in.toData(QString::number(0), td))
	return 1;
      
      if (td.type() != TypeData::_DOUBLE)
	return 1;
      
      v = td.toDouble();
      break;
    }
    case TypeEntity::_CURVE:
    {
      int pos = index;
      pos -= offset;
      if (pos < 0)
        return 1;

      Entity bar;
      if (in.toIndex(keys.at(pos), bar))
        return 1;

      Data td;
      bar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), td);
      v = td.toDouble();
      break;
    }
    default:
      return 1;
      break;
  }

  return 0;
}

int ScriptVerifyCurveValue::setValue (Entity &out, Entity &bar, int pos)
{
  KeyCurveBar keys;  
  switch ((TypeEntity::Key) out.type())
  {
    case TypeEntity::_SETTING:
    {
      Data td;
      if (out.toData(QString("0"), td))
	return 1;
      
      switch ((TypeData::Key) td.type())
      {
	case TypeData::_DOUBLE:
          bar.set(keys.indexToString(KeyCurveBar::_VALUE), td);
          break;
	case TypeData::_COLOR:
          bar.set(keys.indexToString(KeyCurveBar::_COLOR), td);
          break;
	default:
	  return 1;
	  break;
      }
      break;
    }
    case TypeEntity::_CURVE:
    {
      Entity b;
      if (out.toIndex(pos, b))
        return 1;

      Data td;
      b.toData(keys.indexToString(KeyCurveBar::_VALUE), td);
      bar.set(keys.indexToString(KeyCurveBar::_VALUE), td);
      break;
    }
    default:
      return 1;
      break;
  }

  return 0;
}
