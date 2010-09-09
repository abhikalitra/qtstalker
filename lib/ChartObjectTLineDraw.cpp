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


#include "ChartObjectTLineDraw.h"
#include "DateScaleDraw.h"

#include <QDebug>
#include <qwt_plot.h>

ChartObjectTLineDraw::ChartObjectTLineDraw ()
{
  _fieldList << QObject::tr("Open") << QObject::tr("High") << QObject::tr("Low") << QObject::tr("Close");
}

void ChartObjectTLineDraw::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings.date));
  if (x == -1)
    return;

  int x2 = xMap.transform(dsd->x(_settings.date2));
  if (x2 == -1)
    return;

  int y = yMap.transform(_settings.price);
  int y2 = yMap.transform(_settings.price2);

  p->setPen(_settings.color);

  p->drawLine (x, y, x2, y2);

  // save old values;
  int tx2 = x2;
  int ty2 = y2;
  int tx = x;
  int ty = y;

  if (_settings.extend)
  {
    int ydiff = y - y2;
    int xdiff = x2 - x;
    if (xdiff > 0)
    {
      while (x2 < p->window().width())
      {
        x = x2;
        y = y2;
        x2 = x2 + xdiff;
        y2 = y2 - ydiff;
        p->drawLine (x, y, x2, y2);
      }
    }
  }

  // store the selectable area the line occupies
  _selectionArea.clear();
  
  QPolygon array;
//  array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y2 + 4, x2, y2 - 4);
  array.putPoints(0, 4, tx, ty - 4, tx, ty + 4, x2, y2 + 4, x2, y2 - 4);
  
  _selectionArea.append(QRegion(array));

  if (_selected)
  {
    _grabHandles.clear();

    _grabHandles.append(QRegion(tx,
		                ty - (_handleWidth / 2),
		                _handleWidth,
		                _handleWidth,
		                QRegion::Rectangle));

    p->fillRect(tx,
		ty - (_handleWidth / 2),
		_handleWidth,
		_handleWidth,
		_settings.color);

    _grabHandles.append(QRegion(tx2,
		                ty2 - (_handleWidth / 2),
		                _handleWidth,
		                _handleWidth,
		                QRegion::Rectangle));

    p->fillRect(tx2,
		ty2 - (_handleWidth / 2),
		_handleWidth,
		_handleWidth,
		_settings.color);
  }
}

