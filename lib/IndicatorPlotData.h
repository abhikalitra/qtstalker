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

#ifndef INDICATOR_PLOT_DATA_HPP
#define INDICATOR_PLOT_DATA_HPP

#include <QString>
#include <QPixmap>
#include <QFont>
#include <QColor>

#include "Scaler.h"
#include "Indicator.h"
#include "DateBar.h"

struct indicatorPlotData
{
  Indicator indicator;
  QPixmap buffer;
  int pixelspace;
  int startX;
  int startIndex;
  int scaleToScreen;
  Scaler scaler;
  QString plotPluginPath;
  QString coPluginPath;
  DateBar dateBars;
  QFont plotFont;
  QColor backgroundColor;
  QColor borderColor;
  int infoIndex;
};

#endif
