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

#include "CO.h"
#include "ChartObjectSettings.h"
#include "ChartObjectFactory.h"
#include "ChartObject.h"
#include "Globals.h"

#include <QtDebug>

CO::CO ()
{
}

int CO::command (QStringList &l, Indicator &ind, BarData &, QByteArray &ba)
{
  // CO,<TYPE>,*
  // 0    1
  
  int rc = 1;
  ba.clear();
  ba.append("ERROR\n");

  if (l.count() < 2)
  {
    qDebug() << "CO::command: invalid parm count" << l.count();
    return rc;
  }

  if (! g_barData.count())
  {
    qDebug() << "CO::command: no bars available";
    return rc;
  }

  ChartObjectFactory fac;
  ChartObject *co = fac.chartObject(l[1]);
  if (! co)
  {
    qDebug() << "CO::command: invalid type" << l.count();
    return rc;
  }

  if (co->CUS(l))
  {
    delete co;
    return rc;
  }

  ChartObjectSettings set;
  co->settings(set);
  delete co;
  
  set.symbol = g_barData.getSymbol();
  set.exchange = g_barData.getExchange();
  set.indicator = ind.name();

  // we use < 0 id nums for script chart objects so there are no conflicts
  // with permanent chart object id's that are >= 0
  int id = -1;
  QList<int> ids;
  ind.coKeys(ids);
  if (ids.count())
  {
    qSort(ids.begin(), ids.end());
    id = ids.at(0);
  }
  id--;
  set.id = id;

  ind.addChartObject(id, set);

  ba.clear();
  ba.append("0\n");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  CO *o = new CO;
  return ((ScriptPlugin *) o);
}

