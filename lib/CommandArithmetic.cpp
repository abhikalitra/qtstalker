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
#include "DataDouble.h"
#include "DataString.h"
#include "DataList.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveValue.h"
#include "ScriptVerifyCurveKeys.h"
#include "DataFactory.h"

#include <QtDebug>

CommandArithmetic::CommandArithmetic (QObject *p) : Command (p)
{
  _name = "ARITHMETIC";
  _method << "ADD" << "DIV" << "MULT" << "SUB";

  _values.insert(_ParmTypeOutput, new DataString());
  _values.insert(_ParmTypeInput, new DataString());
  _values.insert(_ParmTypeInput2, new DataString());

  DataList *dl = new DataList("ADD");
  dl->set(_method);
  _values.insert(_ParmTypeMethod, dl);
}

void CommandArithmetic::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandArithmetic::runScript: parse error");
    return;
  }

  int offset = 0;
  ScriptVerifyCurve svc;
  QString s = _values.value(_ParmTypeInput)->toString();
  Data *in = svc.toCurve(script, s, offset);
  if (! in)
  {
    DataFactory dfac;
    in = dfac.data(DataFactory::_DOUBLE);
    if (! in)
    {
      Command::error("CommandArithmetic::runScript: error creating DataDouble Input");
      return;
    }

    Command::setTData(in);

    if (in->set(s))
    {
      Command::error("CommandArithmetic::runScript: invalid Input");
      return;
    }
  }

  int offset2 = 0;
  s = _values.value(_ParmTypeInput2)->toString();
  Data *in2 = svc.toCurve(script, s, offset2);
  if (! in2)
  {
    DataFactory dfac;
    in2 = dfac.data(DataFactory::_DOUBLE);
    if (! in2)
    {
      Command::error("CommandArithmetic::runScript: error creating DataDouble Input2");
      return;
    }

    Command::setTData(in2);

    if (in2->set(s))
    {
      Command::error("CommandArithmetic::runScript: invalid Input2");
      return;
    }
  }

  Data *line = getArithmetic(in, offset, in2, offset2, _values.value(_ParmTypeMethod)->toInteger());
  if (! line)
  {
    Command::error("CommandArithmetic::runScript: getArithmetic error");
    return;
  }

  script->setData(_values.value(_ParmTypeOutput)->toString(), line);

  Command::done(QString());
}

Data * CommandArithmetic::getArithmetic (Data *in, int offset, Data *in2, int offset2, int method)
{
  QList<Data *> list;
  list << in << in2;

  ScriptVerifyCurveKeys svck;
  QList<int> keys;
  if (svck.keys(list, keys))
  {
    qDebug() << "CommandArithmetic::runScript: getArithmetic invalid keys";
    return 0;
  }

  ScriptVerifyCurveValue svcv;
  Data *line = new CurveData;
  int loop = 0;
  for (; loop < keys.count(); loop++)
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
        Data *b = new CurveBar;
        b->set(CurveBar::_VALUE, new DataDouble(v + v2));
        line->set(keys.at(loop), b);
	break;
      }
      case 1: // div
      {
        Data *b = new CurveBar;
        b->set(CurveBar::_VALUE, new DataDouble(v / v2));
        line->set(keys.at(loop), b);
	break;
      }
      case 2: // mult
      {
        Data *b = new CurveBar;
        b->set(CurveBar::_VALUE, new DataDouble(v * v2));
        line->set(keys.at(loop), b);
	break;
      }
      case 3: // sub
      {
        Data *b = new CurveBar;
        b->set(CurveBar::_VALUE, new DataDouble(v - v2));
        line->set(keys.at(loop), b);
	break;
      }
      default:
	break;
    }
  }

  return line;
}
