/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "TALibOutput.h"
#include "CurveBar.h"
#include "CurveBarKey.h"

TALibOutput::TALibOutput ()
{
}

int TALibOutput::fillOutputDouble1 (Entity &line, QList<QString> &keys, int outNb, TA_Real out[])
{
  CurveBarKey cbkeys;
  int keyLoop = keys.size() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    CurveBar b;
    b.set(cbkeys.indexToString(CurveBarKey::_VALUE), Data(out[outLoop]));
    line.setEntity(keys.at(keyLoop), b);

    keyLoop--;
    outLoop--;
  }

  return 0;
}

int TALibOutput::fillOutputDouble2 (Entity &line, Entity &line2, QList<QString> &keys, int outNb, TA_Real out[], TA_Real out2[])
{
  CurveBarKey cbkeys;
  int keyLoop = keys.size() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    CurveBar b;
    b.set(cbkeys.indexToString(CurveBarKey::_VALUE), Data(out[outLoop]));
    line.setEntity(keys.at(keyLoop), b);

    b.set(cbkeys.indexToString(CurveBarKey::_VALUE), Data(out2[outLoop]));
    line2.setEntity(keys.at(keyLoop), b);

    keyLoop--;
    outLoop--;
  }

  return 0;
}

int TALibOutput::fillOutputDouble3 (Entity &line, Entity &line2, Entity &line3, QList<QString> &keys,
			      int outNb, TA_Real out[], TA_Real out2[], TA_Real out3[])
{
  CurveBarKey cbkeys;
  int keyLoop = keys.size() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    CurveBar b;
    b.set(cbkeys.indexToString(CurveBarKey::_VALUE), Data(out[outLoop]));
    line.setEntity(keys.at(keyLoop), b);

    b.set(cbkeys.indexToString(CurveBarKey::_VALUE), Data(out2[outLoop]));
    line2.setEntity(keys.at(keyLoop), b);

    b.set(cbkeys.indexToString(CurveBarKey::_VALUE), Data(out3[outLoop]));
    line3.setEntity(keys.at(keyLoop), b);

    keyLoop--;
    outLoop--;
  }

  return 0;
}

int TALibOutput::fillOutputInteger1 (Entity &line, QList<QString> &keys, int outNb, TA_Integer out[])
{
  CurveBarKey cbkeys;
  int keyLoop = keys.size() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    CurveBar b;
    b.set(cbkeys.indexToString(CurveBarKey::_VALUE), Data(out[outLoop]));
    line.setEntity(keys.at(keyLoop), b);

    keyLoop--;
    outLoop--;
  }

  return 0;
}
