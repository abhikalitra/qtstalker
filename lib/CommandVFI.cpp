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

#include "CommandVFI.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "KeyCurveBar.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>
#include <cmath>

CommandVFI::CommandVFI ()
{
  _name = "VFI";

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
  
  td = Data(QString("volume"));
  td.setLabel(QObject::tr("Input Volume"));
  Entity::set(QString("VOLUME"), td);
  
  td = Data(10);
  td.setLabel(QObject::tr("Period"));
  Entity::set(QString("PERIOD"), td);
}

QString CommandVFI::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("HIGH"), td);

  ScriptVerifyCurve svc;
  Entity ihigh;
  if (svc.curve(script, td.toString(), ihigh))
  {
    qDebug() << "CommandVFI::run: invalid HIGH" << td.toString();
    return _returnCode;
  }

  Entity::toData(QString("LOW"), td);
  Entity ilow;
  if (svc.curve(script, td.toString(), ilow))
  {
    qDebug() << "CommandVFI::run: invalid LOW" << td.toString();
    return _returnCode;
  }

  Entity::toData(QString("CLOSE"), td);
  Entity iclose;
  if (svc.curve(script, td.toString(), iclose))
  {
    qDebug() << "CommandVFI::run: invalid CLOSE" << td.toString();
    return _returnCode;
  }

  Entity::toData(QString("VOLUME"), td);
  Entity ivol;
  if (svc.curve(script, td.toString(), ivol))
  {
    qDebug() << "CommandVFI::run: invalid CLOSE" << td.toString();
    return _returnCode;
  }

  Data period;
  Entity::toData(QString("PERIOD"), period);
  
  CurveData line;
  if (getVFI(ihigh, ilow, iclose, ivol, period.toInteger(), line))
  {
    qDebug() << "CommandVFI::runScript: getVFI";
    return _returnCode;
  }

  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  _returnCode = "OK";
  return _returnCode;
}

int CommandVFI::getVFI (Entity &ihigh, Entity &ilow, Entity &iclose, Entity &ivol, int period, Entity &line)
{
  QList<int> keys;
  ScriptVerifyCurveKeys svck;
  if (svck.keys4(ihigh, ilow, iclose, ivol, keys))
    return 1;

  KeyCurveBar cbkeys;
  int loop = period;
  for (; loop < keys.size(); loop++)
  {
    Entity hbar;
    if (ihigh.toIndex(keys.at(loop - period), hbar))
      continue;
    Data thigh;
    if (hbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), thigh))
      continue;
    
    Entity lbar;
    if (ilow.toIndex(keys.at(loop - period), lbar))
      continue;
    Data tlow;
    if (lbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), tlow))
      continue;

    Entity cbar;
    if (iclose.toIndex(keys.at(loop - period), cbar))
      continue;
    Data tclose;
    if (cbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), tclose))
      continue;
    
    double inter = 0.0;
    double sma_vol = 0.0;
    int i;
    double close = tclose.toDouble();
    double high = thigh.toDouble();
    double low = tlow.toDouble();
    double typical = (high + low + close) / 3.0;
    for (i = loop - period + 1; i <= loop; i++)
    {
      Entity hbar;
      if (ihigh.toIndex(keys.at(i), hbar))
        continue;
      Data thigh;
      if (hbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), thigh))
        continue;
    
      Entity lbar;
      if (ilow.toIndex(keys.at(i), lbar))
        continue;
      Data tlow;
      if (lbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), tlow))
        continue;

      Entity cbar;
      if (iclose.toIndex(keys.at(i), cbar))
        continue;
      Data tclose;
      if (cbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), tclose))
        continue;

      Entity vbar;
      if (ivol.toIndex(keys.at(i), vbar))
        continue;
      Data tvol;
      if (vbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), tvol))
        continue;
      
      double ytypical = typical;
      close = tclose.toDouble();
      high = thigh.toDouble();
      low = tlow.toDouble();
      typical = (high + low + close) / 3.0;
      double delta = (log(typical) - log(ytypical));
      inter += delta * delta;
      sma_vol += tvol.toDouble();
    }
    inter = 0.2 * sqrt(inter / (double) period) * typical;
    sma_vol /= (double) period;

    if (ihigh.toIndex(keys.at(loop - period), hbar))
      continue;
    if (hbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), thigh))
      continue;
    
    if (ilow.toIndex(keys.at(loop - period), lbar))
      continue;
    if (lbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), tlow))
      continue;

    if (iclose.toIndex(keys.at(loop - period), cbar))
      continue;
    if (cbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), tclose))
      continue;
    
    close = tclose.toDouble();
    high = thigh.toDouble();
    low = tlow.toDouble();
    typical = (high + low + close) / 3.0;
    double t = 0;
    for (i = loop - period + 1; i <= loop; i++)
    {
      Entity hbar;
      if (ihigh.toIndex(keys.at(i), hbar))
        continue;
      Data thigh;
      if (hbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), thigh))
        continue;
    
      Entity lbar;
      if (ilow.toIndex(keys.at(i), lbar))
        continue;
      Data tlow;
      if (lbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), tlow))
        continue;

      Entity cbar;
      if (iclose.toIndex(keys.at(i), cbar))
        continue;
      Data tclose;
      if (cbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), tclose))
        continue;

      Entity vbar;
      if (ivol.toIndex(keys.at(i), vbar))
        continue;
      Data tvol;
      if (vbar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), tvol))
        continue;
      
      double ytypical = typical;
      double volume = tvol.toDouble();
      close = tclose.toDouble();
      high = thigh.toDouble();
      low = tlow.toDouble();
      typical = (high + low + close) / 3.0;

      if (typical > ytypical + inter)
        t = t + log (1.0 + volume / sma_vol);
      else
      {
        if (typical < ytypical - inter)
          t = t - log (1.0 + volume / sma_vol);
      }
    }

    CurveBar b;
    b.set(cbkeys.indexToString(KeyCurveBar::_VALUE), Data(t));
    line.setEntity(keys.at(loop), b);
  }

  return 0;
}
