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

#include "CommandPlotHistogram.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "HistogramStyle.h"
#include "DataString.h"
#include "DataInteger.h"
#include "DataDouble.h"
#include "DataColor.h"
#include "DataFactory.h"
#include "DataList.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"
#include "ScriptVerifyCurveValue.h"

#include <QtDebug>

CommandPlotHistogram::CommandPlotHistogram (QObject *p) : Command (p)
{
  _name = "PLOT_HISTOGRAM";
  _type = _WAIT;

  _values.insert(_ParmTypeName, new DataString());
  _values.insert(_ParmTypeChart, new DataString());
  _values.insert(_ParmTypeLabel, new DataString());

  DataList *dl = new DataList("Histogram Bar");
  HistogramStyle ls;
  QStringList tl = ls.list();
  dl->set(tl);
  _values.insert(_ParmTypeStyle, dl);

  _values.insert(_ParmTypeColor, new DataColor(QColor(Qt::red)));
  _values.insert(_ParmTypeZ, new DataInteger(0));
  _values.insert(_ParmTypePen, new DataInteger(1));
  _values.insert(_ParmTypeHigh, new DataString());
  _values.insert(_ParmTypeLow, new DataString());
}

void CommandPlotHistogram::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandPlotHistogram::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  QString s = _values.value(_ParmTypeHigh)->toString();
  Data *ihigh = svc.toCurve(script, s, toffset);
  if (! ihigh)
  {
    DataFactory dfac;
    ihigh = dfac.data(DataFactory::_DOUBLE);
    if (! ihigh)
    {
      Command::error("CommandPlotHistogram::runScript: error creating DataDouble High");
      return;
    }

    Command::setTData(ihigh);

    if (ihigh->set(s))
    {
      Command::error("CommandPlotHistogram::runScript: invalid High");
      return;
    }
  }

  s = _values.value(_ParmTypeLow)->toString();
  Data *ilow = svc.toCurve(script, s, toffset);
  if (! ilow)
  {
    DataFactory dfac;
    ilow = dfac.data(DataFactory::_DOUBLE);
    if (! ilow)
    {
      Command::error("CommandPlotHistogram::runScript: error creating DataDouble Low");
      return;
    }

    Command::setTData(ilow);

    if (ilow->set(s))
    {
      Command::error("CommandPlotHistogram::runScript: invalid Low");
      return;
    }
  }

  // keys
  QList<Data *> list;
  list << ihigh << ilow;

  ScriptVerifyCurveKeys svck;
  QList<int> keys;
  if (svck.keys(list, keys))
  {
    Command::error("CommandPlotHistogram::runScript: invalid keys");
    return;
  }

  Data *line = new CurveData;
  line->set(CurveData::_TYPE, new DataString(QString("Histogram")));
  line->set(CurveData::_Z, new DataInteger(_values.value(_ParmTypeZ)->toInteger()));
  line->set(CurveData::_PEN, new DataInteger(_values.value(_ParmTypePen)->toInteger()));
  line->set(CurveData::_LABEL, new DataString(_values.value(_ParmTypeLabel)->toString()));
  line->set(CurveData::_CHART, new DataString(_values.value(_ParmTypeChart)->toString()));
  line->set(CurveData::_STYLE, new DataString(_values.value(_ParmTypeStyle)->toString()));

  ScriptVerifyCurveValue svcv;
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    double v = 0;
    if (svcv.getValue(ihigh, keys, loop, 0, v))
      continue;

    double v2 = 0;
    if (svcv.getValue(ilow, keys, loop, 0, v2))
      continue;

    Data *bar = new CurveBar;
    bar->set(CurveBar::_HIGH, new DataDouble(v));
    bar->set(CurveBar::_LOW, new DataDouble(v2));
    bar->set(CurveBar::_COLOR, new DataColor(_values.value(_ParmTypeColor)->toColor()));
    line->set(keys.at(loop), bar);
  }

  script->setData(_values.value(_ParmTypeName)->toString(), line);

  Command::done(QString());
}
