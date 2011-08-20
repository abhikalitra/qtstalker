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

#include "CurveCandle.h"
#include "Strip.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_scale_div.h>
#include <QDebug>

CurveCandle::CurveCandle ()
{
  _type = "Candle";
}

void CurveCandle::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  QwtScaleDiv *sd = plot()->axisScaleDiv(QwtPlot::xBottom); // test

  int loop = sd->lowerBound();
  int size = sd->upperBound();

  bool ff = FALSE;
  for (; loop < size; loop++)
  {
    CurveBar *b = _data.value(loop);
    if (! b)
      continue;

    ff = FALSE;
    if (b->data(3) < b->data(0))
      ff = TRUE;

    painter->setPen(b->color());

    int x = xMap.transform(loop);
    int xo = yMap.transform(b->data(0));
    int xh = yMap.transform(b->data(1));
    int xl = yMap.transform(b->data(2));
    int xc = yMap.transform(b->data(3));

    int width = xMap.transform(loop + 1) - x;

    if (! ff)
    {
      // empty candle c > o
      QRect rect(QPoint(x + 2, xc), QPoint(x + width - 2, xo));
      painter->drawLine (rect.center().x(), xh, rect.center().x(), xl);
      painter->setBrush(plot()->canvasBackground());
      painter->setPen(b->color());
      painter->drawRect(rect);
    }
    else
    {
      // filled candle c < o
      QRect rect(QPoint(x + 2, xo), QPoint(x + width - 2, xc));
      painter->drawLine (rect.center().x(), xh, rect.center().x(), xl);
      painter->setBrush(b->color());
      painter->drawRect(rect);
    }
  }
}

int CurveCandle::info (int index, Message *data)
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
