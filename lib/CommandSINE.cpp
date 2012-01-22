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

#include "CommandSINE.h"
#include "CurveData.h"
#include "ScriptVerifyCurve.h"
#include "TALibFunction.h"
#include "TypeTALibFunction.h"

#include <QtDebug>

CommandSINE::CommandSINE ()
{
  _name = "HT_SINE";

  Data td;
  td.setLabel(QObject::tr("Output Sine"));
  Entity::set(QString("SINE"), td);
  
  td.setLabel(QObject::tr("Output Lead"));
  Entity::set(QString("LEAD"), td);
  
  td = Data(QString("close"));
  td.setLabel(QObject::tr("Input"));
  Entity::set(QString("INPUT"), td);
}

QString CommandSINE::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("INPUT"), td);

  ScriptVerifyCurve svc;
  Entity in;
  if (svc.curve(script, td.toString(), in))
  {
    qDebug() << "CommandSINE::run: invalid INPUT" << td.toString();
    return _returnCode;
  }

  Entity parms;
  parms.set(QString("FUNCTION"), Data(TypeTALibFunction::_SINE));
  
  CurveData sine, lead;
  TALibFunction func;
  if (func.run(parms, 1, in, in, in, in, 2, sine, lead, lead))
  {
    qDebug() << "CommandSINE::run: TALibFunction error";
    return _returnCode;
  }

  Entity::toData(QString("SINE"), td);
  script->setData(td.toString(), sine);

  Entity::toData(QString("LEAD"), td);
  script->setData(td.toString(), lead);

  _returnCode = "OK";
  return _returnCode;
}
