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

#include "CommandPlotOHLC.h"
#include "OHLCStyle.h"
#include "CurveData.h"
#include "DataString.h"
#include "DataInteger.h"
#include "DataDouble.h"
#include "DataColor.h"
#include "CurveBar.h"
#include "DataList.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>

CommandPlotOHLC::CommandPlotOHLC (QObject *p) : Command (p)
{
  _name = "PLOT_OHLC";
  _type = _WAIT;

  _values.insert(_ParmTypeName, new DataString());
  _values.insert(_ParmTypeChart, new DataString());
  _values.insert(_ParmTypeLabel, new DataString());

  DataList *dl = new DataList("OHLC");
  QStringList tl;
  tl << "OHLC" << "Candle";
  dl->set(tl);
  _values.insert(_ParmTypeStyle, dl);

  _values.insert(_ParmTypeColor, new DataColor());
  _values.insert(_ParmTypeZ, new DataInteger(0));
  _values.insert(_ParmTypePen, new DataInteger(1));
  _values.insert(_ParmTypeOpen, new DataString());
  _values.insert(_ParmTypeHigh, new DataString());
  _values.insert(_ParmTypeLow, new DataString());
  _values.insert(_ParmTypeClose, new DataString());
}

void CommandPlotOHLC::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandPlotOHLC::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  Data *iopen = svc.toCurve(script, _values.value(_ParmTypeOpen)->toString(), toffset);
  if (! iopen)
  {
    Command::error("CommandPlotOHLC::runScript invalid OPEN");
    return;
  }

  Data *ihigh = svc.toCurve(script, _values.value(_ParmTypeHigh)->toString(), toffset);
  if (! ihigh)
  {
    Command::error("CommandPlotOHLC::runScript invalid HIGH");
    return;
  }

  Data *ilow = svc.toCurve(script, _values.value(_ParmTypeLow)->toString(), toffset);
  if (! ilow)
  {
    Command::error("CommandPlotOHLC::runScript invalid LOW");
    return;
  }

  Data *iclose = svc.toCurve(script, _values.value(_ParmTypeClose)->toString(), toffset);
  if (! iclose)
  {
    Command::error("CommandPlotOHLC::runScript invalid CLOSE");
    return;
  }

  QList<Data *> list;
  list << iopen << ihigh << ilow << iclose;

  ScriptVerifyCurveKeys svck;
  QList<int> keys;
  if (svck.keys(list, keys))
  {
    Command::error("CommandPlotOHLC::runScript invalid keys");
    return;
  }

  Data *line = new CurveData;
  line->set(CurveData::_TYPE, new DataString(QString("OHLC")));
  line->set(CurveData::_Z, new DataInteger(_values.value(_ParmTypeZ)->toInteger()));
  line->set(CurveData::_PEN, new DataInteger(_values.value(_ParmTypePen)->toInteger()));
  line->set(CurveData::_LABEL, new DataString(_values.value(_ParmTypeLabel)->toString()));
  line->set(CurveData::_CHART, new DataString(_values.value(_ParmTypeChart)->toString()));
  line->set(CurveData::_STYLE, new DataString(_values.value(_ParmTypeStyle)->toString()));

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Data *obar = iopen->toData(keys.at(loop));
    if (! obar)
      continue;

    Data *hbar = ihigh->toData(keys.at(loop));
    if (! hbar)
      continue;

    Data *lbar = ilow->toData(keys.at(loop));
    if (! lbar)
      continue;

    Data *cbar = iclose->toData(keys.at(loop));
    if (! cbar)
      continue;

    Data *bar = new CurveBar;
    bar->set(CurveBar::_OPEN, new DataDouble(obar->toData(CurveBar::_VALUE)->toDouble()));
    bar->set(CurveBar::_HIGH, new DataDouble(hbar->toData(CurveBar::_VALUE)->toDouble()));
    bar->set(CurveBar::_LOW, new DataDouble(lbar->toData(CurveBar::_VALUE)->toDouble()));
    bar->set(CurveBar::_CLOSE, new DataDouble(cbar->toData(CurveBar::_VALUE)->toDouble()));
    bar->set(CurveBar::_COLOR, new DataColor(_values.value(_ParmTypeColor)->toColor()));
    line->set(keys.at(loop), bar);
  }

  script->setData(_values.value(_ParmTypeName)->toString(), line);

  Command::done(QString());
}
