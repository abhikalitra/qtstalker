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

#include "CommandPrice.h"
#include "CurveData.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"
#include "CurveBar.h"
#include "KeyCurveBar.h"

#include <QtDebug>

CommandPrice::CommandPrice ()
{
  _name = "PRICE";
  _method << "AVERAGE" << "TYPICAL" << "WEIGHTED";

  Data td;
  td.setLabel(QObject::tr("Output"));
  Entity::set(QString("OUTPUT"), td);
  
  td = Data(QString("open"));
  td.setLabel(QObject::tr("Input Open"));
  Entity::set(QString("OPEN"), td);
  
  td = Data(QString("high"));
  td.setLabel(QObject::tr("Input High"));
  Entity::set(QString("HIGH"), td);
  
  td = Data(QString("low"));
  td.setLabel(QObject::tr("Input Low"));
  Entity::set(QString("LOW"), td);
  
  td = Data(QString("close"));
  td.setLabel(QObject::tr("Input Close"));
  Entity::set(QString("CLOSE"), td);
  
  td = Data(_method, _method.at(0));
  td.setLabel(QObject::tr("Method"));
  Entity::set(QString("METHOD"), td);
}

QString CommandPrice::run (CommandParse &, void *d)
{
  Data method;
  Entity::toData(QString("METHOD"), method);
  
  switch (_method.indexOf(method.toString()))
  {
    case 0: // AVERAGE
      if (getAP(d))
        return _returnCode;
      break;
    case 1: // TYPICAL
      if (getTP(d))
        return _returnCode;
      break;
    case 2: // WEIGHTED
      if (getWC(d))
        return _returnCode;
      break;
    default:
      qDebug() << "CommandPrice::run: invalid method" << method.toString();
      return _returnCode;
      break;
  }
  
  _returnCode = "OK";
  return _returnCode;
}

int CommandPrice::getAP (void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("OPEN"), td);

  ScriptVerifyCurve svc;
  Entity iopen;
  if (svc.curve(script, td.toString(), iopen))
  {
    qDebug() << "CommandPrice::getAP: invalid OPEN" << td.toString();
    return 1;
  }
  
  Entity ihigh;
  Entity::toData(QString("HIGH"), td);
  if (svc.curve(script, td.toString(), ihigh))
  {
    qDebug() << "CommandPrice::getAP: invalid HIGH" << td.toString();
    return 1;
  }

  Entity ilow;
  Entity::toData(QString("LOW"), td);
  if (svc.curve(script, td.toString(), ilow))
  {
    qDebug() << "CommandPrice::getAP: invalid LOW" << td.toString();
    return 1;
  }

  Entity iclose;
  Entity::toData(QString("CLOSE"), td);
  if (svc.curve(script, td.toString(), iclose))
  {
    qDebug() << "CommandPrice::getAP: invalid CLOSE" << td.toString();
    return 1;
  }

  CurveData line;
  if (getAP2(iopen, ihigh, ilow, iclose, line))
  {
    qDebug() << "CommandPrice::getAP: getAP2 error";
    return 1;
  }

  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  return 0;
}

int CommandPrice::getAP2 (Entity &iopen, Entity &ihigh, Entity &ilow, Entity &iclose, Entity &line)
{
  QList<int> keys;
  ScriptVerifyCurveKeys svck;
  if (svck.keys4(iopen, ihigh, ilow, iclose, keys))
    return 1;

  KeyCurveBar cbkeys;
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Entity obar;
    if (iopen.toIndex(keys.at(loop), obar))
      continue;
    Data open;
    if (obar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), open))
      continue;

    Entity hbar;
    if (ihigh.toIndex(keys.at(loop), hbar))
      continue;
    Data high;
    if (hbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), high))
      continue;

    Entity lbar;
    if (ilow.toIndex(keys.at(loop), lbar))
      continue;
    Data low;
    if (lbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), low))
      continue;

    Entity cbar;
    if (iclose.toIndex(keys.at(loop), cbar))
      continue;
    Data close;
    if (cbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), close))
      continue;

    double t = (open.toDouble() + high.toDouble() + low.toDouble() + close.toDouble()) / 4.0;
    CurveBar b;
    b.set(cbkeys.indexToString(KeyCurveBar::_VALUE), Data(t));
    line.setEntity(keys.at(loop), b);
  }

  return 0;
}

int CommandPrice::getTP (void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("HIGH"), td);

  ScriptVerifyCurve svc;
  Entity ihigh;
  if (svc.curve(script, td.toString(), ihigh))
  {
    qDebug() << "CommandPrice::getTP: invalid HIGH" << td.toString();
    return 1;
  }

  Entity::toData(QString("LOW"), td);
  Entity ilow;
  if (svc.curve(script, td.toString(), ilow))
  {
    qDebug() << "CommandPrice::getTP: invalid LOW" << td.toString();
    return 1;
  }

  Entity::toData(QString("CLOSE"), td);
  Entity iclose;
  if (svc.curve(script, td.toString(), iclose))
  {
    qDebug() << "CommandPrice::getTP: invalid CLOSE" << td.toString();
    return 1;
  }

  CurveData line;
  if (getTP2(ihigh, ilow, iclose, line))
  {
    qDebug() << "CommandPrice::getTP: getWC2 error";
    return 1;
  }

  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  return 0;
}

int CommandPrice::getTP2 (Entity &ihigh, Entity &ilow, Entity &iclose, Entity &line)
{
  QList<int> keys;
  ScriptVerifyCurveKeys svck;
  if (svck.keys3(ihigh, ilow, iclose, keys))
    return 1;

  KeyCurveBar cbkeys;
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Entity hbar;
    if (ihigh.toIndex(keys.at(loop), hbar))
      continue;
    Data high;
    if (hbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), high))
      continue;
    
    Entity lbar;
    if (ilow.toIndex(keys.at(loop), lbar))
      continue;
    Data low;
    if (lbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), low))
      continue;

    Entity cbar;
    if (iclose.toIndex(keys.at(loop), cbar))
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

int CommandPrice::getWC (void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("HIGH"), td);

  ScriptVerifyCurve svc;
  Entity ihigh;
  if (svc.curve(script, td.toString(), ihigh))
  {
    qDebug() << "CommandPrice::getWC: invalid HIGH" << td.toString();
    return 1;
  }

  Entity::toData(QString("LOW"), td);
  Entity ilow;
  if (svc.curve(script, td.toString(), ilow))
  {
    qDebug() << "CommandPrice::getWC: invalid LOW" << td.toString();
    return 1;
  }

  Entity::toData(QString("CLOSE"), td);
  Entity iclose;
  if (svc.curve(script, td.toString(), iclose))
  {
    qDebug() << "CommandPrice::getWC: invalid CLOSE" << td.toString();
    return 1;
  }

  CurveData line;
  if (getWC2(ihigh, ilow, iclose, line))
  {
    qDebug() << "CommandPrice::getWC: getWC2 error";
    return 1;
  }

  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  return 0;
}

int CommandPrice::getWC2 (Entity &ihigh, Entity &ilow, Entity &iclose, Entity &line)
{
  QList<int> keys;
  ScriptVerifyCurveKeys svck;
  if (svck.keys3(ihigh, ilow, iclose, keys))
    return 1;

  KeyCurveBar cbkeys;
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Entity hbar;
    if (ihigh.toIndex(keys.at(loop), hbar))
      continue;
    Data high;
    if (hbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), high))
      continue;
    
    Entity lbar;
    if (ilow.toIndex(keys.at(loop), lbar))
      continue;
    Data low;
    if (lbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), low))
      continue;

    Entity cbar;
    if (iclose.toIndex(keys.at(loop), cbar))
      continue;
    Data close;
    if (cbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), close))
      continue;

    double t = (high.toDouble() + low.toDouble() + (close.toDouble() * 2)) / 4.0;

    CurveBar b;
    b.set(cbkeys.indexToString(KeyCurveBar::_VALUE), Data(t));
    line.setEntity(keys.at(loop), b);
  }

  return 0;
}
