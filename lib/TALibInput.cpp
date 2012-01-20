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

#include "TALibInput.h"
#include "CurveBar.h"
#include "KeyCurveBar.h"

TALibInput::TALibInput ()
{
}

int TALibInput::fillInput (Entity &in, QList<QString> &keys, TA_Real out[])
{
  KeyCurveBar cbkeys;
  int ipos = 0;
  int opos = 0;
  for (; ipos < keys.size(); ipos++)
  {
    Entity bar;
    if (in.toEntity(keys.at(ipos), bar))
      continue;

    Data td;
    bar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), td);
    out[opos] = (TA_Real) td.toDouble();
    opos++;
  }

  return opos;
}

int TALibInput::fillInput2 (Entity &in, Entity &in2, QList<QString> &keys, TA_Real out[], TA_Real out2[])
{
  KeyCurveBar cbkeys;
  int ipos = 0;
  int opos = 0;
  for (; ipos < keys.size(); ipos++)
  {
    Entity bar;
    if (in.toEntity(keys.at(ipos), bar))
      continue;

    Entity bar2;
    if (in2.toEntity(keys.at(ipos), bar2))
      continue;

    Data td;
    bar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), td);
    out[opos] = (TA_Real) td.toDouble();
    
    bar2.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), td);
    out2[opos] = (TA_Real) td.toDouble();
    opos++;
  }

  return opos;
}

int TALibInput::fillInput3 (Entity &in, Entity &in2, Entity &in3, QList<QString> &keys,
			    TA_Real out[], TA_Real out2[], TA_Real out3[])
{
  KeyCurveBar cbkeys;
  int ipos = 0;
  int opos = 0;
  for (; ipos < keys.size(); ipos++)
  {
    Entity bar;
    if (in.toEntity(keys.at(ipos), bar))
      continue;

    Entity bar2;
    if (in2.toEntity(keys.at(ipos), bar2))
      continue;

    Entity bar3;
    if (in3.toEntity(keys.at(ipos), bar3))
      continue;

    Data td;
    bar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), td);
    out[opos] = (TA_Real) td.toDouble();
    
    bar2.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), td);
    out2[opos] = (TA_Real) td.toDouble();
    
    bar3.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), td);
    out3[opos] = (TA_Real) td.toDouble();
    
    opos++;
  }

  return opos;
}

int TALibInput::fillInput4 (Entity &in, Entity &in2, Entity &in3, Entity &in4, QList<QString> &keys,
			    TA_Real out[], TA_Real out2[], TA_Real out3[], TA_Real out4[])
{
  KeyCurveBar cbkeys;
  int ipos = 0;
  int opos = 0;
  for (; ipos < keys.size(); ipos++)
  {
    Entity bar;
    if (in.toEntity(keys.at(ipos), bar))
      continue;

    Entity bar2;
    if (in2.toEntity(keys.at(ipos), bar2))
      continue;

    Entity bar3;
    if (in3.toEntity(keys.at(ipos), bar3))
      continue;

    Entity bar4;
    if (in4.toEntity(keys.at(ipos), bar4))
      continue;

    Data td;
    bar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), td);
    out[opos] = (TA_Real) td.toDouble();
    
    bar2.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), td);
    out2[opos] = (TA_Real) td.toDouble();
    
    bar3.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), td);
    out3[opos] = (TA_Real) td.toDouble();
    
    bar4.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), td);
    out4[opos] = (TA_Real) td.toDouble();
    
    opos++;
  }

  return opos;
}
