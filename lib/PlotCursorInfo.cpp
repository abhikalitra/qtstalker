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

#include "PlotCursorInfo.h"
#include "PlotLine.h"
#include "Strip.h"
#include "COPlugin.h"

#include <QString>
#include <QtDebug>
#include <QHash>
#include <QHashIterator>
#include <QList>

PlotCursorInfo::PlotCursorInfo ()
{
}

Setting * PlotCursorInfo::info (QPoint &p, PlotData &pd, Indicator &indicator, DateBar &dateBars)
{
  Setting *r = new Setting;

  // determine if we are over a chart object, if so we display parms in the data panel
  QList<int> keyList;
  indicator.coKeys(keyList);
  int loop = 0;
  for (; loop < keyList.count(); loop++)
  {
    COPlugin *co = indicator.chartObject(keyList.at(loop));
    if (co->isSelected(p))
    {
      co->getInfo(r);
      if (r->count())
        return r;
    }
  }

  QString s, k;
  dateBars.getDateString(pd.infoIndex, s);
  k = "D";
  r->setData(k, s);
  dateBars.getTimeString(pd.infoIndex, s);
  k = "T";
  r->setData(k, s);
  
  QStringList plotList = indicator.plotOrder();
  for (loop = 0; loop < plotList.count(); loop++)
  {
    QString ts = plotList.at(loop);
    PlotLine *line = indicator.line(ts);
    if (line->type() == "Horizontal")
      continue;
      
    line->info(pd.infoIndex, r);
  }

  if (! r->count())
  {
    delete r;
    r = 0;
  }
  
  return r;
}

Setting * PlotCursorInfo::infoXY (int i, int y, DateBar &dateBars, Scaler &scaler)
{
  Setting *set = new Setting;
  QString d;
  QString k = "X";
  dateBars.getDateTimeString(i, d);
  set->setData(k, d);
  
  Strip strip;
  strip.strip(scaler.convertToVal(y), 4, d);
  k = "Y";
  set->setData(k, d);
  
  return set;
}
