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

#include "SCChartObject.h"
#include "COFactory.h"
#include "COPlugin.h"

#include <QtDebug>

SCChartObject::SCChartObject ()
{
//  _methodList << "NEW";
}

int SCChartObject::calculate (QStringList &l, QByteArray &ba, Indicator &ind, BarData *data)
{
  // CO,<TYPE>,*
  // 0    1
  
  int rc = 1;
  ba.clear();
  ba.append("ERROR\n");

  if (l.count() < 2)
  {
    qDebug() << "SCChartObject::calculate: invalid parm count" << l.count();
    return rc;
  }

  COFactory fac;
  COPlugin *co = fac.getCO(l[1]);
  if (! co)
  {
    qDebug() << "SCChartObject::calculate: invalid type" << l.count();
    return rc;
  }

  if (co->CUS(l))
  {
    delete co;
    return rc;
  }

  if (! data)
  {
    qDebug() << "SCChartObject::calculate: no bars available";
    return rc;
  }

  co->setSymbol(data->getSymbol());
  co->setExchange(data->getExchange());
  co->setIndicator(ind.name());

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
  co->setID(id);

  ind.addChartObject(co);
  
  ba.clear();
  ba.append("0\n");

  return 0;
}

