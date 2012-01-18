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

#include "CommandT3.h"
#include "CurveData.h"
#include "ScriptVerifyCurve.h"
#include "TALibFunction.h"
#include "TALibFunctionKey.h"

#include <QtDebug>

CommandT3::CommandT3 ()
{
  _name = "T3";

  Data td;
  td.setLabel(QObject::tr("Output"));
  Entity::set(QString("OUTPUT"), td);
  
  td = Data(QString("close"));
  td.setLabel(QObject::tr("Input"));
  Entity::set(QString("INPUT"), td);
  
  td = Data(14);
  td.setLabel(QObject::tr("Period"));
  Entity::set(QString("PERIOD"), td);
  
  td = Data(0.7);
  td.setLabel(QObject::tr("Volume Factor"));
  Entity::set(QString("V"), td);
}

QString CommandT3::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("INPUT"), td);

  ScriptVerifyCurve svc;
  Entity in;
  if (svc.curve(script, td.toString(), in))
  {
    qDebug() << "CommandT3::run: invalid INPUT" << td.toString();
    return _returnCode;
  }

  Data period, vfactor;
  Entity::toData(QString("PERIOD"), period);
  Entity::toData(QString("V"), vfactor);
  
  Entity parms;
  parms.set(QString("FUNCTION"), Data(TALibFunctionKey::_T3));
  parms.set(QString("PERIOD"), period);
  parms.set(QString("V"), vfactor);
  
  CurveData line;
  TALibFunction func;
  if (func.run(parms, 1, in, in, in, in, 1, line, line, line))
  {
    qDebug() << "CommandT3::run: TALibFunction error";
    return _returnCode;
  }

  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  _returnCode = "OK";
  return _returnCode;
}
