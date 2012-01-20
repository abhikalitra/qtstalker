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

#include "CommandMAMA.h"
#include "CurveData.h"
#include "ScriptVerifyCurve.h"
#include "TALibFunction.h"
#include "TypeTALibFunction.h"

#include <QtDebug>

CommandMAMA::CommandMAMA ()
{
  _name = "MAMA";

  Data td;
  td.setLabel(QObject::tr("Output MAMA"));
  Entity::set(QString("MAMA"), td);

  td.setLabel(QObject::tr("Output FAMA"));
  Entity::set(QString("FAMA"), td);
  
  td = Data(QString("close"));
  td.setLabel(QObject::tr("Input"));
  Entity::set(QString("INPUT"), td);
  
  td = Data(0.5);
  td.setLabel(QObject::tr("Limit Fast"));
  Entity::set(QString("LIMIT_FAST"), td);
  
  td = Data(0.05);
  td.setLabel(QObject::tr("Limit Slow"));
  Entity::set(QString("LIMIT_SLOW"), td);
}

QString CommandMAMA::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("INPUT"), td);

  ScriptVerifyCurve svc;
  Entity in;
  if (svc.curve(script, td.toString(), in))
  {
    qDebug() << "CommandMAMA::run: invalid INPUT" << td.toString();
    return _returnCode;
  }

  Data lf, ls;
  Entity::toData(QString("LIMIT_FAST"), lf);
  Entity::toData(QString("LIMIT_SLOW"), ls);

  Entity parms;
  parms.set(QString("FUNCTION"), Data(TypeTALibFunction::_MAMA));
  parms.set(QString("LIMIT_FAST"), lf);
  parms.set(QString("LIMIT_SLOW"), ls);
  
  CurveData mama, fama;
  TALibFunction func;
  if (func.run(parms, 1, in, in, in, in, 2, mama, fama, fama))
  {
    qDebug() << "CommandMAMA::run: TALibFunction error";
    return _returnCode;
  }

  Entity::toData(QString("MAMA"), td);
  script->setData(td.toString(), mama);

  Entity::toData(QString("FAMA"), td);
  script->setData(td.toString(), fama);

  _returnCode = "OK";
  return _returnCode;
}
