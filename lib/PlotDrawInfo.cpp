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

#include "PlotDrawInfo.h"
#include "PlotLine.h"
#include "Strip.h"

#include <QPainter>
#include <QtDebug>
#include <QFontMetrics>
#include <QString>
#include <QList>

PlotDrawInfo::PlotDrawInfo ()
{
}

void PlotDrawInfo::draw (PlotData &pd, Indicator &indicator, DateBar &dateBars)
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
  Strip strip;
  for (loop = 0; loop < plotList.count(); loop++)
  {
    PlotLine *line = plotList.at(loop);
      
    if (! line->count())
      continue;
      
    if (line->type() == "Horizontal")
      continue;
      
    PlotLineBar *bar = line->data(pd.infoIndex);
    if (! bar)
      continue;
	
    s = line->label() + "=";
    QString str;
    strip.strip(bar->data(), 4, str);
    s.append(str);
    s.append(" ");

    painter.setPen(bar->color());
    painter.drawText(pos, 10, s);
    pos = pos + fm.width(s);
  }

  painter.end();
}

