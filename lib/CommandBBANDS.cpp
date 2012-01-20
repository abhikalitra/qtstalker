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

#include "CommandBBANDS.h"
#include "CurveData.h"
#include "ScriptVerifyCurve.h"
#include "TALibFunction.h"
#include "TypeTALibFunction.h"
#include "TypeMA.h"

#include <QtDebug>

CommandBBANDS::CommandBBANDS ()
{
  _name = "BBANDS";

  Data td;
  td.setLabel(QObject::tr("Output Upper"));
  Entity::set(QString("UPPER"), td);

  td.setLabel(QObject::tr("Output Middle"));
  Entity::set(QString("MIDDLE"), td);

  td.setLabel(QObject::tr("Output Lower"));
  Entity::set(QString("LOWER"), td);

  td = Data(QString("close"));
  td.setLabel(QObject::tr("Input"));
  Entity::set(QString("INPUT"), td);
  
  td = Data(20);
  td.setLabel(QObject::tr("Period"));
  Entity::set(QString("PERIOD"), td);
  
  TypeMA mat;
  td = Data(mat.list(), QString("EMA"));
  td.setLabel(QObject::tr("MA Type"));
  Entity::set(QString("MA_TYPE"), td);
  
  td = Data(2.0);
  td.setLabel(QObject::tr("Deviation Upper"));
  Entity::set(QString("DEV_UP"), td);
  
  td = Data(2.0);
  td.setLabel(QObject::tr("Deviation Lower"));
  Entity::set(QString("DEV_DOWN"), td);
}

QString CommandBBANDS::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("INPUT"), td);

  ScriptVerifyCurve svc;
  Entity in;
  if (svc.curve(script, td.toString(), in))
  {
    qDebug() << "CommandBBANDS::run: invalid Input";
    return _returnCode;
  }

  Data period, maType, devUp, devDown;
  Entity::toData(QString("PERIOD"), period);
  Entity::toData(QString("MA_TYPE"), maType);
  Entity::toData(QString("DEV_UP"), devUp);
  Entity::toData(QString("DEV_DOWN"), devDown);
  
  Entity parms;
  parms.set(QString("FUNCTION"), Data(TypeTALibFunction::_BBANDS));
  parms.set(QString("PERIOD"), period);
  parms.set(QString("MA_TYPE"), maType);
  parms.set(QString("DEV_UP"), devUp);
  parms.set(QString("DEV_DOWN"), devDown);
  
  CurveData upper, middle, lower;
  TALibFunction func;
  if (func.run(parms, 1, in, in, in, in, 3, upper, middle, lower))
  {
    qDebug() << "CommandBBANDS::run: TALibFunction error";
    return _returnCode;
  }

  Entity::toData(QString("UPPER"), td);
  script->setData(td.toString(), upper);

  Entity::toData(QString("MIDDLE"), td);
  script->setData(td.toString(), middle);

  Entity::toData(QString("LOWER"), td);
  script->setData(td.toString(), lower);

  _returnCode = "OK";
  return _returnCode;
}
