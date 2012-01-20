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

#include "CommandULTOSC.h"
#include "CurveData.h"
#include "ScriptVerifyCurve.h"
#include "TALibFunction.h"
#include "TypeTALibFunction.h"

#include <QtDebug>

CommandULTOSC::CommandULTOSC ()
{
  _name = "ULTOSC";

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
  
  td = Data(7);
  td.setLabel(QObject::tr("Period Short"));
  Entity::set(QString("PERIOD_S"), td);
  
  td = Data(14);
  td.setLabel(QObject::tr("Period Medium"));
  Entity::set(QString("PERIOD_M"), td);

  td = Data(28);
  td.setLabel(QObject::tr("Period Long"));
  Entity::set(QString("PERIOD_L"), td);
}

QString CommandULTOSC::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("HIGH"), td);

  ScriptVerifyCurve svc;
  Entity ihigh;
  if (svc.curve(script, td.toString(), ihigh))
  {
    qDebug() << "CommandULTOSC::run: invalid HIGH" << td.toString();
    return _returnCode;
  }

  Entity::toData(QString("LOW"), td);
  Entity ilow;
  if (svc.curve(script, td.toString(), ilow))
  {
    qDebug() << "CommandULTOSC::run: invalid LOW" << td.toString();
    return _returnCode;
  }

  Entity::toData(QString("CLOSE"), td);
  Entity iclose;
  if (svc.curve(script, td.toString(), iclose))
  {
    qDebug() << "CommandULTOSC::run: invalid CLOSE" << td.toString();
    return _returnCode;
  }

  Data sp, mp, lp;
  Entity::toData(QString("PERIOD_S"), sp);
  Entity::toData(QString("PERIOD_M"), mp);
  Entity::toData(QString("PERIOD_L"), lp);
  
  Entity parms;
  parms.set(QString("FUNCTION"), Data(TypeTALibFunction::_ULTOSC));
  parms.set(QString("PERIOD_S"), sp);
  parms.set(QString("PERIOD_M"), mp);
  parms.set(QString("PERIOD_L"), lp);
  
  CurveData line;
  TALibFunction func;
  if (func.run(parms, 3, ihigh, ilow, iclose, iclose, 1, line, line, line))
  {
    qDebug() << "CommandULTOSC::run: TALibFunction error";
    return _returnCode;
  }

  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  _returnCode = "OK";
  return _returnCode;
}
