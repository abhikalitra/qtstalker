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

#include "CommandBreakout.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "KeyCurveBar.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>

CommandBreakout::CommandBreakout ()
{
  _name = "BREAKOUT";
  _method << "ABOVE" << "BELOW";

  Data td(QString("close"));
  td.setLabel(QObject::tr("Input"));
  Entity::set(QString("INPUT"), td);
  
  td.setLabel(QObject::tr("Input 2"));
  Entity::set(QString("INPUT2"), td);

  td = Data(_method, _method.at(0));
  td.setLabel(QObject::tr("Method"));
  Entity::set(QString("METHOD"), td);
}

QString CommandBreakout::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("INPUT"), td);

  ScriptVerifyCurve svc;
  Entity in;
  if (svc.curve(script, td.toString(), in))
  {
    qDebug() << "CommandBreakout::run: invalid INPUT" << td.toString();
    return _returnCode;
  }

  Entity::toData(QString("INPUT2"), td);
  Entity in2;
  if (svc.curve(script, td.toString(), in2))
  {
    qDebug() << "CommandBreakout::run: invalid INPUT2" << td.toString();
    return _returnCode;
  }

  Data method;
  Entity::toData(QString("METHOD"), method);
  
  int flag = 0;
  if (breakout(in, in2, method.toInteger(), flag))
  {
    qDebug() << "CommandBreakout::runScript: breakout error";
    return _returnCode;
  }

  _returnCode = QString::number(flag);
  return _returnCode;
}

int CommandBreakout::breakout (Entity &in, Entity &in2, int method, int &flag)
{
  flag = 0;

  QList<QString> keys;
  ScriptVerifyCurveKeys svck;
  if (svck.keys2(in, in2, keys))
    return 1;

  int end = keys.size() - 1;
  if (end < 0)
    return 1;

  KeyCurveBar cbkeys;
  int loop = 0;
  if (method == 0)
  {
    for (; loop < end - 1; loop++)
    {
      Entity bar;
      if (in.toEntity(keys.at(loop), bar))
        continue;
      Data tv;
      if (bar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), tv))
        continue;

      Entity bar2;
      if (in2.toEntity(keys.at(loop), bar2))
        continue;
      Data tv2;
      if (bar2.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), tv2))
        continue;

      double v = tv.toDouble();
      double v2 = tv2.toDouble();
      if (v > v2)
        return 0;
    }
  }
  else
  {
    for (; loop < end - 1; loop++)
    {
      Entity bar;
      if (in.toEntity(keys.at(loop), bar))
        continue;
      Data tv;
      if (bar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), tv))
        continue;

      Entity bar2;
      if (in2.toEntity(keys.at(loop), bar2))
        continue;
      Data tv2;
      if (bar2.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), tv2))
        continue;

      double v = tv.toDouble();
      double v2 = tv2.toDouble();
      if (v < v2)
        return 0;
    }
  }

  // compare last bar to confirm breakout
  Entity bar;
  if (in.toEntity(keys.at(end), bar))
    return 0;
  Data tv;
  if (bar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), tv))
    return 0;

  Entity bar2;
  if (in2.toEntity(keys.at(end), bar2))
    return 0;
  Data tv2;
  if (bar2.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), tv2))
    return 0;

  double v = tv.toDouble();
  double v2 = tv2.toDouble();
  
  if (method == 0)
  {
    if (v <= v2)
      return 0;
  }
  else
  {
    if (v >= v2)
      return 0;
  }

  flag++;

  return 0;
}
