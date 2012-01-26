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

#include "CommandNewHighLow.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "KeyCurveBar.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>

CommandNewHighLow::CommandNewHighLow ()
{
  _name = "NEW_HIGH_LOW";
  _method << "HIGH" << "LOW";

  Data td(QString("close"));
  td.setLabel(QObject::tr("Input"));
  Entity::set(QString("INPUT"), td);
  
  td = Data(_method, _method.at(0));
  td.setLabel(QObject::tr("Method"));
  Entity::set(QString("METHOD"), td);
}

QString CommandNewHighLow::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("INPUT"), td);

  ScriptVerifyCurve svc;
  Entity in;
  if (svc.curve(script, td.toString(), in))
  {
    qDebug() << "CommandNewHighLow::run: invalid INPUT" << td.toString();
    return _returnCode;
  }

  Data method;
  Entity::toData(QString("METHOD"), method);

  int flag = 0;
  if (getNewHighLow(in, method.toInteger(), flag))
  {
    qDebug() << "CommandNewHighLow::run: getNewHighLow error";
    return _returnCode;
  }

  _returnCode = QString::number(flag);
  return _returnCode;
}

int CommandNewHighLow::getNewHighLow (Entity &in, int method, int &flag)
{
  flag = 0;

  QList<int> keys;
  ScriptVerifyCurveKeys svck;
  if (svck.keys1(in, keys))
    return 1;

  KeyCurveBar cbkeys;
  int loop = 0;
  Entity bar;
  in.toIndex(keys.at(loop++), bar);
  Data val;
  bar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), val);
  double v = val.toDouble();

  if (method == 0) // highest
  {
    for (; loop < keys.size() - 2; loop++)
    {
      in.toIndex(keys.at(loop), bar);
      bar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), val);
      double tv = val.toDouble();
      if (tv > v)
        v = tv;
    }
  }
  else // lowest
  {
    for (; loop < keys.size() - 2; loop++)
    {
      in.toIndex(keys.at(loop), bar);
      bar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), val);
      double tv = val.toDouble();
      if (tv < v)
        v = tv;
    }
  }

  in.toIndex(keys.at(keys.size() - 1), bar);
  bar.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), val);
  if (method == 0)
  {
    if (val.toDouble() > v)
      flag = 1;
  }
  else
  {
    if (val.toDouble() < v)
      flag = 1;
  }

  return 0;
}
