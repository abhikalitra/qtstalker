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
#include "VerifyDataInput.h"
#include "QuoteDataBase.h"

#include <QtDebug>

CommandSymbol::CommandSymbol (QObject *p) : Command (p)
{
  _name = "SYMBOL";
}

int CommandSymbol::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // SYMBOL
  QString tsymbol;
  QString s = sg->value("SYMBOL");
  if (vdi.toString(script, s, tsymbol))
  {
    qDebug() << "CommandSymbol::runScript: invalid SYMBOL" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  QStringList tl = tsymbol.split(":");
  if (tl.count() != 2)
  {
    qDebug() << "CommandSymbol::runScript: invalid SYMBOL" << tsymbol;
    emit signalResume((void *) this);
    return _ERROR;
  }

  QString exchange = tl.at(0);
  QString symbol = tl.at(1);

  // LENGTH
  int length = 7;
  s = sg->value("LENGTH");
  if (vdi.toBarLength(script, s, length))
  {
    qDebug() << "CommandSymbol::runScript: invalid LENGTH, using default" << s;
    length = 7;
  }

  // RANGE
  int range = 5;
  s = sg->value("RANGE");
  if (vdi.toDateRange(script, s, range))
  {
    qDebug() << "CommandSymbol::runScript: invalid RANGE, using default" << s;
    range = 5;
  }

  Symbol bd;
  bd.setExchange(exchange);
  bd.setSymbol(symbol);
  bd.setLength(length);
  bd.setStartDate(QDateTime());
  bd.setEndDate(QDateTime());
  bd.setRange(range);

  // load quotes
  QuoteDataBase db;
  if (db.getBars(&bd))
  {
    qDebug() << "CommandSymbol::runScript: QuoteDataBase error" << "EXCHANGE=" << exchange << "SYMBOL=" << symbol;
    qDebug() << "CommandSymbol::runScript: LENGTH=" << length << "RANGE=" << range;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // date
  QString ts;
  s = sg->value("DATE");
  if (vdi.toString(script, s, ts))
  {
    qDebug() << "CommandSymbol::runScript: invalid DATE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  Data *dline = new CurveData;
  script->setData(ts, dline);

  // open
  s = sg->value("OPEN");
  if (vdi.toString(script, s, ts))
  {
    qDebug() << "CommandSymbol::runScript: invalid OPEN" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  Data *oline = new CurveData;
  script->setData(ts, oline);

  // high
  s = sg->value("HIGH");
  if (vdi.toString(script, s, ts))
  {
    qDebug() << "CommandSymbol::runScript: invalid HIGH" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  Data *hline = new CurveData;
  script->setData(ts, hline);

  // low
  s = sg->value("LOW");
  if (vdi.toString(script, s, ts))
  {
    qDebug() << "CommandSymbol::runScript: invalid LOW" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  Data *lline = new CurveData;
  script->setData(ts, lline);

  // close
  s = sg->value("CLOSE");
  if (vdi.toString(script, s, ts))
  {
    qDebug() << "CommandSymbol::runScript: invalid CLOSE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  Data *cline = new CurveData;
  script->setData(ts, cline);

  // volume
  s = sg->value("VOLUME");
  if (vdi.toString(script, s, ts))
  {
    qDebug() << "CommandSymbol::runScript: invalid VOLUME" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  Data *vline = new CurveData;
  script->setData(ts, vline);

  // oi
  s = sg->value("OI");
  if (vdi.toString(script, s, ts))
  {
    qDebug() << "CommandSymbol::runScript: invalid OI" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }
  Data *iline = new CurveData;
  script->setData(ts, iline);

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

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}
