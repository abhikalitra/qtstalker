/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "SYMBOL.h"
#include "BarData.h"
#include "DataBase.h"

#include <QtDebug>


SYMBOL::SYMBOL ()
{
}

int SYMBOL::calculate (QStringList &set, QHash<QString, PlotLine *> &tlines)
{
  // INDICATOR,SYMBOL,<NAME>,<SYMBOL>,<BAR_FIELD>,<BAR_LENGTH>,<BARS>

  if (set.count() != 7)
  {
    qDebug() << "SYMBOL::calculate: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[2]);
  if (tl)
  {
    qDebug() << set[1] << "::calculate: duplicate name" << set[2];
    return 1;
  }

  bool ok;
  int bars = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "SYMBOL::calculate: invalid bars" << set[6];
    return 1;
  }

  DataBase db;
  BarData symbol;
  symbol.setSymbol(set[3]);
  symbol.setBarLength(set[5]);
  symbol.setBarsRequested(bars);
  db.getChart(&symbol);

  BarData::InputType it = symbol.getInputType(set[4]);
  PlotLine *line = symbol.getInput(it);
  line->setScaleFlag(TRUE);

/*
  QHash<QString, double> dict;
  int loop;
  QString ts = "Close";
  QString ts2;
  QDateTime dt;
  double val = 0;
  for (loop = 0; loop < (int) symbol->count(); loop++)
  {
    symbol->getDate(loop, dt);
    ts = dt.toString("yyyyMMddHHmmss");
    dict.insert(ts, symbol->getClose(loop));
  }

  PlotLine *line = new PlotLine;
  line->setScaleFlag(TRUE);
  for (loop = 0; loop < (int) data->count(); loop++)
  {
    data->getDate(loop, dt);
    ts = dt.toString("yyyyMMddHHmmss");
    if (dict.contains(ts))
      line->append(dict.value(ts));
  }
*/

  tlines.insert(set[2], line);

  return 0;
}

