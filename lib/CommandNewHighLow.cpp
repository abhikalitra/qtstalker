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
#include "CurveBar.h"
#include "DataString.h"
#include "DataList.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>

CommandNewHighLow::CommandNewHighLow (QObject *p) : Command (p)
{
  _name = "NEW_HIGH_LOW";
  _method << "HIGH" << "LOW";

  _values.insert(_ParmTypeInput, new DataString());

  DataList *dl = new DataList(_method.at(0));
  dl->set(_method);
  _values.insert(_ParmTypeMethod, dl);
}

void CommandNewHighLow::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandNewHighLow::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  Data *in = svc.toCurve(script, _values.value(_ParmTypeInput)->toString(), toffset);
  if (! in)
  {
    Command::error("CommandNewHighLow::runScript: invalid Input");
    return;
  }

  int flag = 0;
  int rc = getNewHighLow(in, _values.value(_ParmTypeMethod)->toInteger(), flag);
  if (rc)
  {
    Command::error("CommandNewHighLow::runScript: getNewHighLow error");
    return;
  }

  Command::done(QString::number(flag));
}

int CommandNewHighLow::getNewHighLow (Data *in, int method, int &flag)
{
  flag = 0;

  QList<Data *> list;
  list << in;

  ScriptVerifyCurveKeys svck;
  QList<int> keys;
  if (svck.keys(list, keys))
    return 1;

  int loop = 0;
  Data *bar = in->toData(keys.at(loop++));
  double v = bar->toData(CurveBar::_VALUE)->toDouble();

  if (method == 0) // highest
  {
    for (; loop < keys.count() - 2; loop++)
    {
      bar = in->toData(keys.at(loop));
      double tv = bar->toData(CurveBar::_VALUE)->toDouble();
      if (tv > v)
        v = tv;
    }
  }
  else // lowest
  {
    for (; loop < keys.count() - 2; loop++)
    {
      bar = in->toData(keys.at(loop));
      double tv = bar->toData(CurveBar::_VALUE)->toDouble();
      if (tv < v)
        v = tv;
    }
  }

  bar = in->toData(keys.at(keys.count() - 1));
  if (method == 0)
  {
    if (bar->toData(CurveBar::_VALUE)->toDouble() > v)
      flag = 1;
  }
  else
  {
    if (bar->toData(CurveBar::_VALUE)->toDouble() < v)
      flag = 1;
  }

  return 0;
}
