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

#include "CommandSymbol.h"
#include "Symbol.h"
#include "CurveData.h"
#include "CurveBar.h"
#include "DataString.h"
#include "DataInteger.h"
#include "DataDouble.h"
#include "DataDateTime.h"
#include "QuoteDataBase.h"
#include "DataSymbol.h"
#include "DataBarLength.h"
#include "DataDateRange.h"

#include <QtDebug>

CommandSymbol::CommandSymbol (QObject *p) : Command (p)
{
  _name = "SYMBOL";

  _values.insert(_ParmTypeSymbol, new DataSymbol);
  _values.insert(_ParmTypeLength, new DataBarLength);
  _values.insert(_ParmTypeRange, new DataDateRange);
  _values.insert(_ParmTypeDate, new DataString);
  _values.insert(_ParmTypeOpen, new DataString);
  _values.insert(_ParmTypeHigh, new DataString);
  _values.insert(_ParmTypeLow, new DataString);
  _values.insert(_ParmTypeClose, new DataString);
  _values.insert(_ParmTypeVolume, new DataString);
  _values.insert(_ParmTypeOI, new DataString);
}

void CommandSymbol::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandSymbol::runScript: parse error");
    return;
  }

  QStringList tl = _values.value(_ParmTypeSymbol)->toList();
  if (! tl.count())
  {
    Command::error("CommandSymbol::runScript: invalid Symbol");
    return;
  }

  // SYMBOL
  QStringList tl2 = tl.at(0).split(":");
  QString exchange = tl2.at(0);
  QString symbol = tl2.at(1);

  Symbol bd;
  bd.setExchange(exchange);
  bd.setSymbol(symbol);
  bd.setLength(_values.value(_ParmTypeLength)->toInteger());
  bd.setStartDate(QDateTime());
  bd.setEndDate(QDateTime());
  bd.setRange(_values.value(_ParmTypeRange)->toInteger());

  // load quotes
  QuoteDataBase db;
  if (db.getBars(&bd))
  {
    qDebug() << "CommandSymbol::runScript: QuoteDataBase error" << "EXCHANGE=" << exchange << "SYMBOL=" << symbol;
    qDebug() << "CommandSymbol::runScript: LENGTH=" <<_values.value(_ParmTypeLength)->toString() << "RANGE=" << _values.value(_ParmTypeRange)->toString();
    Command::error("CommandSymbol::runScript: QuoteDataBase error");
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
  QList<int> barKeys = bd.barKeys();
  for (; loop < barKeys.count(); loop++)
  {
    Data *b = bd.getData(barKeys.at(loop));

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
