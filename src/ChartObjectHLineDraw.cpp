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


#include "ChartObjectHLineDraw.h"
#include "DateScaleDraw.h"

#include <QDebug>
#include <QPolygon>
#include <qwt_plot.h>

ChartObjectHLineDraw::ChartObjectHLineDraw ()
{
}

void ChartObjectHLineDraw::draw (QPainter *p, const QwtScaleMap &, const QwtScaleMap &yMap, const QRect &) const
{
  p->setPen(_settings->color("COLOR"));

  int y = yMap.transform(_settings->getDouble("PRICE"));

  // test start
  QString s = " " + _settings->data("PRICE"); // prepend space so we can clearly read text
  QFontMetrics fm = p->fontMetrics();
  QRect rc = p->boundingRect(0, y - (fm.height() / 2), 1, 1, 0, s);
  p->fillRect(rc, plot()->canvasBackground()); // fill in behind text first
  p->drawText(rc, s); // draw text
  p->drawRect(rc); // draw a nice little box around text

  p->drawLine (rc.width(), y, p->window().width(), y);

  _selectionArea.clear();

  QPolygon array;
  array.putPoints(0,
                  4,
                  0,
                  y - 4,
                  0,
                  y + 4,
                  p->window().width(),
                  y + 4,
                  p->window().width(),
                  y - 4);
  
  _selectionArea.append(QRegion(array));

  if (_selected)
  {
    _grabHandles.clear();
    
    int t = (int) p->window().width() / 4;
    int loop;
    for (loop = 0; loop < 5; loop++)
    {
      _grabHandles.append(QRegion(t * loop,
                                  y - (_handleWidth / 2),
                                  _handleWidth,
                                  _handleWidth,
                                  QRegion::Rectangle));

      p->fillRect(t * loop,
                  y - (_handleWidth / 2),
                  _handleWidth,
                  _handleWidth,
                  _settings->color("COLOR"));
    }
  }
}
