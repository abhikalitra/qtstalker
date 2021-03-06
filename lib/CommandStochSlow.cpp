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

#include "CommandStochSlow.h"
#include "CurveData.h"
#include "TypeMA.h"
#include "ScriptVerifyCurve.h"
#include "TALibFunction.h"
#include "TypeTALibFunction.h"

#include <QtDebug>

CommandStochSlow::CommandStochSlow ()
{
  _name = "STOCH_SLOW";

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
  td.setLabel(QObject::tr("Period Fast K"));
  Entity::set(QString("PERIOD_FK"), td);
  
  td = Data(3);
  td.setLabel(QObject::tr("Period K"));
  Entity::set(QString("PERIOD_K"), td);
  
  td = Data(3);
  td.setLabel(QObject::tr("Period D"));
  Entity::set(QString("PERIOD_D"), td);
  
  TypeMA mat;
  td = Data(mat.list(), QString("EMA"));
  td.setLabel(QObject::tr("%K MA Type"));
  Entity::set(QString("KMA"), td);
  
  td.setLabel(QObject::tr("%D MA Type"));
  Entity::set(QString("DMA"), td);
}

QString CommandStochSlow::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("HIGH"), td);

  ScriptVerifyCurve svc;
  Entity ihigh;
  if (svc.curve(script, td.toString(), ihigh))
  {
    qDebug() << "CommandStochSlow::run: invalid HIGH" << td.toString();
    return _returnCode;
  }

  Entity::toData(QString("LOW"), td);
  Entity ilow;
  if (svc.curve(script, td.toString(), ilow))
  {
    qDebug() << "CommandStochSlow::run: invalid LOW" << td.toString();
    return _returnCode;
  }

  Entity::toData(QString("CLOSE"), td);
  Entity iclose;
  if (svc.curve(script, td.toString(), iclose))
  {
    qDebug() << "CommandStochSlow::run: invalid CLOSE" << td.toString();
    return _returnCode;
  }

  Data fk, pk, pd, kma, dma;
  Entity::toData(QString("PERIOD_FK"), fk);
  Entity::toData(QString("PERIOD_K"), pk);
  Entity::toData(QString("PERIOD_D"), pd);
  Entity::toData(QString("KMA"), kma);
  Entity::toData(QString("DMA"), dma);
  
  Entity parms;
  parms.set(QString("FUNCTION"), Data(TypeTALibFunction::_STOCHS));
  parms.set(QString("PERIOD_FK"), fk);
  parms.set(QString("PERIOD_K"), pk);
  parms.set(QString("PERIOD_D"), pd);
  parms.set(QString("KMA"), kma);
  parms.set(QString("DMA"), dma);
  
  CurveData kl, dl;
  TALibFunction func;
  if (func.run(parms, 3, ihigh, ilow, iclose, iclose, 2, kl, dl, dl))
  {
    qDebug() << "CommandStochSlow::run: TALibFunction error";
    return _returnCode;
  }

  Entity::toData(QString("K"), td);
  script->setData(td.toString(), kl);

  Entity::toData(QString("D"), td);
  script->setData(td.toString(), dl);

  _returnCode = "OK";
  return _returnCode;
}
