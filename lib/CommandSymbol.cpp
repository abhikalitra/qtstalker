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
#include "Strip.h"
#include "QuoteDataBase.h"
#include "DateRange.h"
#include "BarLength.h"
#include "Script.h"
#include "Symbol.h"
#include "CurveData.h"
#include "CurveBar.h"

#include <QtDebug>

CommandSymbol::CommandSymbol (QObject *p) : Command (p)
{
  _type = "SYMBOL";
}

int CommandSymbol::runScript (Data *sg, Script *script)
{
  // symbol
  QString s = sg->get("SYMBOL").toString();
  QStringList tl = s.split(":");
  if (tl.count() != 2)
  {
    qDebug() << _type << "::runScript: invalid SYMBOL" << s;
    return _ERROR;
  }
  QString symbol = tl.at(0);
  QString exchange = tl.at(1);

  // length
  s = sg->get("LENGTH").toString();
  BarLength bl;
  int length = bl.stringToType(s);
  if (length == -1)
  {
    qDebug() << _type << "::runScript: invalid LENGTH" << s;
    return _ERROR;
  }

  // range
  s = sg->get("RANGE").toString();
  DateRange dr;
  int range = dr.toType(s);
  if (range == -1)
  {
    qDebug() << _type << "::runScript: invalid RANGE" << s;
    return _ERROR;
  }

  // date
  s = sg->get("DATE").toString();
  Data *dline = new CurveData;
  script->setData(s, dline);

  // open
  s = sg->get("OPEN").toString();
  Data *oline = new CurveData;
  script->setData(s, oline);

  // high
  s = sg->get("HIGH").toString();
  Data *hline = new CurveData;
  script->setData(s, hline);

  // low
  s = sg->get("LOW").toString();
  Data *lline = new CurveData;
  script->setData(s, lline);

  // close
  s = sg->get("CLOSE").toString();
  Data *cline = new CurveData;
  script->setData(s, cline);

  // volume
  s = sg->get("VOLUME").toString();
  Data *vline = new CurveData;
  script->setData(s, vline);

  // oi
  s = sg->get("OI").toString();
  Data *iline = new CurveData;
  script->setData(s, iline);

  Data *bd = new Symbol;
  bd->set(Symbol::_EXCHANGE, QVariant(symbol));
  bd->set(Symbol::_SYMBOL, QVariant(exchange));
  bd->set(Symbol::_LENGTH, QVariant(length));
  bd->set(Symbol::_START_DATE, QVariant(QDateTime()));
  bd->set(Symbol::_END_DATE, QVariant(QDateTime()));
  bd->set(Symbol::_RANGE, QVariant(range));

  // load quotes
  QuoteDataBase db;
  if (db.getBars(bd))
  {
    qDebug() << _type << "::runScript: QuoteDataBase error";
    delete bd;
    return _ERROR;
  }

  int loop = 0;
  QList<int> barKeys = bd->barKeys();
  for (; loop < barKeys.count(); loop++)
  {
    Data *b = bd->getData(barKeys.at(loop));

    Data *cb = new CurveBar;
    cb->set(CurveBar::_DATE, b->get(CurveBar::_DATE));
    dline->set(loop, cb);

    cb = new CurveBar;
    cb->set(CurveBar::_VALUE, b->get(CurveBar::_OPEN));
    oline->set(loop, cb);

    cb = new CurveBar;
    cb->set(CurveBar::_VALUE, b->get(CurveBar::_HIGH));
    hline->set(loop, cb);

    cb = new CurveBar;
    cb->set(CurveBar::_VALUE, b->get(CurveBar::_LOW));
    lline->set(loop, cb);

    cb = new CurveBar;
    cb->set(CurveBar::_VALUE, b->get(CurveBar::_CLOSE));
    cline->set(loop, cb);

    cb = new CurveBar;
    cb->set(CurveBar::_VALUE, b->get(CurveBar::_VOLUME));
    vline->set(loop, cb);

    cb = new CurveBar;
    cb->set(CurveBar::_VALUE, b->get(CurveBar::_OI));
    iline->set(loop, cb);
  }

  delete bd;

  return _OK;
}

Data * CommandSymbol::settings ()
{
  Data *sg = new Data;
  sg->set("DATE", QVariant(QString("date")));
  sg->set("OPEN", QVariant(QString("open")));
  sg->set("HIGH", QVariant(QString("high")));
  sg->set("LOW", QVariant(QString("low")));
  sg->set("CLOSE", QVariant(QString("close")));
  sg->set("VOLUME", QVariant(QString("volume")));
  sg->set("OI", QVariant(QString("oi")));
  sg->set("LENGTH", QVariant(QString("D")));
  sg->set("RANGE", QVariant(QString("1 Year")));
  return sg;
}
