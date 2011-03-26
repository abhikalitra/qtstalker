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

#include "STOCK_SPLIT.h"
#include "Curve.h"
#include "Globals.h"
#include "DateRange.h"
#include "QuoteDataBase.h"

#include <QtDebug>

STOCK_SPLIT::STOCK_SPLIT ()
{
  _plugin = "STOCK_SPLIT";
}

int STOCK_SPLIT::command (Command *command)
{
  // PARMS:
  // EXCHANGE
  // SYMBOL
  // DATE
  // SPLIT

  BarData symbol;
  
  // verify EXCHANGE
  QString s = command->parm("EXCHANGE");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::command: invalid EXCHANGE" << s;
    return 1;
  }
  symbol.setExchange(s);

  // verify SYMBOL
  s = command->parm("SYMBOL");
  if (s.isEmpty())
  {
    qDebug() << _plugin << "::command: invalid SYMBOL" << s;
    return 1;
  }
  symbol.setSymbol(s);

  s = command->parm("DATE");
  QDateTime splitDate = QDateTime::fromString(s, Qt::ISODate);
  if (! splitDate.isValid())
  {
    qDebug() << _plugin << "::command: invalid DATE" << s;
    return 1;
  }

  QStringList l = command->parm("SPLIT").split(":", QString::SkipEmptyParts);
  if (l.count() != 2)
  {
    qDebug() << _plugin << "::command: invalid SPLIT" << l;
    return 1;
  }

  double divisor = l.at(0).toDouble() / l.at(1).toDouble();
  if (divisor == 0)
  {
    qDebug() << _plugin << "::command: invalid divisor";
    return 1;
  }

  DateRange dr;
  symbol.setRange(dr.toType("All"));

  symbol.setBarLength(0);

  QuoteDataBase db;
  if (db.getBars(&symbol))
  {
    qDebug() << _plugin << "::command: QuoteDataBase error";
    return 1;
  }

  BarData symbol2;
  symbol2.setExchange(symbol.exchange());
  symbol2.setSymbol(symbol.symbol());

  int loop = 0;
  for (; loop < symbol.count(); loop++)
  {
    Bar *b = symbol.bar(loop);
    if (! b)
      continue;

    if (b->date() >= splitDate)
      continue;

    Bar *b2 = new Bar;
    b2->setDates(b->date(), b->date());
    b2->setOpen(b->open() / divisor);
    b2->setHigh(b->high() / divisor);
    b2->setLow(b->low() / divisor);
    b2->setClose(b->close() / divisor);
    b2->setVolume(b->volume() * divisor);
    symbol2.append(b2);
  }

  db.transaction();
  if (db.setBars(&symbol2))
  {
    qDebug() << _plugin << "::command: QuoteDataBase error";
    return 1;
  }
  db.commit();
  
  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  STOCK_SPLIT *o = new STOCK_SPLIT;
  return ((Plugin *) o);
}
