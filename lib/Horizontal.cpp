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

#include "Horizontal.h"
#include "Utils.h"

#include <QFontMetrics>
#include <QDebug>

Horizontal::Horizontal ()
{
}

void Horizontal::draw (PlotLine *line, PlotData &pd)
{
  QPainter painter;
  painter.begin(&pd.buffer);

  QColor color;
  double d = line->getData(line->count() - 1, color);
  painter.setPen(color);
  
  int y = pd.scaler.convertToY(d);

  Utils util;
  QString s, s2;
  util.strip(d, 4, s);
  line->getLabel(s2);
  s = s2 + s;

  painter.setFont(pd.plotFont);

  QFontMetrics fm = painter.fontMetrics();
  QRect rc = painter.boundingRect(pd.startX, y - (fm.height() / 2), 1, 1, 0, s);
  painter.fillRect(rc, pd.backgroundColor); // fill in behind text first
  painter.drawText(rc, s);
  painter.drawRect(rc);
  
  painter.drawLine (rc.x() + rc.width(), y, pd.buffer.width(), y);

  painter.end();
}

