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

#include "CurveOHLC.h"
#include "Strip.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_scale_div.h>
#include <QDebug>

CurveOHLC::CurveOHLC ()
{
  _type = "OHLC";
}

void CurveOHLC::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  QwtScaleDiv *sd = plot()->axisScaleDiv(QwtPlot::xBottom);

  int loop = sd->lowerBound();
  int size = sd->upperBound();

  for (; loop < size; loop++)
  {
    CurveBar *b = _data.value(loop);
    if (! b)
      continue;

    painter->setPen(b->color());

    int x = xMap.transform(loop);
    int yo = yMap.transform(b->data(0));
    int yh = yMap.transform(b->data(1));
    int yl = yMap.transform(b->data(2));
    int yc = yMap.transform(b->data(3));
    int width = xMap.transform(loop + 1) - x;

    QRect rect(QPoint(x + 1, yh), QPoint(x + width - 1, yl));

    // draw the high/low line
    painter->drawLine (rect.center().x(), yh, rect.center().x(), yl);

    // draw the open tick
    painter->drawLine (rect.left(), yo, rect.center().x(), yo);

    // draw the close tick
    painter->drawLine (rect.right(), yc, rect.center().x(), yc);
  }
}

int CurveOHLC::info (int index, Message *data)
{
  if (! _data.count())
    return 1;

  CurveBar *b = _data.value(index);
  if (! b)
    return 1;

  Strip strip;

  QString d;
  strip.strip(b->data(0), 4, d);
  data->insert("O", d);

  strip.strip(b->data(1), 4, d);
  data->insert("H", d);

  strip.strip(b->data(2), 4, d);
  data->insert("L", d);

  strip.strip(b->data(3), 4, d);
  data->insert("C", d);

  return 0;
}
