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

#include "ScriptVerifyCurveKeys.h"
#include "DataFactory.h"

#include <QtDebug>

ScriptVerifyCurveKeys::ScriptVerifyCurveKeys ()
{
}

int ScriptVerifyCurveKeys::keys (QList<Data *> &list, QList<int> &keys)
{
  keys.clear();

  Data *keyData = 0;
  int minKey = 99999999;
  int loop = 0;
  for (; loop < list.count(); loop++)
  {
    Data *curve = list.at(loop);
    if (curve->type() != DataFactory::_CURVE)
      continue;

    if (curve->keyCount() < minKey)
    {
      minKey = curve->keyCount();
      keyData = curve;
    }
  }

  if (keyData)
  {
    keys = keyData->keys();
    return 0;
  }

  return 1;
}
