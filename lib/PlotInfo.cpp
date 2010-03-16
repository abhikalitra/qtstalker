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
#include "PluginFactory.h"
#include "Setting.h"
#include "ChartObject.h"
#include "COPlugin.h"
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

Setting * PlotInfo::getInfo (QPoint &p, Indicator &indicator, int index, DateBar &dateBars,
			     QString &coPluginPath)
{
  PluginFactory fac;
  Setting *r = new Setting;

  // determine if we are over a chart object, if so we display parms in the data panel
  QHash<QString, ChartObject *> coList;
  indicator.getChartObjects(coList);
  QHashIterator<QString, ChartObject *> it(coList);
  while (it.hasNext())
  {
    it.next();
    ChartObject *co = it.value();
    if (co->isSelected(p))
    {
      QString s;
      co->getData(ChartObject::ParmPlugin, s);
      COPlugin *plug = fac.getCO(coPluginPath, s);
      plug->getInfo(co, r);
      if (r->count())
        return r;
    }
  }

  QString s, k;
  dateBars.getDateString(index, s);
  k = "D";
  r->setData(k, s);
  dateBars.getTimeString(index, s);
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
      
    int li = line->count() - dateBars.count() + index;
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

Setting * PlotInfo::getCOInfo (ChartObject *co, QString &coPluginPath)
{
  Setting *tr = new Setting;
  PluginFactory fac;
  QString s;
  co->getData(ChartObject::ParmPlugin, s);
  COPlugin *plug = fac.getCO(coPluginPath, s);
  plug->getInfo(co, tr);

  if (! tr->count())
  {
    delete tr;
    tr = 0;
  }
  
  return tr;
}

void PlotInfo::drawInfo (QPixmap &buffer, QColor &borderColor, QColor &backColor, QFont &font, int startX,
			 DateBar &dateBars, Indicator &indicator)
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setPen(borderColor);
  painter.setFont(font);
  painter.setBackgroundMode(Qt::OpaqueMode);
  painter.setBackground(QBrush(backColor));

  QFontMetrics fm(font);
  int pos = startX;

  QString s;
  dateBars.getDateString(dateBars.count() - 1, s);
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
    double d = line->getData(line->count() - 1, c);
	
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

void PlotInfo::getPointInfo (Indicator &indicator, int index, DateBar &dateBars, QList<Setting> &l)
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
      
    int li = line->count() - dateBars.count() + index;
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

