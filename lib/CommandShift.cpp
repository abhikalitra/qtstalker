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

#include "CommandShift.h"
#include "CurveData.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"
#include "ScriptVerifyCurveValue.h"
#include "Script.h"

#include <QtDebug>

CommandShift::CommandShift ()
{
  _name = "SHIFT";

  Data td;
  td.setLabel(QObject::tr("Output"));
  Entity::set(QString("OUTPUT"), td);

  td.setLabel(QObject::tr("Input"));
  Entity::set(QString("INPUT"), td);
  
  td = Data(10);
  td.setLabel(QObject::tr("Period"));
  Entity::set(QString("PERIOD"), td);
}

QString CommandShift::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("INPUT"), td);

  ScriptVerifyCurve svc;
  Entity in;
  if (svc.curve(script, td.toString(), in))
  {
    qDebug() << "CommandShift::run: invalid INPUT" << td.toString();
    return _returnCode;
  }
  
  Data period;
  Entity::toData(QString("PERIOD"), period);

  CurveData line;
  if (getShift(in, period.toInteger(), line))
  {
    qDebug() << "CommandShift::run: getShift error";
    return _returnCode;
  }

  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  _returnCode = "OK";
  return _returnCode;
}

int CommandShift::getShift (Entity &in, int period, Entity &line)
{
  QList<QString> keys;
  ScriptVerifyCurveKeys svck;
  if (svck.keys1(in, keys))
    return 1;

  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Entity bar;
    in.toEntity(keys.at(loop), bar);

    int index = loop + period;
    if (index < 0)
      continue;
    
    QString key;
    if (index >= keys.size())
      key = QString::number(index);
    else
      key = keys.at(index);

    line.setEntity(key, bar);
  }

  return 0;
}
