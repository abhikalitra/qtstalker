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

#include "CommandAROON.h"
#include "CurveData.h"
#include "ScriptVerifyCurve.h"
#include "TALibFunction.h"
#include "TypeTALibFunction.h"
#include "Script.h"

#include <QtDebug>

CommandAROON::CommandAROON ()
{
  _name = "AROON";

  Data td(QString("aroonu"));
  td.setLabel(QObject::tr("Output Upper"));
  Entity::set(QString("UPPER"), td);

  td = Data(QString("aroonl"));
  td.setLabel(QObject::tr("Output Lower"));
  Entity::set(QString("LOWER"), td);
  
  td = Data(QString("high"));
  td.setLabel(QObject::tr("Input High"));
  Entity::set(QString("HIGH"), td);
  
  td = Data(QString("low"));
  td.setLabel(QObject::tr("Input Low"));
  Entity::set(QString("LOW"), td);
  
  td = Data(10);
  td.setLabel(QObject::tr("Period"));
  Entity::set(QString("PERIOD"), td);
}

QString CommandAROON::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("HIGH"), td);

  ScriptVerifyCurve svc;
  Entity ihigh;
  if (svc.curve(script, td.toString(), ihigh))
  {
    qDebug() << "CommandAROON::run: invalid HIGH" << td.toString();
    return _returnCode;
  }

  Entity::toData(QString("LOW"), td);
  Entity ilow;
  if (svc.curve(script, td.toString(), ilow))
  {
    qDebug() << "CommandAROON::run: invalid LOW" << td.toString();
    return _returnCode;
  }

  Entity parms;
  Data period;
  Entity::toData(QString("PERIOD"), period);
  parms.set(QString("PERIOD"), period);
  parms.set(QString("FUNCTION"), Data(TypeTALibFunction::_AROON));
  
  CurveData upper;
  CurveData lower;
  TALibFunction func;
  if (func.run(parms, 2, ihigh, ilow, ilow, ilow, 2, upper, lower, lower))
  {
    qDebug() << "CommandAROON::run: getAROON error";
    return _returnCode;
  }

  Entity::toData(QString("UPPER"), td);
  script->setData(td.toString(), upper);

  Entity::toData(QString("LOWER"), td);
  script->setData(td.toString(), lower);

  _returnCode = "OK";
  return _returnCode;
}
