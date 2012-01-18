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

#include "CommandMA.h"
#include "MAType.h"
#include "ScriptVerifyCurve.h"
#include "Script.h"
#include "CurveData.h"
#include "MA.h"

#include <QtDebug>

CommandMA::CommandMA ()
{
  _name = "MA";

  Entity::set(QString("OUTPUT"), Data(QString("ma")));
  Entity::set(QString("INPUT"), Data(QString("close")));
  Entity::set(QString("PERIOD"), Data(10));

  MAType mat;
  Data td(mat.list(), QString("EMA"));
  Entity::set(QString("TYPE"), td);
}

QString CommandMA::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;

  Data td;
  Entity::toData(QString("INPUT"), td);
  
  ScriptVerifyCurve svc;
  Entity in;
  if (svc.curve(script, td.toString(), in))
  {
    qDebug() << "CommandMA::run: invalid Input";
    return _returnCode;
  }

  Data period, type;
  Entity::toData(QString("PERIOD"), period);
  Entity::toData(QString("TYPE"), type);
  
  MA ma;
  CurveData line;
  if (ma.getMA(in, period.toInteger(), type.toInteger(), line))
  {
    qDebug() << "CommandMA::run: MAType error";
    return _returnCode;
  }

  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  _returnCode = "OK";
  return _returnCode;
}
