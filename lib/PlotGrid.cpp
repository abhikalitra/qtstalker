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

#include "PlotGrid.h"

#include <QPainter>
#include <QtDebug>

PlotGrid::PlotGrid ()
{
  gridColor.setNamedColor("#626262");
  gridFlag = TRUE;
}

void PlotGrid::draw (QPixmap &buffer, int startX, int startIndex, int pixelspace, Scaler &scaler)
{
  if (gridFlag == FALSE)
    return;

  drawXGrid(buffer, startX, startIndex, pixelspace);
  drawYGrid(buffer, scaler, startX);
}

void PlotGrid::setGridColor (QColor &d)
{
  gridColor = d;
}

void PlotGrid::setGridFlag (int d)
{
  gridFlag = d;
}

void PlotGrid::setXGrid (QVector<int> &d)
{
  xGrid = d;
}

void PlotGrid::drawXGrid (QPixmap &buffer, int startX, int startIndex, int pixelspace)
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setPen(QPen(gridColor, 1, Qt::DotLine));

  int loop;
  for (loop = 0; loop < (int) xGrid.size(); loop++)
  {
    if (xGrid[loop] >= startIndex)
    {
      int x = startX + (xGrid[loop] * pixelspace) - (startIndex * pixelspace);
      painter.drawLine (x, 0, x, buffer.height());
    }
  }

  painter.end();
}

void PlotGrid::drawYGrid (QPixmap &buffer, Scaler &scaler, int startX)
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setPen(QPen(gridColor, 1, Qt::DotLine));

  QVector<double> scaleArray;
  scaler.getScaleArray(scaleArray);

  int loop;
  for (loop = 0; loop < (int) scaleArray.size(); loop++)
  {
    int y = scaler.convertToY(scaleArray[loop]);
    painter.drawLine (startX, y, buffer.width(), y);
  }

  painter.end();
}

