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

#include "CommandPlotLine.h"
#include "LineStyle.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "DataString.h"
#include "DataInteger.h"
#include "DataColor.h"
#include "DataDouble.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"
#include "DataList.h"

#include <QtDebug>

CommandPlotLine::CommandPlotLine (QObject *p) : Command (p)
{
  _name = "PLOT_LINE";
  _type = _WAIT;

  _values.insert(_ParmTypeName, new DataString());
  _values.insert(_ParmTypeChart, new DataString());
  _values.insert(_ParmTypeLabel, new DataString());

  DataList *dl = new DataList("Line");
  LineStyle ls;
  QStringList tl = ls.list();
  dl->set(tl);
  _values.insert(_ParmTypeStyle, dl);

  _values.insert(_ParmTypeColor, new DataColor());
  _values.insert(_ParmTypeZ, new DataInteger(0));
  _values.insert(_ParmTypePen, new DataInteger(1));
  _values.insert(_ParmTypeInput, new DataString());
}

void CommandPlotLine::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandPlotLine::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  Data *in = svc.toCurve(script, _values.value(_ParmTypeInput)->toString(), toffset);
  if (! in)
  {
    Command::error("CommandPlotLine::runScript: invalid Input");
    return;
  }

  QList<Data *> list;
  list << in;

  ScriptVerifyCurveKeys svck;
  QList<int> keys;
  if (svck.keys(list, keys))
  {
    Command::error("CommandPlotLine::runScript: invalid keys");
    return;
  }

  Data *line = new CurveData;
  line->set(CurveData::_TYPE, new DataString(QString("Line")));
  line->set(CurveData::_Z, new DataInteger(_values.value(_ParmTypeZ)->toInteger()));
  line->set(CurveData::_PEN, new DataInteger(_values.value(_ParmTypePen)->toInteger()));
  line->set(CurveData::_LABEL, new DataString(_values.value(_ParmTypeLabel)->toString()));
  line->set(CurveData::_CHART, new DataString(_values.value(_ParmTypeChart)->toString()));
  line->set(CurveData::_STYLE, new DataString(_values.value(_ParmTypeStyle)->toString()));

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *ibar = in->toData(keys.at(loop));
    if (! ibar)
      continue;

    Data *bar = new CurveBar;
    bar->set(CurveBar::_VALUE, new DataDouble(ibar->toData(CurveBar::_VALUE)->toDouble()));
    bar->set(CurveBar::_COLOR, new DataColor(_values.value(_ParmTypeColor)->toColor()));
    line->set(keys.at(loop), bar);
  }

  script->setData(_values.value(_ParmTypeName)->toString(), line);

  Command::done(QString());
}
