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

#include "CommandTypicalPrice.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "KeyCurveBar.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>

CommandTypicalPrice::CommandTypicalPrice ()
{
  _name = "TYPICAL_PRICE";

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
}

QString CommandTypicalPrice::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("HIGH"), td);

  ScriptVerifyCurve svc;
  Entity ihigh;
  if (svc.curve(script, td.toString(), ihigh))
  {
    qDebug() << "CommandTypicalPrice::run: invalid HIGH" << td.toString();
    return _returnCode;
  }

  Entity::toData(QString("LOW"), td);
  Entity ilow;
  if (svc.curve(script, td.toString(), ilow))
  {
    qDebug() << "CommandTypicalPrice::run: invalid LOW" << td.toString();
    return _returnCode;
  }

  Entity::toData(QString("CLOSE"), td);
  Entity iclose;
  if (svc.curve(script, td.toString(), iclose))
  {
    qDebug() << "CommandTypicalPrice::run: invalid CLOSE" << td.toString();
    return _returnCode;
  }

  CurveData line;
  if (getTP(ihigh, ilow, iclose, line))
  {
    qDebug() << "CommandTypicalPrice::runScript: getWC error";
    return _returnCode;
  }

  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  _returnCode = "OK";
  return _returnCode;
}

int CommandTypicalPrice::getTP (Entity &ihigh, Entity &ilow, Entity &iclose, Entity &line)
{
  QList<QString> keys;
  ScriptVerifyCurveKeys svck;
  if (svck.keys3(ihigh, ilow, iclose, keys))
    return 1;

  KeyCurveBar cbkeys;
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Entity hbar;
    if (ihigh.toEntity(keys.at(loop), hbar))
      continue;
    Data high;
    if (hbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), high))
      continue;
    
    Entity lbar;
    if (ilow.toEntity(keys.at(loop), lbar))
      continue;
    Data low;
    if (lbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), low))
      continue;

    Entity cbar;
    if (iclose.toEntity(keys.at(loop), cbar))
      continue;
    Data close;
    if (cbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), close))
      continue;

    double t = (high.toDouble() + low.toDouble() + close.toDouble()) / 3.0;

    CurveBar b;
    b.set(cbkeys.indexToString(KeyCurveBar::_VALUE), Data(t));
    line.setEntity(keys.at(loop), b);
  }

  return 0;
}
