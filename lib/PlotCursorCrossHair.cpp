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

#include "PlotCursorCrossHair.h"
#include "PlotCursorInfo.h"

PlotCursorCrossHair::PlotCursorCrossHair ()
{
  _type = "CrossHair";
}

int PlotCursorCrossHair::getCursor ()
{
  return (int) Qt::CrossCursor;
}

void PlotCursorCrossHair::draw (QPainter &painter, PlotData &pd, DateBar &, Scaler &scaler)
{
  int y = scaler.convertToY(pd.y1);
  painter.setPen(QPen(pd.borderColor, 1, Qt::DotLine));
  painter.drawLine (0, y, pd.buffer.width(), y);
  painter.drawLine (pd.x, 0, pd.x, pd.buffer.height());
}

void PlotCursorCrossHair::mouseMove (PlotData &pd, DateBar &dateBars, Scaler &scaler, Indicator &)
{
  // force an update so we can draw overtop the widget and not the buffer
//  emit signalUpdate();  
  pd.plot->update();

  PlotCursorInfo info;
  Setting *mess = info.infoXY(pd.infoIndex, pd.y, dateBars, scaler);
  if (mess)
    emit signalInfoMessage(mess);
}

