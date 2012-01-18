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

#include "CommandMACD.h"
#include "CurveData.h"
#include "ScriptVerifyCurve.h"
#include "TALibFunction.h"
#include "TALibFunctionKey.h"
#include "MAType.h"

#include <QtDebug>

CommandMACD::CommandMACD ()
{
  _name = "MACD";

  Data td;
  td.setLabel(QObject::tr("Output MACD"));
  Entity::set(QString("MACD"), td);

  td.setLabel(QObject::tr("Output Signal"));
  Entity::set(QString("SIGNAL"), td);

  td.setLabel(QObject::tr("Output Histogram"));
  Entity::set(QString("HIST"), td);

  td = Data(QString("close"));
  td.setLabel(QObject::tr("Input"));
  Entity::set(QString("INPUT"), td);
  
  td = Data(12);
  td.setLabel(QObject::tr("Period Fast"));
  Entity::set(QString("PERIOD_FAST"), td);
  
  td = Data(24);
  td.setLabel(QObject::tr("Period Slow"));
  Entity::set(QString("PERIOD_SLOW"), td);
  
  td = Data(9);
  td.setLabel(QObject::tr("Period Signal"));
  Entity::set(QString("PERIOD_SIG"), td);

  MAType mat;
  td = Data(mat.list(), QString("EMA"));
  td.setLabel(QObject::tr("MA Type Fast"));
  Entity::set(QString("MA_FAST"), td);
  
  td.setLabel(QObject::tr("MA Type Slow"));
  Entity::set(QString("MA_SLOW"), td);
  
  td.setLabel(QObject::tr("MA Type Signal"));
  Entity::set(QString("MA_SIG"), td);
}

QString CommandMACD::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("INPUT"), td);

  ScriptVerifyCurve svc;
  Entity in;
  if (svc.curve(script, td.toString(), in))
  {
    qDebug() << "CommandMACD::run: invalid INPUT" << td.toString();
    return _returnCode;
  }

  Data perf, pers, persig, maf, mas, masig;
  Entity::toData(QString("PERIOD_FAST"), perf);
  Entity::toData(QString("PERIOD_SLOW"), pers);
  Entity::toData(QString("PERIOD_SIG"), persig);
  Entity::toData(QString("MA_FAST"), maf);
  Entity::toData(QString("MA_SLOW"), mas);
  Entity::toData(QString("MA_SIG"), masig);
  
  Entity parms;
  parms.set(QString("FUNCTION"), Data(TALibFunctionKey::_MACD));
  parms.set(QString("PERIOD_FAST"), perf);
  parms.set(QString("PERIOD_SLOW"), pers);
  parms.set(QString("PERIOD_SIG"), persig);
  parms.set(QString("MA_FAST"), maf);
  parms.set(QString("MA_SLOW"), mas);
  parms.set(QString("MA_SIG"), masig);
  
  CurveData macd;
  CurveData signal;
  CurveData hist;
  TALibFunction func;
  if (func.run(parms, 1, in, in, in, in, 3, macd, signal, hist))
  {
    qDebug() << "CommandMACD::run: TALibFunction error";
    return _returnCode;
  }

  Entity::toData(QString("MACD"), td);
  script->setData(td.toString(), macd);

  Entity::toData(QString("SIGNAL"), td);
  script->setData(td.toString(), signal);

  Entity::toData(QString("HIST"), td);
  script->setData(td.toString(), hist);

  _returnCode = "OK";
  return _returnCode;
}
