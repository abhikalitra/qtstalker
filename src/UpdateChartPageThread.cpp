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


#include "UpdateChartPageThread.h"
#include "QuoteServerRequest.h"

#include <QDebug>
#include <QStringList>

UpdateChartPageThread::UpdateChartPageThread (QObject *p, QString &exchange, QString &symbol) : QThread(p)
{
  _searchExchange = exchange;
  _searchSymbol = symbol;
}

void UpdateChartPageThread::run()
{
  QStringList tl;
  tl << "Search" << "S" << _searchExchange << _searchSymbol;
  QString _request = tl.join(",") + "\n";
  
  QuoteServerRequest qsr;
  if (qsr.run(_request))
    return;

  QStringList l = qsr.data().split(":");

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QStringList l2 = l.at(loop).split(",");
    if (l2.count() != 2)
      continue;

    BarData bd;
    bd.setExchange(l2[0]);
    bd.setSymbol(l2[1]);

    tl.clear();
    tl << "Details" << "G" << bd.getExchange() << bd.getSymbol() << "Name";
    _request = tl.join(",") + "\n";

    if (qsr.run(_request))
    {
      emit signalSymbol(bd);
      continue;
    }

    bd.setName(qsr.data());
  
    emit signalSymbol(bd);
  }

  emit signalDone();
}

