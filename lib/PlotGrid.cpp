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

#include "PlotGrid.h"

#include <QPainter>
#include <QtDebug>

PlotGrid::PlotGrid ()
{
  gridColor.setNamedColor("#626262");
  gridFlag = TRUE;
}

void PlotGrid::draw (PlotData &pd)
{
  if (gridFlag == FALSE)
    return;

  drawXGrid(pd);
  drawYGrid(pd);
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

void PlotGrid::drawXGrid (PlotData &pd)
{
  QPainter painter;
  painter.begin(&pd.buffer);
  painter.setPen(QPen(gridColor, 1, Qt::DotLine));

  int loop;
  for (loop = 0; loop < (int) xGrid.size(); loop++)
  {
    if (xGrid[loop] >= pd.startIndex)
    {
      int x = pd.startX + (xGrid[loop] * pd.pixelspace) - (pd.startIndex * pd.pixelspace);
      painter.drawLine (x, 0, x, pd.buffer.height());
    }
  }

  painter.end();
}

void PlotGrid::drawYGrid (PlotData &pd)
{
  QPainter painter;
  painter.begin(&pd.buffer);
  painter.setPen(QPen(gridColor, 1, Qt::DotLine));

  QVector<double> scaleArray;
  pd.scaler.getScaleArray(scaleArray);

  int loop;
  for (loop = 0; loop < (int) scaleArray.size(); loop++)
  {
    int y = pd.scaler.convertToY(scaleArray[loop]);
    painter.drawLine (pd.startX, y, pd.buffer.width(), y);
  }

  painter.end();
}

