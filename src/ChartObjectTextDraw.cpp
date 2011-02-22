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


#include "ChartObjectTextDraw.h"
#include "DateScaleDraw.h"

#include <QDebug>
#include <qwt_plot.h>

ChartObjectTextDraw::ChartObjectTextDraw ()
{
}

void ChartObjectTextDraw::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings->dateTime("Date")));

  int y = yMap.transform(_settings->getDouble("Price"));

  p->setPen(_settings->color("Color"));

  p->setFont(_settings->font("Font"));
  
  p->drawText(x, y, _settings->data("Text"));

  QFontMetrics fm = p->fontMetrics();
  
  _selectionArea.clear();
  
  _selectionArea.append(QRegion(x,
		                y - fm.height(),
		                fm.width(_settings->data("Text"), -1),
		                fm.height(),
		                QRegion::Rectangle));

  if (_selected)
  {
    _grabHandles.clear();

    _grabHandles.append(QRegion(x - _handleWidth - 1,
		                y - (fm.height() / 2),
		                _handleWidth,
		                _handleWidth,
		                QRegion::Rectangle));

    p->fillRect(x - _handleWidth - 1,
		y - (fm.height() / 2),
		_handleWidth,
		_handleWidth,
		_settings->color("Color"));
  }
}
