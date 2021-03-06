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

#include "CommandROC.h"
#include "CurveData.h"
#include "ScriptVerifyCurve.h"
#include "TALibFunction.h"
#include "TypeTALibFunction.h"

#include <QtDebug>

CommandROC::CommandROC ()
{
  _name = "ROC";
  _method << "ROC" << "ROCP" << "ROCR" << "ROCR100";

  Data td;
  td.setLabel(QObject::tr("Output"));
  Entity::set(QString("OUTPUT"), td);
  
  td = Data(QString("close"));
  td.setLabel(QObject::tr("Input"));
  Entity::set(QString("INPUT"), td);
  
  td = Data(10);
  td.setLabel(QObject::tr("Period"));
  Entity::set(QString("PERIOD"), td);

  td = Data(_method, _method.at(0));
  td.setLabel(QObject::tr("Method"));
  Entity::set(QString("METHOD"), td);
}

QString CommandROC::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("INPUT"), td);

  ScriptVerifyCurve svc;
  Entity in;
  if (svc.curve(script, td.toString(), in))
  {
    qDebug() << "CommandROC::run: invalid INPUT" << td.toString();
    return _returnCode;
  }

  Data period, method;
  Entity::toData(QString("PERIOD"), period);
  Entity::toData(QString("METHOD"), method);
  
  Entity parms;
  TypeTALibFunction fkeys;
  parms.set(QString("FUNCTION"), Data(fkeys.stringToIndex(method.toString())));
  parms.set(QString("PERIOD"), period);
  
  CurveData line;
  TALibFunction func;
  if (func.run(parms, 1, in, in, in, in, 1, line, line, line))
  {
    qDebug() << "CommandROC::run: TALibFunction error";
    return _returnCode;
  }

  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  _returnCode = "OK";
  return _returnCode;
}
