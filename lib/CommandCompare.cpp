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

#include "CommandCompare.h"
#include "TypeOperator.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"
#include "ScriptVerifyCurveValue.h"
#include "Script.h"

#include <QtDebug>

CommandCompare::CommandCompare ()
{
  _name = "COMPARE";

  Data td(QString("close"));
  td.setLabel(QObject::tr("Input"));
  Entity::set(QString("INPUT"), td);
  
  td = Data(0);
  td.setLabel(QObject::tr("Input Offset"));
  Entity::set(QString("INPUT_OFFSET"), td);
  
  td = Data(QString("close"));
  td.setLabel(QObject::tr("Input 2"));
  Entity::set(QString("INPUT2"), td);
  
  td = Data(0);
  td.setLabel(QObject::tr("Input 2 Offset"));
  Entity::set(QString("INPUT2_OFFSET"), td);
  
  td = Data(QString("close"));
  td.setLabel(QObject::tr("Output"));
  Entity::set(QString("OUTPUT"), td);
  
  td = Data(QString("close"));
  td.setLabel(QObject::tr("Output 2"));
  Entity::set(QString("OUTPUT2"), td);

  TypeOperator op;
  td = Data(op.list(), QString("="));
  td.setLabel(QObject::tr("Op"));
  Entity::set(QString("OP"), td);
}

QString CommandCompare::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;

  // verify Input
  Data td;
  Entity::toData(QString("INPUT"), td);
  
  ScriptVerifyCurve svc;
  Entity in;
  if (svc.curve(script, td.toString(), in))
  {
    if (svc.entity(td.toString(), in))
    {
      qDebug() << "CommandCompare::run: invalid INPUT" << td.toString();
      return _returnCode;
    }
  }

  // verify INPUT_OFFSET
  Data offset;
  Entity::toData(QString("INPUT_OFFSET"), offset);
  
  // verify Operator
  Data op;
  Entity::toData(QString("OP"), op);
  
  // verify Input2
  Entity::toData(QString("INPUT2"), td);
  Entity in2;
  if (svc.curve(script, td.toString(), in2))
  {
    if (svc.entity(td.toString(), in2))
    {
      qDebug() << "CommandCompare::run: invalid INPUT2" << td.toString();
      return _returnCode;
    }
  }

  // verify INPUT2_OFFSET
  Data offset2;
  Entity::toData(QString("INPUT2_OFFSET"), offset2);
  
  // verify Output
  Data outName;
  Entity::toData(QString("OUTPUT"), outName);
  Entity out;
  svc.curve(script, outName.toString(), out);

  // verify Output2
  Entity::toData(QString("OUTPUT2"), td);
  Entity out2;
  if (svc.curve(script, td.toString(), out2))
  {
    if (svc.entity(td.toString(), out2))
    {
      qDebug() << "CommandCompare::run: invalid OUTPUT2" << td.toString();
      return _returnCode;
    }
  }

  QList<QString> keys;
  ScriptVerifyCurveKeys svck;
  if (svck.keys2(in, in2, keys))
  {
    qDebug() << "CommandCompare::run: invalid keys";
    return _returnCode;
  }

  ScriptVerifyCurveValue svcv;
  TypeOperator top;
  int comp = top.stringToIndex(op.toString());
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    double v = 0;
    if (svcv.getValue(in, keys, loop, offset.toInteger(), v))
      continue;

    double v2 = 0;
    if (svcv.getValue(in2, keys, loop, offset2.toInteger(), v2))
      continue;

    if (! top.test(v, (TypeOperator::Key) comp, v2))
      continue;

    Entity b;
    out.toEntity(keys.at(loop), b); // try to load existing bar if it exists

    if (svcv.setValue(out2, b, keys.at(loop)))
      continue;

    out.setEntity(keys.at(loop), b);
  }

  script->setData(outName.toString(), out);
  
  _returnCode = "OK";
  return _returnCode;
}
