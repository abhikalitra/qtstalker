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

#include "CommandPO.h"
#include "CurveData.h"
#include "ScriptVerifyCurve.h"
#include "TypeMA.h"
#include "TALibFunction.h"
#include "TypeTALibFunction.h"

#include <QtDebug>

CommandPO::CommandPO ()
{
  _name = "PO";
  _method << "APO" << "PPO";

  Data td;
  td.setLabel(QObject::tr("Output"));
  Entity::set(QString("OUTPUT"), td);
  
  td = Data(QString("close"));
  td.setLabel(QObject::tr("Input"));
  Entity::set(QString("INPUT"), td);
  
  td = Data(12);
  td.setLabel(QObject::tr("Period Fast"));
  Entity::set(QString("PERIOD_FAST"), td);
  
  td = Data(24);
  td.setLabel(QObject::tr("Period Slow"));
  Entity::set(QString("PERIOD_SLOW"), td);
  
  TypeMA mat;
  td = Data(mat.list(), QString("EMA"));
  td.setLabel(QObject::tr("MA Type"));
  Entity::set(QString("MA"), td);
  
  td = Data(_method, _method.at(0));
  td.setLabel(QObject::tr("Method"));
  Entity::set(QString("METHOD"), td);
}

QString CommandPO::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("INPUT"), td);

  ScriptVerifyCurve svc;
  Entity in;
  if (svc.curve(script, td.toString(), in))
  {
    qDebug() << "CommandPO::run: invalid INPUT" << td.toString();
    return _returnCode;
  }

  Data fp, sp, method, ma;
  Entity::toData(QString("PERIOD_FAST"), fp);
  Entity::toData(QString("PERIOD_SLOW"), sp);
  Entity::toData(QString("METHOD"), method);
  Entity::toData(QString("MA"), ma);
  
  Entity parms;
  TypeTALibFunction fkeys;
  parms.set(QString("FUNCTION"), Data(fkeys.stringToIndex(method.toString())));
  parms.set(QString("PERIOD_FAST"), fp);
  parms.set(QString("PERIOD_SLOW"), sp);
  parms.set(QString("MA"), ma);
  
  CurveData line;
  TALibFunction func;
  if (func.run(parms, 1, in, in, in, in, 1, line, line, line))
  {
    qDebug() << "CommandPO::run: TALibFunction error";
    return _returnCode;
  }

  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  _returnCode = "OK";
  return _returnCode;
}
