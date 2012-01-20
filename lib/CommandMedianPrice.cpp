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

#include "CommandMedianPrice.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "KeyCurveBar.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"
#include "ScriptVerifyCurveValue.h"

#include <QtDebug>

CommandMedianPrice::CommandMedianPrice ()
{
  _name = "MEDIAN_PRICE";

  Data td;
  td.setLabel(QObject::tr("Output"));
  Entity::set(QString("OUTPUT"), td);

  td.setLabel(QObject::tr("Input"));
  Entity::set(QString("INPUT"), td);
  
  td.setLabel(QObject::tr("Input 2"));
  Entity::set(QString("INPUT2"), td);
}

QString CommandMedianPrice::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("INPUT"), td);

  ScriptVerifyCurve svc;
  Entity in;
  if (svc.curve(script, td.toString(), in))
  {
    if (svc.entity(td.toString(), in))
    {
      qDebug() << "CommandMedianPrice::run: invalid INPUT" << td.toString();
      return _returnCode;
    }
  }
  
  Entity::toData(QString("INPUT2"), td);
  Entity in2;
  if (svc.curve(script, td.toString(), in2))
  {
    if (svc.entity(td.toString(), in2))
    {
      qDebug() << "CommandMedianPrice::run: invalid INPUT2" << td.toString();
      return _returnCode;
    }
  }
  
  CurveData line;
  if (getMP(in, in2, line))
  {
    qDebug() << "CommandMedianPrice::run: getMP error";
    return _returnCode;
  }

  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  _returnCode = "OK";
  return _returnCode;
}

int CommandMedianPrice::getMP (Entity &in, Entity &in2, Entity &line)
{
  QList<QString> keys;
  ScriptVerifyCurveKeys svck;
  if (svck.keys2(in, in2, keys))
    return 1;

  KeyCurveBar cbkeys;
  ScriptVerifyCurveValue svcv;
  int offset = 0;
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    double v = 0;
    if (svcv.getValue(in, keys, loop, offset, v))
      continue;

    double v2 = 0;
    if (svcv.getValue(in2, keys, loop, offset, v2))
      continue;

    double t = (v + v2) / 2.0;
    CurveBar b;
    b.set(cbkeys.indexToString(KeyCurveBar::_VALUE), Data(t));
    line.setEntity(keys.at(loop), b);
  }

  return 0;
}
