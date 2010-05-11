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

#include "PlotInfo.h"
#include "Setting.h"
#include "PlotLine.h"
#include "Utils.h"

#include <QPainter>
#include <QtDebug>
#include <QHash>
#include <QHashIterator>
#include <QList>
#include <cmath>

PlotInfo::PlotInfo ()
{
}

Setting * PlotInfo::getInfo (QPoint &p, PlotData &pd, Indicator &indicator, DateBar &dateBars)
{
  Setting *r = new Setting;

  // determine if we are over a chart object, if so we display parms in the data panel
  QHash<QString, COPlugin *> coList;
  indicator.getChartObjects(coList);
  QHashIterator<QString, COPlugin *> it(coList);
  while (it.hasNext())
  {
    it.next();
    COPlugin *co = it.value();
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
  
  QList<PlotLine *> plotList;
  indicator.getLines(plotList);

  int loop;
  for (loop = 0; loop < plotList.count(); loop++)
  {
    PlotLine *line = plotList.at(loop);
    line->getPlugin(s);

    if (s == "Horizontal")
      continue;
      
    int li = line->count() - dateBars.count() + pd.infoIndex;
    if (li > -1 && li < line->count())
      line->getInfo(li, r);
  }

  if (! r->count())
  {
    delete r;
    r = 0;
  }
  
  return r;
}

Setting * PlotInfo::getCOInfo (COPlugin *co)
{
  Setting *tr = new Setting;
  QString s;
  co->getInfo(tr);

  if (! tr->count())
  {
    delete tr;
    tr = 0;
  }
  
  return tr;
}

void PlotInfo::drawInfo (PlotData &pd, Indicator &indicator, DateBar &dateBars)
{
  QPainter painter;
  painter.begin(&pd.buffer);
  painter.setPen(pd.borderColor);
  painter.setFont(pd.plotFont);
  painter.setBackgroundMode(Qt::OpaqueMode);
  painter.setBackground(QBrush(pd.backgroundColor));

  QFontMetrics fm(pd.plotFont);
  int pos = 0;

  // draw the date
  QString s;
  dateBars.getDateString(pd.infoIndex, s);
  s.append(" ");
  painter.drawText(pos, 10, s);
  pos = pos + fm.width(s);
  
  QList<PlotLine *> plotList;
  indicator.getLines(plotList);
  
  int loop;
  Utils util;
  for (loop = 0; loop < plotList.count(); loop++)
  {
    PlotLine *line = plotList.at(loop);
      
    if (! line->count())
      continue;
      
    QString ts;
    line->getPlugin(ts);
    if (ts == "Horizontal")
      continue;
      
    QColor c;
    int index = line->count() - dateBars.count() + pd.infoIndex;
    if (index < 0 || index >= line->count())
      continue;
    
    double d = line->getData(index, c);
	
    line->getLabel(s);
    s.append("=");
    QString str;
    util.strip(d, 4, str);
    s.append(str);
    s.append(" ");

    painter.setPen(c);
    painter.drawText(pos, 10, s);
    pos = pos + fm.width(s);
  }

  painter.end();
}

void PlotInfo::getPointInfo (PlotData &pd, QList<Setting> &l, Indicator &indicator, DateBar &dateBars)
{
  l.clear();
  
  QList<PlotLine *> plotList;
  indicator.getLines(plotList);

  Utils util;
  int loop;
  for (loop = 0; loop < plotList.count(); loop++)
  {
    PlotLine *line = plotList.at(loop);
    QString s;
    line->getPlugin(s);

    if (s == "Horizontal")
      continue;
      
    int li = line->count() - dateBars.count() + pd.infoIndex;
    if (li > -1 && li < line->count())
    {
      Setting set;
      QColor color;
      double v = line->getData(li, color);

      s = color.name();
      set.setData(0, s);

      util.strip(v, 4, s);
      set.setData(1, s);
      
      l.append(set);
    }
  }
}

Setting * PlotInfo::getCursorInfo (int i, int y, DateBar &dateBars, Scaler &scaler)
{
  Setting *set = new Setting;
  QString d;
  QString k = "X";
  dateBars.getDateTimeString(i, d);
  set->setData(k, d);
  
  Utils util;
  util.strip(scaler.convertToVal(y), 4, d);
  k = "Y";
  set->setData(k, d);
  
  return set;
}

