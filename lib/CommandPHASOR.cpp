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

#include "CommandPHASOR.h"
#include "CurveData.h"
#include "ScriptVerifyCurve.h"
#include "TALibFunction.h"
#include "TALibFunctionKey.h"

#include <QtDebug>

CommandPHASOR::CommandPHASOR ()
{
  _name = "HT_PHASOR";

  Data td;
  td.setLabel(QObject::tr("Output Phase"));
  Entity::set(QString("PHASE"), td);
  
  td.setLabel(QObject::tr("Output Quad"));
  Entity::set(QString("QUAD"), td);

  td = Data(QString("close"));
  td.setLabel(QObject::tr("Input"));
  Entity::set(QString("INPUT"), td);
}

QString CommandPHASOR::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("INPUT"), td);

  ScriptVerifyCurve svc;
  Entity in;
  if (svc.curve(script, td.toString(), in))
  {
    qDebug() << "CommandPHASOR::run: invalid INPUT" << td.toString();
    return _returnCode;
  }

  Entity parms;
  parms.set(QString("FUNCTION"), Data(TALibFunctionKey::_PHASOR));
  
  CurveData phase, quad;
  TALibFunction func;
  if (func.run(parms, 1, in, in, in, in, 2, phase, quad, quad))
  {
    qDebug() << "CommandPHASOR::run: TALibFunction error";
    return _returnCode;
  }

  Entity::toData(QString("PHASE"), td);
  script->setData(td.toString(), phase);

  Entity::toData(QString("QUAD"), td);
  script->setData(td.toString(), quad);

  _returnCode = "OK";
  return _returnCode;
}
