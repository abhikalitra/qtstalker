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

#include "CurveHistogram.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_scale_div.h>
#include <QDebug>

CurveHistogram::CurveHistogram (QString m)
{
//  _type = "Histogram";
  _type = m;
  _method = m;
  _list << "Histogram" << "Histogram Bar";
}

void CurveHistogram::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  QwtScaleDiv *sd = plot()->axisScaleDiv(QwtPlot::xBottom);

  int loop = sd->lowerBound();
  int size = sd->upperBound();

  int zero = yMap.transform(0);

  if (_method == "Histogram")
  {
    QPolygon poly;
    int flag = 0;
    int x = 0;
    int width = 0;
    for (; loop < size; loop++)
    {
      CurveBar *b = _data.value(loop);
      if (! b)
        continue;

      x = xMap.transform(loop);
      int y = yMap.transform(b->data());
      width = xMap.transform(loop + 1) - x;

      if (! flag)
      {
        poly << QPoint(x, zero) << QPoint(x, y);
        flag++;
      }

      poly << QPoint(x + width, y);

      painter->setBrush(b->color());
    }

    poly << QPoint(x + width, zero);

    painter->drawPolygon(poly, Qt::OddEvenFill);
  }
  else
  {
    // histogram bar
    for (; loop < size; loop++)
    {
      CurveBar *b = _data.value(loop);
      if (! b)
        continue;

      int x = xMap.transform(loop);
      int y = yMap.transform(b->data());

      int width = xMap.transform(loop + 1) - x;

      QRect rect(QPoint(x + 1, y), QPoint(x + width - 1, zero));

      painter->setBrush(b->color());

      painter->drawRect(rect);
    }
  }
}

int CurveHistogram::highLowRange (int start, int end, double &h, double &l)
{
  if (! _data.count())
    return 1;

  int rc = 1;
  int loop = start;
  int flag = 0;
  for (; loop < end; loop++)
  {
    CurveBar *r = _data.value(loop);
    if (! r)
      continue;

    double th, tl;
    if (r->highLow(th, tl))
      continue;

    rc = 0;

    if (! flag)
    {
      h = th;
      l = 0;
      flag++;
    }
    else
    {
      if (th > h)
        h = th;
      if (tl < l)
        l = tl;
    }
  }

  return rc;
}
