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
#include "Config.h"

#include <QPainter>
#include <QtDebug>

PlotGrid::PlotGrid ()
{
  Config config;
  config.getData(Config::GridColor, _gridColor);
  if (! _gridColor.isValid())
    _gridColor = QColor("#626262");

  _gridFlag = TRUE;
  QString s;
  config.getData(Config::Grid, s);
  if (! s.isEmpty())
    _gridFlag = s.toInt();
}

void PlotGrid::setGridColor (QColor &d)
{
  _gridColor = d;
}

void PlotGrid::setGridFlag (int d)
{
  _gridFlag = d;
}

void PlotGrid::setXGrid (QList<int> &d)
{
  _xGrid = d;
}

void PlotGrid::drawXGrid (PlotData &pd)
{
  if (_gridFlag == FALSE)
    return;

  QPainter painter;
  painter.begin(&pd.buffer);
  painter.setPen(QPen(_gridColor, 1, Qt::DotLine));

  int loop;
  for (loop = 0; loop < (int) _xGrid.size(); loop++)
  {
    if (_xGrid.at(loop) >= pd.startIndex)
    {
      int x = (_xGrid.at(loop) * pd.barSpacing) - (pd.startIndex * pd.barSpacing);
      painter.drawLine (x, 0, x, pd.buffer.height() - pd.dateHeight);
    }
  }

  painter.end();
}

void PlotGrid::drawYGrid (PlotData &pd, Scaler &scaler)
{
  if (_gridFlag == FALSE)
    return;

  QPainter painter;
  painter.begin(&pd.buffer);
  painter.setPen(QPen(_gridColor, 1, Qt::DotLine));

  QList<double> scaleArray;
  scaler.scaleArray(scaleArray);

  int loop;
  for (loop = 0; loop < (int) scaleArray.size(); loop++)
  {
    int y = scaler.convertToY(scaleArray.at(loop));
    painter.drawLine (0, y, pd.buffer.width() - pd.scaleWidth, y);
  }

  painter.end();
}

