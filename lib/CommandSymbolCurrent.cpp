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

#include "CommandSymbolCurrent.h"
#include "Strip.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "DataString.h"
#include "DataDouble.h"
#include "DataDateTime.h"
#include "GlobalSymbol.h"

#include <QtDebug>
#include <QMutexLocker>

CommandSymbolCurrent::CommandSymbolCurrent (QObject *p) : Command (p)
{
  _name = "SYMBOL_CURRENT";
//  _type = _WAIT;

  _values.insert(_ParmTypeDate, new DataString());
  _values.insert(_ParmTypeOpen, new DataString());
  _values.insert(_ParmTypeHigh, new DataString());
  _values.insert(_ParmTypeLow, new DataString());
  _values.insert(_ParmTypeClose, new DataString());
  _values.insert(_ParmTypeVolume, new DataString());
  _values.insert(_ParmTypeOI, new DataString());
}

void CommandSymbolCurrent::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandSymbolCurrent::runScript: parse error");
    return;
  }

  QMutexLocker locker(&g_symbolMutex);

  Symbol *bd = script->symbol();
  if (! bd)
  {
    Command::error("CommandSymbolCurrent::runScript: invalid symbol");
    return;
  }

  // date
  Data *dline = new CurveData;
  script->setData(_values.value(_ParmTypeDate)->toString(), dline);

  // open
  Data *oline = new CurveData;
  script->setData(_values.value(_ParmTypeOpen)->toString(), oline);

  // high
  Data *hline = new CurveData;
  script->setData(_values.value(_ParmTypeHigh)->toString(), hline);

  // low
  Data *lline = new CurveData;
  script->setData(_values.value(_ParmTypeLow)->toString(), lline);

  // close
  Data *cline = new CurveData;
  script->setData(_values.value(_ParmTypeClose)->toString(), cline);

  // volume
  Data *vline = new CurveData;
  script->setData(_values.value(_ParmTypeVolume)->toString(), vline);

  // oi
  Data *iline = new CurveData;
  script->setData(_values.value(_ParmTypeOI)->toString(), iline);

  int loop = 0;
  QList<int> keys = bd->barKeys();
  for (; loop < keys.count(); loop++)
  {
    Data *b = bd->getData(keys.at(loop));

    Data *db = new CurveBar;
    db->set(CurveBar::_DATE, new DataDateTime(b->toData(CurveBar::_DATE)->toDateTime()));
    dline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new DataDouble(b->toData(CurveBar::_OPEN)->toDouble()));
    oline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new DataDouble(b->toData(CurveBar::_HIGH)->toDouble()));
    hline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new DataDouble(b->toData(CurveBar::_LOW)->toDouble()));
    lline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new DataDouble(b->toData(CurveBar::_CLOSE)->toDouble()));
    cline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new DataDouble(b->toData(CurveBar::_VOLUME)->toDouble()));
    vline->set(loop, db);

    db = new CurveBar;
    db->set(CurveBar::_VALUE, new DataDouble(b->toData(CurveBar::_OI)->toDouble()));
    iline->set(loop, db);
  }

  Command::done(QString());
}
