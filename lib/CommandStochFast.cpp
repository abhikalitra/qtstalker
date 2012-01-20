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

#include "CommandStochFast.h"
#include "CurveData.h"
#include "ScriptVerifyCurve.h"
#include "TypeMA.h"
#include "TALibFunction.h"
#include "TypeTALibFunction.h"

#include <QtDebug>

CommandStochFast::CommandStochFast ()
{
  _name = "STOCH_FAST";

  Data td;
  td.setLabel(QObject::tr("Output %K"));
  Entity::set(QString("K"), td);
  
  td.setLabel(QObject::tr("Output %D"));
  Entity::set(QString("D"), td);
  
  td = Data(QString("high"));
  td.setLabel(QObject::tr("Input High"));
  Entity::set(QString("HIGH"), td);
  
  td = Data(QString("low"));
  td.setLabel(QObject::tr("Input Low"));
  Entity::set(QString("LOW"), td);
  
  td = Data(QString("close"));
  td.setLabel(QObject::tr("Input Close"));
  Entity::set(QString("CLOSE"), td);
  
  td = Data(5);
  td.setLabel(QObject::tr("Period K"));
  Entity::set(QString("PERIOD_K"), td);
  
  td = Data(3);
  td.setLabel(QObject::tr("Period D"));
  Entity::set(QString("PERIOD_D"), td);
  
  TypeMA mat;
  td = Data(mat.list(), QString("EMA"));
  td.setLabel(QObject::tr("MA Type"));
  Entity::set(QString("MA"), td);
}

QString CommandStochFast::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("HIGH"), td);

  ScriptVerifyCurve svc;
  Entity ihigh;
  if (svc.curve(script, td.toString(), ihigh))
  {
    qDebug() << "CommandStochFast::run: invalid HIGH" << td.toString();
    return _returnCode;
  }

  Entity::toData(QString("LOW"), td);
  Entity ilow;
  if (svc.curve(script, td.toString(), ilow))
  {
    qDebug() << "CommandStochFast::run: invalid LOW" << td.toString();
    return _returnCode;
  }

  Entity::toData(QString("CLOSE"), td);
  Entity iclose;
  if (svc.curve(script, td.toString(), iclose))
  {
    qDebug() << "CommandStochFast::run: invalid CLOSE" << td.toString();
    return _returnCode;
  }

  Data pk, pd, ma;
  Entity::toData(QString("PERIOD_K"), pk);
  Entity::toData(QString("PERIOD_D"), pd);
  Entity::toData(QString("MA"), ma);
  
  Entity parms;
  parms.set(QString("FUNCTION"), Data(TypeTALibFunction::_STOCHF));
  parms.set(QString("PERIOD_K"), pk);
  parms.set(QString("PERIOD_D"), pd);
  parms.set(QString("MA"), ma);
  
  CurveData kl, dl;
  TALibFunction func;
  if (func.run(parms, 3, ihigh, ilow, iclose, iclose, 2, kl, dl, dl))
  {
    qDebug() << "CommandStochFast::run: TALibFunction error";
    return _returnCode;
  }

  Entity::toData(QString("K"), td);
  script->setData(td.toString(), kl);

  Entity::toData(QString("D"), td);
  script->setData(td.toString(), dl);

  _returnCode = "OK";
  return _returnCode;
}
