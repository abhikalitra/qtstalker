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

#include "CommandVBP.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "DataDouble.h"
#include "DataColor.h"
#include "DataString.h"
#include "ScriptVerifyCurve.h"
#include "ScriptVerifyCurveKeys.h"

#include <QtDebug>
#include <QHash>

CommandVBP::CommandVBP (QObject *p) : Command (p)
{
  _name = "VBP";

  _values.insert(_ParmTypeOutput, new DataString());
  _values.insert(_ParmTypeClose, new DataString());
  _values.insert(_ParmTypeVolume, new DataString());
  _values.insert(_ParmTypeColorUp, new DataColor(QColor(Qt::green)));
  _values.insert(_ParmTypeColorDown, new DataColor(QColor(Qt::red)));
}

void CommandVBP::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandVBP::runScript: parse error");
    return;
  }

  int toffset = 0;
  ScriptVerifyCurve svc;
  Data *iclose = svc.toCurve(script, _values.value(_ParmTypeClose)->toString(), toffset);
  if (! iclose)
  {
    Command::error("CommandVBP::runScript: invalid Close");
    return;
  }

  Data *ivol = svc.toCurve(script, _values.value(_ParmTypeVolume)->toString(), toffset);
  if (! ivol)
  {
    Command::error("CommandVBP::runScript: invalid Volume");
    return;
  }

  QList<Data *> list;
  list << iclose << ivol;

  Data *line = getVBP(list,
		      _values.value(_ParmTypeColorUp)->toColor(),
		      _values.value(_ParmTypeColorDown)->toColor());
  if (! line)
  {
    Command::error("CommandVBP::runScript: getVBP error");
    return;
  }

  script->setData(_values.value(_ParmTypeOutput)->toString(), line);

  Command::done(QString());
}

Data * CommandVBP::getVBP (QList<Data *> &list, QColor upColor, QColor downColor)
{
  if (list.count() != 2)
    return 0;

  ScriptVerifyCurveKeys svck;
  QList<int> keys;
  if (svck.keys(list, keys))
    return 0;

  // get close high/low
  int loop = 0;
  double high = 0;
  double low = 0;
  Data *close = list.at(0);
  Data *vol = list.at(1);
  if (close->highLow(high, low))
    return 0;

  // get the price block range
  double range = (high - low) / 12;
  QHash<int, Data *> hash;
  double bottom = low;
  double top = bottom + range;
  loop = 0;
  for (; loop < 12; loop++)
  {
    Data *set = new CurveBar;
    set->set(CurveBar::_HIGH, new DataDouble(top));
    set->set(CurveBar::_LOW, new DataDouble(bottom));
    set->set(CurveBar::_UP, new DataDouble(0));
    set->set(CurveBar::_DOWN, new DataDouble(0));
    hash.insert(loop, set);

    bottom = top + 0.01;
    top = bottom + range;
  }

  loop = 1;
  for (; loop < keys.count(); loop++)
  {
    Data *cb = close->toData(keys.at(loop));
    if (! cb)
      continue;

    Data *ycb = close->toData(keys.at(loop - 1));
    if (! ycb)
      continue;

    Data *vb = vol->toData(keys.at(loop));
    if (! vb)
      continue;

    int flag = 1;
    if (cb->toData(CurveBar::_VALUE)->toDouble() < ycb->toData(CurveBar::_VALUE)->toDouble())
      flag = 0;

    QHashIterator<int, Data *> it(hash);
    while (it.hasNext())
    {
      it.next();
      Data *set = it.value();

      if (cb->toData(CurveBar::_VALUE)->toDouble() >= set->toData(CurveBar::_LOW)->toDouble() && cb->toData(CurveBar::_VALUE)->toDouble() <= set->toData(CurveBar::_HIGH)->toDouble())
      {
        if (flag)
	  set->set(CurveBar::_UP, new DataDouble(set->toData(CurveBar::_UP)->toDouble() + vb->toData(CurveBar::_VALUE)->toDouble()));
	else
	  set->set(CurveBar::_DOWN, new DataDouble(set->toData(CurveBar::_DOWN)->toDouble() + vb->toData(CurveBar::_VALUE)->toDouble()));
	break;
      }
    }
  }

  Data *line = new CurveData;
//  line->setType("HistogramHorizontal");

  loop = 0;
  int pos = 0;
  for (; loop < 12; loop++)
  {
    Data *set = hash.value(loop);
    if (! set)
      continue;

    Data *b = new CurveBar;
    b->set(CurveBar::_HIGH, new DataDouble(set->toData(CurveBar::_HIGH)->toDouble()));
    b->set(CurveBar::_LOW, new DataDouble(set->toData(CurveBar::_LOW)->toDouble()));
    b->set(CurveBar::_OPEN, new DataDouble(set->toData(CurveBar::_UP)->toDouble()));
    b->set(CurveBar::_CLOSE, new DataDouble(set->toData(CurveBar::_DOWN)->toDouble()));
    b->set(CurveBar::_COLOR, new DataColor(upColor));
    line->set(pos++, b);

    b = new CurveBar;
    b->set(CurveBar::_HIGH, new DataDouble(set->toData(CurveBar::_HIGH)->toDouble()));
    b->set(CurveBar::_LOW, new DataDouble(set->toData(CurveBar::_LOW)->toDouble()));
    b->set(CurveBar::_OPEN, new DataDouble(set->toData(CurveBar::_UP)->toDouble()));
    b->set(CurveBar::_CLOSE, new DataDouble(set->toData(CurveBar::_DOWN)->toDouble()));
    b->set(CurveBar::_COLOR, new DataColor(downColor));
    line->set(pos++, b);
  }

  qDeleteAll(hash);

  return line;
}
