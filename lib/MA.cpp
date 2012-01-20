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

#include "MA.h"
#include "ta_libc.h"
#include "CurveData.h"
#include "KeyCurveBar.h"
#include "CurveBar.h"
#include "ScriptVerifyCurveKeys.h"
#include "TypeMA.h"

#include <QDebug>

MA::MA ()
{
  TA_RetCode rc = TA_Initialize();
  if (rc != TA_SUCCESS)
    qDebug("MA::MA: error on TA_Initialize");
}

int MA::getMA (Entity &in, int period, int method, Entity &line)
{
  if (method == TypeMA::_WILDER)
    return getWilder(in, period, line);

  // verify curve keys
  QList<QString> keys;
  ScriptVerifyCurveKeys svck;
  if (svck.keys1(in, keys))
  {
    qDebug() << "MA::getMA: invalid keys";
    return 1;
  }

  int size = keys.size();
  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  KeyCurveBar cbkeys;
  int loop = 0;
  for (; loop < size; loop++)
  {
    Entity bar;
    in.toEntity(keys.at(loop), bar);
    
    Data td;
    bar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), td);
    input[loop] = (TA_Real) td.toDouble();
  }

  TA_RetCode rc = TA_MA(0, size - 1, &input[0], period, (TA_MAType) method, &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "MA::calculate: TA-Lib error" << rc;
    return 1;
  }

  int keyLoop = keys.size() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    Entity b;
    b.set(cbkeys.indexToString(KeyCurveBar::_VALUE), Data(out[outLoop]));
    line.setEntity(keys.at(keyLoop), b);

    keyLoop--;
    outLoop--;
  }

  return 0;
}

int MA::getWilder (Entity &in, int period, Entity &line)
{
  // verify curve keys
  QList<QString> keys;
  ScriptVerifyCurveKeys svck;
  if (svck.keys1(in, keys))
  {
    qDebug() << "MA::getWilder: invalid keys";
    return 1;
  }

  KeyCurveBar cbkeys;
  double t = 0;
  int loop = 0;
  for (; loop < period; loop++)
  {
    Entity bar;
    in.toEntity(keys.at(loop), bar);
    
    Data td;
    bar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), td);
    t += td.toDouble();
  }
  double yesterday = t / (double) period;
  
  Entity db;
  db.set(cbkeys.indexToString(KeyCurveBar::_VALUE), Data(yesterday));
  line.setEntity(keys.at(loop), db);

  for (; loop < keys.size(); loop++)
  {
    Entity bar;
    in.toEntity(keys.at(loop), bar);
    
    Data td;
    bar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), td);
    double t  = (yesterday * (period - 1) + td.toDouble()) / (double) period;
    yesterday = t;

    Entity db;
    db.set(cbkeys.indexToString(KeyCurveBar::_VALUE), Data(t));
    line.setEntity(keys.at(loop), db);
  }

  return 0;
}
