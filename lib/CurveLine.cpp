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

#include "CurveLine.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_scale_div.h>
#include <QDebug>

CurveLine::CurveLine (int style)
{
  _type = "Line";
  _lineStyle = style;
}

void CurveLine::draw (QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  QwtScaleDiv *sd = plot()->axisScaleDiv(QwtPlot::xBottom);

  int loop = sd->lowerBound();
  int size = sd->upperBound();

//  painter->setRenderHint(QPainter::Antialiasing, TRUE);

  for (; loop < size; loop++)
  {
    CurveBar *yb = _data.value(loop - 1);
    if (! yb)
      continue;

    CurveBar *b = _data.value(loop);
    if (! b)
      continue;

    int x = xMap.transform(loop - 1);
    int x2 = xMap.transform(loop);

    int y = yMap.transform(yb->data());
    int y2 = yMap.transform(b->data());

    QPen tpen = painter->pen();
    tpen.setStyle((Qt::PenStyle) _lineStyle);
    tpen.setWidth(_penWidth);
    tpen.setColor(b->color());
    painter->setPen(tpen);

    painter->drawLine (x, y, x2, y2);
  }
}
