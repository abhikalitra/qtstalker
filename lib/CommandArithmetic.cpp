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

#include "CommandArithmetic.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveValue.h"
#include "ScriptVerifyCurveKeys.h"
#include "KeyCurveBar.h"

#include <QtDebug>

CommandArithmetic::CommandArithmetic ()
{
  _name = "ARITHMETIC";
  _method << "ADD" << "DIV" << "MULT" << "SUB";

  Data td;
  td.setLabel(QObject::tr("Output"));
  Entity::set(QString("OUTPUT"), td);

  td.setLabel(QObject::tr("Input"));
  Entity::set(QString("INPUT"), td);
  
  td.setLabel(QObject::tr("Input 2"));
  Entity::set(QString("INPUT2"), td);

  td = Data(0);
  td.setLabel(QObject::tr("Input Offset"));
  Entity::set(QString("INPUT_OFFSET"), td);

  td.setLabel(QObject::tr("Input 2 Offset"));
  Entity::set(QString("INPUT2_OFFSET"), td);
  
  td = Data(_method, QString("ADD"));
  td.setLabel(QObject::tr("Method"));
  Entity::set(QString("METHOD"), td);
}

QString CommandArithmetic::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Data td;
  Entity::toData(QString("INPUT"), td);

  ScriptVerifyCurve svc;
  Entity in;
  if (svc.curve(script, td.toString(), in))
  {
    if (svc.entity(td.toString(), in))
    {
      qDebug() << "CommandArithmetic::run: invalid INPUT" << td.toString();
      return _returnCode;
    }
  }
  
  Data offset;
  Entity::toData(QString("INPUT_OFFSET"), offset);
  
  Entity::toData(QString("INPUT2"), td);
  Entity in2;
  if (svc.curve(script, td.toString(), in2))
  {
    if (svc.entity(td.toString(), in2))
    {
      qDebug() << "CommandArithmetic::run: invalid INPUT2" << td.toString();
      return _returnCode;
    }
  }
  
  Data offset2;
  Entity::toData(QString("INPUT2_OFFSET"), offset2);
  
  Data method;
  Entity::toData(QString("METHOD"), method);

  CurveData line;
  if (getArithmetic(in, offset.toInteger(), in2, offset2.toInteger(), method.toInteger(), line))
  {
    qDebug() << "CommandArithmetic::run: getArithmetic error";
    return _returnCode;
  }

  Entity::toData(QString("OUTPUT"), td);
  script->setData(td.toString(), line);

  _returnCode = "OK";
  return _returnCode;
}

int CommandArithmetic::getArithmetic (Entity &in, int offset, Entity &in2, int offset2, int method, Entity &line)
{
  QList<QString> keys;
  ScriptVerifyCurveKeys svck;
  if (svck.keys2(in, in2, keys))
  {
    qDebug() << "CommandArithmetic::getArithmetic: invalid keys";
    return 1;
  }

  KeyCurveBar cbkeys;
  ScriptVerifyCurveValue svcv;
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    double v = 0;
    if (svcv.getValue(in, keys, loop, offset, v))
      continue;

    double v2 = 0;
    if (svcv.getValue(in2, keys, loop, offset2, v2))
      continue;

    switch (method)
    {
      case 0: // add
      {
        CurveBar b;
        b.set(cbkeys.indexToString(KeyCurveBar::_VALUE), Data(v + v2));
        line.setEntity(keys.at(loop), b);
	break;
      }
      case 1: // div
      {
        CurveBar b;
        b.set(cbkeys.indexToString(KeyCurveBar::_VALUE), Data(v / v2));
        line.setEntity(keys.at(loop), b);
	break;
      }
      case 2: // mult
      {
        CurveBar b;
        b.set(cbkeys.indexToString(KeyCurveBar::_VALUE), Data(v * v2));
        line.setEntity(keys.at(loop), b);
	break;
      }
      case 3: // sub
      {
        CurveBar b;
        b.set(cbkeys.indexToString(KeyCurveBar::_VALUE), Data(v - v2));
        line.setEntity(keys.at(loop), b);
	break;
      }
      default:
	break;
    }
  }

  return 0;
}
