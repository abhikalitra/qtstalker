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
#include "CurveData.h"
#include "CurveBar.h"
#include "DataDouble.h"
#include "DataFactory.h"
#include "DataOp.h"
#include "DataColor.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"
#include "ScriptVerifyCurveValue.h"

#include <QtDebug>

CommandCompare::CommandCompare (QObject *p) : Command (p)
{
  _name = "COMPARE";
}

void CommandCompare::runScript (CommandParse sg, Script *script)
{
  // we have to do all verifying manually here because of variable inputs
  // format: input, op, input2, output, output2

  if (sg.values() != 5)
  {
    Command::error("CommandCompare::runScript: invalid # of values");
    return;
  }

  // verify Input
  int pos = 0;
  int offset = 0;
  ScriptVerifyCurve svc;
  QString s = sg.value(pos);
  Data *in = svc.toCurve(script, s, offset);
  if (! in)
  {
    // check if object
    in = script->data(s);
    if (! in)
    {
      // is it a number?
      DataFactory dfac;
      in = dfac.data(DataFactory::_DOUBLE);
      if (! in)
      {
        Command::error("CommandCompare::runScript: error creating DataDouble pos " + QString::number(pos));
        return;
      }

      Command::setTData(in);

      if (in->set(s))
      {
        Command::error("CommandCompare::runScript: invalid value pos " + QString::number(pos));
        return;
      }
    }
    else
    {
      if (in->type() != DataFactory::_DOUBLE)
      {
        Command::error("CommandCompare::runScript: invalid DOUBLE object pos " + QString::number(pos));
        return;
      }
    }
  }

  // verify Operator
  pos++;
  s = sg.value(pos);
  Data *op = script->data(s);
  if (! op)
  {
    // is it a operator?
    DataFactory dfac;
    op = dfac.data(DataFactory::_OP);
    if (! op)
    {
      Command::error("CommandCompare::runScript: error creating DataOperator");
      return;
    }

    Command::setTData(op);

    if (op->set(s))
    {
      Command::error("CommandCompare::runScript: invalid value pos " + QString::number(pos));
      return;
    }
  }

  // verify Input2
  pos++;
  int offset2 = 0;
  s = sg.value(pos);
  Data *in2 = svc.toCurve(script, s, offset2);
  if (! in2)
  {
    // check if object
    in2 = script->data(s);
    if (! in2)
    {
      // is it a number?
      DataFactory dfac;
      in2 = dfac.data(DataFactory::_DOUBLE);
      if (! in2)
      {
        Command::error("CommandCompare::runScript: error creating DataDouble pos " + QString::number(pos));
        return;
      }

      Command::setTData(in2);

      if (in2->set(s))
      {
        Command::error("CommandCompare::runScript: invalid value pos " + QString::number(pos));
        return;
      }
    }
    else
    {
      if (in2->type() != DataFactory::_DOUBLE)
      {
        Command::error("CommandCompare::runScript: invalid DOUBLE object pos " + QString::number(pos));
        return;
      }
    }
  }

  // verify Output
  pos++;
  s = sg.value(pos);
  int toffset = 0;
  Data *out = svc.toCurve(script, s, toffset);
  if (! out)
  {
    // create it
    DataFactory dfac;
    out = dfac.data(DataFactory::_CURVE);
    if (! out)
    {
      Command::error("CommandCompare::runScript: error creating Output");
      return;
    }

    script->setData(s, out);
  }

  // verify Output2
  pos++;
  s = sg.value(pos);
  Data *out2 = svc.toCurve(script, s, toffset);
  if (! out2)
  {
    // is it a number?
    DataFactory dfac;
    out2 = dfac.data(DataFactory::_DOUBLE);
    if (! out2)
    {
      Command::error("CommandCompare::runScript: error creating DataDouble Output2");
      return;
    }

    Command::setTData(out2);

    if (out2->set(s))
    {
      // is it a color?
      out2 = dfac.data(DataFactory::_COLOR);
      if (! out2)
      {
        Command::error("CommandCompare::runScript: error creating DataColor Output2");
        return;
      }

      Command::setTData(out2);

      if (out2->set(s))
      {
        Command::error("CommandCompare::runScript: invalid value pos " + QString::number(pos));
        return;
      }
    }
  }

  // verify curve keys
  QList<Data *> list;
  list << in << in2;

  ScriptVerifyCurveKeys svck;
  QList<int> keys;
  if (svck.keys(list, keys))
  {
    Command::error("CommandCompare::runScript: invalid keys");
    return;
  }

  ScriptVerifyCurveValue svcv;
  Operator top;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    double v = 0;
    if (svcv.getValue(in, keys, loop, offset, v))
      continue;

    double v2 = 0;
    if (svcv.getValue(in2, keys, loop, offset2, v2))
      continue;

    if (top.test(v, (Operator::Type) op->toInteger(), v2))
    {
      Data *b = out->toData(keys.at(loop));
      if (! b)
      {
        b = new CurveBar;
        out->set(keys.at(loop), b);
      }

      svcv.setValue(out, out2, b, keys.at(loop));
    }
  }

  Command::done(QString());
}
