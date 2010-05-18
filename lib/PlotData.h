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

#ifndef PLOT_DATA_HPP
#define PLOT_DATA_HPP

#include <QString>
#include <QFont>
#include <QColor>
#include <QDateTime>
#include <QPixmap>

struct PlotData
{
  QPixmap buffer;
  int barSpacing;
  int startIndex;
  int endIndex;
  int infoIndex; // calculated position for info
  int x; // raw mouse x position from event->x()
  int y; // raw mouse y position from event-y()
  int infoFlag;
  int pos;
  int interval;
  int dateHeight;
  int scaleWidth;
  int barWidth;
  double y1; // scaler y position
  QFont plotFont;
  QColor backgroundColor;
  QColor borderColor;
  QDateTime x1; // dateBars bar position
};

#endif
