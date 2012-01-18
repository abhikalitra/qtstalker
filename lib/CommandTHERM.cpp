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

/* The "Market Thermometer" _indicator is described in
   Dr. Alexander Elder's book _Come Into My Trading Room_, p.162 */

/* Alert Notes
1) enter when therm falls below MA
2) exit when threshold is triggered
3) explosive move expected when therm stays below MA for 5-7 days
*/

#include "CommandTHERM.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "CurveBarKey.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>
#include <cmath>

CommandTHERM::CommandTHERM ()
{
  _name = "THERM";

  Data td;
  td.setLabel(QObject::tr("Output"));
  Entity::set(QString("OUTPUT"), td);
  
  td = Data(QString("high"));
  td.setLabel(QObject::tr("Input High"));
  Entity::set(QString("HIGH"), td);
  
  td = Data(QString("low"));
  td.setLabel(QObject::tr("Input Low"));
  Entity::set(QString("LOW"), td);
}

QString CommandTHERM::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("HIGH"), td);

  ScriptVerifyCurve svc;
  Entity ihigh;
  if (svc.curve(script, td.toString(), ihigh))
  {
    qDebug() << "CommandTHERM::run: invalid HIGH" << td.toString();
    return _returnCode;
  }

  Entity::toData(QString("LOW"), td);
  Entity ilow;
  if (svc.curve(script, td.toString(), ilow))
  {
    qDebug() << "CommandTHERM::run: invalid LOW" << td.toString();
    return _returnCode;
  }

  CurveData line;
  if (getTHERM(ihigh, ilow, line))
  {
    qDebug() << "CommandTHERM::runScript: getTHERM error";
    return _returnCode;
  }

  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  _returnCode = "OK";
  return _returnCode;
}

int CommandTHERM::getTHERM (Entity &ihigh, Entity &ilow, Entity &line)
{
  QList<QString> keys;
  ScriptVerifyCurveKeys svck;
  if (svck.keys2(ihigh, ilow, keys))
    return 1;

  CurveBarKey cbkeys;
  double thermometer = 0;
  int loop = 1;
  for (; loop < keys.count(); loop++)
  {
    Entity hbar;
    if (ihigh.toEntity(keys.at(loop), hbar))
      continue;
    Data high;
    if (hbar.toData(cbkeys.indexToString(CurveBarKey::_VALUE), high))
      continue;

    Entity phbar;
    if (ihigh.toEntity(keys.at(loop - 1), phbar))
      continue;
    Data phigh;
    if (phbar.toData(cbkeys.indexToString(CurveBarKey::_VALUE), phigh))
      continue;

    Entity lbar;
    if (ilow.toEntity(keys.at(loop), lbar))
      continue;
    Data low;
    if (lbar.toData(cbkeys.indexToString(CurveBarKey::_VALUE), low))
      continue;

    Entity plbar;
    if (ilow.toEntity(keys.at(loop - 1), plbar))
      continue;
    Data plow;
    if (plbar.toData(cbkeys.indexToString(CurveBarKey::_VALUE), plow))
      continue;

    double h = fabs(high.toDouble() - phigh.toDouble());
    double lo = fabs(plow.toDouble() - low.toDouble());

    if (h > lo)
      thermometer = h;
    else
      thermometer = lo;

    CurveBar b;
    b.set(cbkeys.indexToString(CurveBarKey::_VALUE), Data(thermometer));
    line.setEntity(keys.at(loop), b);
  }

  return 0;
}
