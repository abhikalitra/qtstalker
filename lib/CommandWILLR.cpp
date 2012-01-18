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

#include "CommandWILLR.h"
#include "CurveData.h"
#include "ScriptVerifyCurve.h"
#include "TALibFunction.h"
#include "TALibFunctionKey.h"

#include <QtDebug>

CommandWILLR::CommandWILLR ()
{
  _name = "WILLR";

  Data td;
  td.setLabel(QObject::tr("Output"));
  Entity::set(QString("OUTPUT"), td);
  
  td = Data(QString("high"));
  td.setLabel(QObject::tr("Input High"));
  Entity::set(QString("HIGH"), td);
  
  td = Data(QString("low"));
  td.setLabel(QObject::tr("Input Low"));
  Entity::set(QString("LOW"), td);
  
  td = Data(QString("close"));
  td.setLabel(QObject::tr("Input Close"));
  Entity::set(QString("CLOSE"), td);
  
  td = Data(10);
  td.setLabel(QObject::tr("Period"));
  Entity::set(QString("PERIOD"), td);
}

QString CommandWILLR::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("HIGH"), td);

  ScriptVerifyCurve svc;
  Entity ihigh;
  if (svc.curve(script, td.toString(), ihigh))
  {
    qDebug() << "CommandWILLR::run: invalid HIGH" << td.toString();
    return _returnCode;
  }

  Entity::toData(QString("LOW"), td);
  Entity ilow;
  if (svc.curve(script, td.toString(), ilow))
  {
    qDebug() << "CommandWILLR::run: invalid LOW" << td.toString();
    return _returnCode;
  }

  Entity::toData(QString("CLOSE"), td);
  Entity iclose;
  if (svc.curve(script, td.toString(), iclose))
  {
    qDebug() << "CommandWILLR::run: invalid CLOSE" << td.toString();
    return _returnCode;
  }

  Data period;
  Entity::toData(QString("PERIOD"), period);
  
  Entity parms;
  parms.set(QString("FUNCTION"), Data(TALibFunctionKey::_WILLR));
  parms.set(QString("PERIOD"), period);
  
  CurveData line;
  TALibFunction func;
  if (func.run(parms, 3, ihigh, ilow, iclose, iclose, 1, line, line, line))
  {
    qDebug() << "CommandWILLR::run: TALibFunction error";
    return _returnCode;
  }

  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  _returnCode = "OK";
  return _returnCode;
}
