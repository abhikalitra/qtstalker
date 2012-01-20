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
#include "TypeEntity.h"

#include <QtDebug>

ScriptVerifyCurveKeys::ScriptVerifyCurveKeys ()
{
}

int ScriptVerifyCurveKeys::keys1 (Entity &in, QList<QString> &keys)
{
  int minKey = 99999999;
  int pos = -1;
  verifyKeys(in, minKey, pos);

  if (pos == -1)
    return 1;
  
  QList<QString> skeys = in.ekeys();

  sortKeys(skeys, keys);
    
  return 0;
}

int ScriptVerifyCurveKeys::keys2 (Entity &in, Entity &in2, QList<QString> &keys)
{
  int minKey = 99999999;
  int pos = -1;
  verifyKeys(in, minKey, pos);
  verifyKeys(in2, minKey, pos);

  if (pos == -1)
    return 1;
  
  QList<QString> skeys;
  switch (pos)
  {
    case 0:
      skeys = in.ekeys();
      break;
    case 1:
      skeys = in2.ekeys();
      break;
    default:
      return 1;
      break;
  }

  sortKeys(skeys, keys);
  
  return 0;
}

int ScriptVerifyCurveKeys::keys3 (Entity &in, Entity &in2, Entity &in3, QList<QString> &keys)
{
  int minKey = 99999999;
  int pos = -1;
  verifyKeys(in, minKey, pos);
  verifyKeys(in2, minKey, pos);
  verifyKeys(in3, minKey, pos);

  if (pos == -1)
    return 1;
  
  QList<QString> skeys;
  switch (pos)
  {
    case 0:
      skeys = in.ekeys();
      break;
    case 1:
      skeys = in2.ekeys();
      break;
    case 2:
      skeys = in3.ekeys();
      break;
    default:
      return 1;
      break;
  }

  sortKeys(skeys, keys);
  
  return 0;
}

int ScriptVerifyCurveKeys::keys4 (Entity &in, Entity &in2, Entity &in3, Entity &in4, QList<QString> &keys)
{
  int minKey = 99999999;
  int pos = -1;
  verifyKeys(in, minKey, pos);
  verifyKeys(in2, minKey, pos);
  verifyKeys(in3, minKey, pos);
  verifyKeys(in4, minKey, pos);

  if (pos == -1)
    return 1;
  
  QList<QString> skeys;
  switch (pos)
  {
    case 0:
      skeys = in.ekeys();
      break;
    case 1:
      skeys = in2.ekeys();
      break;
    case 2:
      skeys = in3.ekeys();
      break;
    case 3:
      skeys = in4.ekeys();
      break;
    default:
      return 1;
      break;
  }

  sortKeys(skeys, keys);
  
  return 0;
}

int ScriptVerifyCurveKeys::verifyKeys (Entity &e, int &minKey, int &pos)
{
  if (e.type() != TypeEntity::_CURVE)
    return 1;

  if (e.ekeyCount() < minKey)
  {
    minKey = e.ekeyCount();
    pos++;
  }
  
  return 0;
}

void ScriptVerifyCurveKeys::sortKeys (QList<QString> &inKeys, QList<QString> &outKeys)
{
  outKeys.clear();
  
  QList<int> ikeys;
  int loop = 0;
  for (; loop < inKeys.size(); loop++)
    ikeys << inKeys.at(loop).toInt();
  qSort(ikeys);
    
  for (loop = 0; loop < ikeys.size(); loop++)
    outKeys << QString::number(ikeys.at(loop));
}
