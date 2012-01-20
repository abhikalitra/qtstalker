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

#include "CommandNormalize.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "KeyCurveBar.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>
#include <cmath>

CommandNormalize::CommandNormalize ()
{
  _name = "NORMALIZE";

  Data td;
  td.setLabel(QObject::tr("Output"));
  Entity::set(QString("OUTPUT"), td);
  
  td = Data(QString("close"));
  td.setLabel(QObject::tr("Input"));
  Entity::set(QString("INPUT"), td);
}

QString CommandNormalize::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("INPUT"), td);

  ScriptVerifyCurve svc;
  Entity in;
  if (svc.curve(script, td.toString(), in))
  {
    qDebug() << "CommandNormalize::run: invalid INPUT" << td.toString();
    return _returnCode;
  }

  CurveData line;
  if (getNORM(in, line))
  {
    qDebug() << "CommandNormalize::runScript: getNORM error";
    return _returnCode;
  }

  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  _returnCode = "OK";
  return _returnCode;
}

int CommandNormalize::getNORM (Entity &in, Entity &line)
{
  QList<QString> keys;
  ScriptVerifyCurveKeys svck;
  if (svck.keys1(in, keys))
    return 1;

  double max = 0;
  double min = 0;
  if (in.highLow(max, min))
    return 0;

  double range = fabs(max) + fabs(min);

  KeyCurveBar cbkeys;
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Entity bar;
    in.toEntity(keys.at(loop), bar);
    Data v;
    if (bar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), v))
      continue;
    
    double t = ((v.toDouble() - min) / range) * 100;

    CurveBar b;
    b.set(cbkeys.indexToString(KeyCurveBar::_VALUE), Data(t));
    line.setEntity(keys.at(loop), b);
  }

  return 0;
}
