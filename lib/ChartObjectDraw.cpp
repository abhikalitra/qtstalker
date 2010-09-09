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


#include "ChartObjectDraw.h"

#include <QDebug>
#include <qwt_plot.h>

ChartObjectDraw::ChartObjectDraw ()
{
  _settings.id = -1;
  _settings.type = -1;
  _settings.color = QColor(Qt::red);
  _settings.date = QDateTime::currentDateTime();
  _settings.date2 = _settings.date;
  _settings.price = 0;
  _settings.price2 = 0;
  _settings.high = 0;
  _settings.low = 0;
  _settings.extend = 0;
  _settings.line1 = 0;
  _settings.line2 = 0;
  _settings.line3 = 0;
  _settings.line4 = 0;
  _settings.line5 = 0;
  _settings.line6 = 0;

  _selected = 0;
  _handleWidth = 6;

  setYAxis(QwtPlot::yRight);
}

int ChartObjectDraw::rtti () const
{
  return Rtti_PlotUserItem;
}

void ChartObjectDraw::setSettings (ChartObjectSettings &d)
{
  _settings = d;
}

int ChartObjectDraw::isSelected (QPoint p)
{
  int loop;
  for (loop = 0; loop < (int) _selectionArea.count(); loop++)
  {
    QRegion r = _selectionArea.at(loop);
    if (r.contains(p))
      return 1;
  }

  return 0;
}

int ChartObjectDraw::isGrabSelected (QPoint p)
{
  int loop;
  for (loop = 0; loop < (int) _grabHandles.count(); loop++)
  {
    QRegion r = _grabHandles.at(loop);
    if (r.contains(p))
      return loop + 1;
  }

  return 0;
}

void ChartObjectDraw::setSelected (int d)
{
  _selected = d;
}

