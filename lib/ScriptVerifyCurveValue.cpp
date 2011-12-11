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
#include "DataFactory.h"
#include "CurveBar.h"
#include "DataDouble.h"
#include "DataColor.h"

#include <QtDebug>

ScriptVerifyCurveValue::ScriptVerifyCurveValue ()
{
}

int ScriptVerifyCurveValue::getValue (Data *in, QList<int> &keys, int index, int offset, double &v)
{
  v = 0;
  switch ((DataFactory::Type) in->type())
  {
    case DataFactory::_DOUBLE:
      v = in->toDouble();
      break;
    case DataFactory::_CURVE:
    {
      int pos = index;
      pos -= offset;
      if (pos < 0)
        return 1;

      Data *bar = in->toData(keys.at(pos));
      if (! bar)
        return 1;

      v = bar->toData(CurveBar::_VALUE)->toDouble();
      break;
    }
    default:
      return 1;
      break;
  }

  return 0;
}

int ScriptVerifyCurveValue::setValue (Data *, Data *out2, Data *bar, int pos)
{
  switch ((DataFactory::Type) out2->type())
  {
    case DataFactory::_DOUBLE:
      bar->set(CurveBar::_VALUE, new DataDouble(out2->toDouble()));
      break;
    case DataFactory::_COLOR:
      bar->set(CurveBar::_COLOR, new DataColor(out2->toColor()));
      break;
    case DataFactory::_CURVE:
    {
      Data *b = out2->toData(pos);
      if (! b)
        return 1;

      bar->set(CurveBar::_VALUE, new DataDouble(b->toData(CurveBar::_VALUE)->toDouble()));
      break;
    }
    default:
      return 1;
      break;
  }

  return 0;
}
