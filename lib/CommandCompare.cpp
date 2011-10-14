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
#include "Operator.h"
#include "VerifyDataInput.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "DataDouble.h"
#include "DataFactory.h"

#include <QtDebug>

CommandCompare::CommandCompare (QObject *p) : Command (p)
{
  _name = "COMPARE";
}

int CommandCompare::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // OUTPUT
  QString name;
  QString s = sg->value("OUTPUT");
  if (vdi.toString(script, s, name))
  {
    qDebug() << "CommandCompare::runScript: invalid OUTPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // verify INPUT_1
  s = sg->value("INPUT_1");
  Data *in = vdi.toCurve(script, s);
  if (! in)
  {
    qDebug() << "CommandCompare::runScript: invalid INPUT_1" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  int offset = 0;
  Data *set = in->toData(CurveData::_OFFSET);
  if (set)
    offset = set->toInteger();

  // verify OP
  int op = 0;
  s = sg->value("OP");
  if (vdi.toOp(script, s, op))
  {
    qDebug() << "CommandCompare::runScript: invalid OP" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // verify INPUT_2
  s = sg->value("INPUT_2");
  Data *in2 = vdi.toCurve(script, s);
  if (! in2)
  {
    qDebug() << "CommandCompare::runScript: invalid INPUT_2" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  int offset2 = 0;
  set = in2->toData(CurveData::_OFFSET);
  if (set)
    offset2 = set->toInteger();

  // RESULT_1
  s = sg->value("RESULT_1");
  Data *in3 = vdi.toCurve(script, s);
  if (! in3)
  {
    qDebug() << "CommandCompare::runScript: invalid RESULT_1" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  int offset3 = 0;
  set = in3->toData(CurveData::_OFFSET);
  if (set)
    offset3 = set->toInteger();

  // RESULT_2
  s = sg->value("RESULT_2");
  Data *in4 = vdi.toCurve(script, s);
  if (! in4)
  {
    qDebug() << "CommandCompare::runScript: invalid RESULT_2" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  int offset4 = 0;
  set = in4->toData(CurveData::_OFFSET);
  if (set)
    offset4 = set->toInteger();

  QList<Data *> list;
  list << in << in2 << in3 << in4;

  QList<int> keys;
  if (vdi.curveKeys(list, keys))
  {
    qDebug() << "CommandCompare::runScript: invalid keys";
    emit signalResume((void *) this);
    return _ERROR;
  }

  Operator top;
  Data *line = new CurveData;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    double v = 0;
    if (vdi.curveValue(in, keys, loop, offset, v))
      continue;

    double v2 = 0;
    if (vdi.curveValue(in2, keys, loop, offset2, v2))
      continue;

    double v3 = 0;
    if (vdi.curveValue(in3, keys, loop, offset3, v3))
      continue;

    double v4 = 0;
    if (vdi.curveValue(in4, keys, loop, offset4, v4))
      continue;

    if (top.test(v, (Operator::Type) op, v2))
    {
      Data *b = new CurveBar;
      b->set(CurveBar::_VALUE, new DataDouble(v3));
      line->set(keys.at(loop), b);
    }
    else
    {
      Data *b = new CurveBar;
      b->set(CurveBar::_VALUE, new DataDouble(v4));
      line->set(keys.at(loop), b);
    }
  }

  script->setData(name, line);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}
