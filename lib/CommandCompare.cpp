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
#include "SettingDouble.h"
#include "DataFactory.h"
#include "DataSetting.h"

#include <QtDebug>

CommandCompare::CommandCompare (QObject *p) : Command (p)
{
  _name = "COMPARE";
}

int CommandCompare::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // OUTPUT
  QString name = sg->value("OUTPUT");
  if (name.isEmpty())
  {
    _message << "invalid OUTPUT";
    emit signalResume((void *) this);
    return _ERROR;
  }

  // INPUT_1
  QString s = sg->value("INPUT_1");
  Data *in = vdi.curveAll(script, s);
  if (! in)
  {
    _message << "invalid INPUT_1 " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  int offset = in->offset();

  // verify OP
  Operator top;
  s = sg->value("OP");
  Operator::Type op = top.stringToOperator(s);
  if (op == -1)
  {
    _message << "invalid OP " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // verify INPUT_2
  s = sg->value("INPUT_2");
  Data *in2 = vdi.curveAll(script, s);
  if (! in2)
  {
    _message << "invalid INPUT_2 " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  int offset2 = in2->offset();

  // verify RESULT_1
  s = sg->value("RESULT_1");
  Data *in3 = vdi.curveAll(script, s);
  if (! in3)
  {
    _message << "invalid RESULT_1 " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  int offset3 = in3->offset();


  // verify RESULT_2
  s = sg->value("RESULT_2");
  Data *in4 = vdi.curveAll(script, s);
  if (! in4)
  {
    _message << "invalid RESULT_4 " + s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  int offset4 = in4->offset();

  QList<Data *> list;
  list << in << in2 << in3 << in4;

  QList<int> keys;
  if (vdi.curveKeys(list, keys))
  {
    _message << "invalid keys";
    emit signalResume((void *) this);
    return _ERROR;
  }

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

    if (top.test(v, op, v2))
    {
      Data *b = new CurveBar;
      b->set(CurveBar::_VALUE, new SettingDouble(v3));
      line->set(keys.at(loop), b);
    }
    else
    {
      Data *b = new CurveBar;
      b->set(CurveBar::_VALUE, new SettingDouble(v4));
      line->set(keys.at(loop), b);
    }
  }

  script->setData(name, line);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}
