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


#include "ChartObjectRetracementDraw.h"
#include "DateScaleDraw.h"

#include <QDebug>
#include <qwt_plot.h>

ChartObjectRetracementDraw::ChartObjectRetracementDraw ()
{
}

void ChartObjectRetracementDraw::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings->dateTime("Date")));
  if (x == -1)
    return;

  QDateTime dt = _settings->dateTime("Date2");
  if (_settings->getInt("Extend"))
    dsd->date(dsd->count() - 1, dt);

  int x2 = xMap.transform(dsd->x(dt));
  if (x2 == -1)
    return;

  p->setPen(_settings->color("Color"));

  QPolygon array;
  
  _selectionArea.clear();

  QList<double> lineList;
  lineList.append(_settings->getDouble("Line1"));
  lineList.append(_settings->getDouble("Line2"));
  lineList.append(_settings->getDouble("Line3"));
  lineList.append(_settings->getDouble("Line4"));
  lineList.append(_settings->getDouble("Line5"));
  lineList.append(_settings->getDouble("Line6"));

  int loop;
  for (loop = 0; loop < lineList.count(); loop++)
  {
    double td = lineList[loop];
    if (td != 0)
    {
      double range = _settings->getDouble("High") - _settings->getDouble("Low");
      double r = 0;
      if (td < 0)
        r = _settings->getDouble("Low") + (range * td);
      else
      {
        if (td > 0)
          r = _settings->getDouble("Low") + (range * td);
        else
        {
          if (td < 0)
            r = _settings->getDouble("High");
          else
            r = _settings->getDouble("Low");
        }
      }
      
      int y = yMap.transform(r);
      p->drawLine (x, y, x2, y);
      p->drawText(x, y - 1, QString::number(td * 100) + "% - " + QString::number(r));

      array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
      _selectionArea.append(QRegion(array));
    }
  }
  
  // draw the low line
  int y = yMap.transform(_settings->getDouble("Low"));
  p->drawLine (x, y, x2, y);
  p->drawText(x, y - 1, "0% - " + _settings->data("Low"));

  // store the selectable area the low line occupies
  array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
  _selectionArea.append(QRegion(array));

  // draw the high line
  int y2 = yMap.transform(_settings->getDouble("High"));
  p->drawLine (x, y2, x2, y2);
  p->drawText(x, y2 - 1, "100% - " + _settings->data("High"));

  // store the selectable area the high line occupies
  array.putPoints(0, 4, x, y2 - 4, x, y2 + 4, x2, y2 + 4, x2, y2 - 4);
  _selectionArea.append(QRegion(array));

  if (_selected)
  {
    _grabHandles.clear();

    //top left corner
    _grabHandles.append(QRegion(x, y2 - (_handleWidth / 2),
		                _handleWidth,
		                _handleWidth,
		                QRegion::Rectangle));

    p->fillRect(x,
		y2 - (_handleWidth / 2),
		_handleWidth,
		_handleWidth,
		_settings->color("Color"));

    //bottom right corner
    _grabHandles.append(QRegion(x2, y - (_handleWidth / 2),
		                _handleWidth,
		                _handleWidth,
		                QRegion::Rectangle));

    p->fillRect(x2,
		y - (_handleWidth / 2),
		_handleWidth,
		_handleWidth,
		_settings->color("Color"));
  }
}
