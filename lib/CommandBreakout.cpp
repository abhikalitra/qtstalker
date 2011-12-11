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
#include "DataString.h"
#include "DataList.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>

CommandBreakout::CommandBreakout (QObject *p) : Command (p)
{
  _name = "BREAKOUT";
  _method << "ABOVE" << "BELOW";

  _values.insert(_ParmTypeInput, new DataString());
  _values.insert(_ParmTypeInput2, new DataString());

  DataList *dl = new DataList(_method.at(0));
  dl->set(_method);
  _values.insert(_ParmTypeMethod, dl);
}

void CommandBreakout::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandBreakout::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  Data *in = svc.toCurve(script, _values.value(_ParmTypeInput)->toString(), toffset);
  if (! in)
  {
    Command::error("CommandBreakout::runScript: invalid Input");
    return;
  }

  Data *in2 = svc.toCurve(script, _values.value(_ParmTypeInput2)->toString(), toffset);
  if (! in2)
  {
    Command::error("CommandBreakout::runScript: invalid Input2");
    return;
  }

  int flag = 0;
  int rc = breakout(in, in2, _values.value(_ParmTypeMethod)->toInteger(), flag);
  if (rc)
  {
    Command::error("CommandBreakout::runScript: breakout error");
    return;
  }

  Command::done(QString::number(flag));
}

int CommandBreakout::breakout (Data *in, Data *in2, int method, int &flag)
{
  flag = 0;

  QList<Data *> list;
  list << in << in2;

  ScriptVerifyCurveKeys svck;
  QList<int> keys;
  if (svck.keys(list, keys))
    return 0;

  int end = keys.count() - 1;
  if (end < 0)
    return 1;

  int loop = 0;

  if (method == 0)
  {
    for (; loop < end - 1; loop++)
    {
      Data *bar = in->toData(keys.at(loop));
      if (! bar)
        continue;

      Data *bar2 = in2->toData(keys.at(loop));
      if (! bar2)
        continue;

      double v = bar->toData(CurveBar::_VALUE)->toDouble();
      double v2 = bar2->toData(CurveBar::_VALUE)->toDouble();
      if (v > v2)
        return 0;
    }
  }
  else
  {
    for (; loop < end - 1; loop++)
    {
      Data *bar = in->toData(keys.at(loop));
      if (! bar)
        continue;

      Data *bar2 = in2->toData(keys.at(loop));
      if (! bar2)
        continue;

      double v = bar->toData(CurveBar::_VALUE)->toDouble();
      double v2 = bar2->toData(CurveBar::_VALUE)->toDouble();
      if (v < v2)
        return 0;
    }
  }

  // compare last bar to confirm breakout
  Data *bar = in->toData(keys.at(end));
  if (! bar)
    return 0;

  Data *bar2 = in2->toData(keys.at(end));
  if (! bar2)
    return 0;

  double v = bar->toData(CurveBar::_VALUE)->toDouble();
  double v2 = bar2->toData(CurveBar::_VALUE)->toDouble();

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
