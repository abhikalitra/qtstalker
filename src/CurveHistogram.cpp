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
#include "CurveData.h"
#include "CurveBar.h"
#include "Strip.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_scale_div.h>
#include <QDebug>

CurveHistogram::CurveHistogram ()
{
  _list << "Histogram" << "Histogram Bar";
}

void CurveHistogram::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  QwtScaleDiv *sd = plot()->axisScaleDiv(QwtPlot::xBottom);

  int loop = sd->lowerBound();
  int size = sd->upperBound();

//  int zero = yMap.transform(0);

  if (_settings->get(CurveData::_STYLE)->toString() == "Histogram")
  {
    for (; loop < size; loop++)
    {
      Data *yb = _settings->getData(loop - 1);
      if (! yb)
        continue;

      Data *b = _settings->getData(loop);
      if (! b)
        continue;

      QPolygon poly;

      // bottom left
      int x = xMap.transform(loop - 1);
      int y = yMap.transform(yb->get(CurveBar::_LOW)->toDouble());
      poly << QPoint(x, y);

      // top left
      y = yMap.transform(yb->get(CurveBar::_HIGH)->toDouble());
      poly << QPoint(x, y);

      // top right
      x = xMap.transform(loop);
      y = yMap.transform(b->get(CurveBar::_HIGH)->toDouble());
      poly << QPoint(x, y);

      // bottom right
      y = yMap.transform(b->get(CurveBar::_LOW)->toDouble());
      poly << QPoint(x, y);

      painter->setBrush(b->get(CurveBar::_COLOR)->toColor());
      painter->drawPolygon(poly, Qt::OddEvenFill);
    }

/*
    QPolygon poly;
    int flag = 0;
    int x = 0;
    int width = 0;
    for (; loop < size; loop++)
    {
      Data *b = _settings->getData(loop);
      if (! b)
        continue;

      x = xMap.transform(loop);
      int zero = yMap.transform(b->getDouble(CurveBar::_LOW));
      int y = yMap.transform(b->getDouble(CurveBar::_HIGH));
      width = xMap.transform(loop + 1) - x;

      if (! flag)
      {
        poly << QPoint(x, zero) << QPoint(x, y);
        flag++;
      }

      poly << QPoint(x + width, y);

      painter->setBrush(b->getColor(CurveBar::_COLOR));
    }

//    poly << QPoint(x + width, zero);

    painter->drawPolygon(poly, Qt::OddEvenFill);
*/
  }
  else
  {
    // histogram bar
    for (; loop < size; loop++)
    {
      Data *b = _settings->getData(loop);
      if (! b)
        continue;

      if (b->get(CurveBar::_LOW)->toDouble() > b->get(CurveBar::_HIGH)->toDouble())
      {
        QPolygon poly;

        // bottom left
        int x = xMap.transform(loop);
        int y = yMap.transform(b->get(CurveBar::_HIGH)->toDouble());
        poly << QPoint(x, y);

        // top left
        y = yMap.transform(b->get(CurveBar::_LOW)->toDouble());
        poly << QPoint(x, y);

        int width = xMap.transform(loop + 1) - x;

        // top right
        x += width;
        y = yMap.transform(b->get(CurveBar::_LOW)->toDouble());
        poly << QPoint(x, y);

        // bottom right
        y = yMap.transform(b->get(CurveBar::_HIGH)->toDouble());
        poly << QPoint(x, y);

        painter->setBrush(b->get(CurveBar::_COLOR)->toColor());
        painter->drawPolygon(poly, Qt::OddEvenFill);
      }
      else
      {
        QPolygon poly;

        // bottom left
        int x = xMap.transform(loop);
        int y = yMap.transform(b->get(CurveBar::_LOW)->toDouble());
        poly << QPoint(x, y);

        // top left
        y = yMap.transform(b->get(CurveBar::_HIGH)->toDouble());
        poly << QPoint(x, y);

        int width = xMap.transform(loop + 1) - x;

        // top right
        x += width;
        y = yMap.transform(b->get(CurveBar::_HIGH)->toDouble());
        poly << QPoint(x, y);

        // bottom right
        y = yMap.transform(b->get(CurveBar::_LOW)->toDouble());
        poly << QPoint(x, y);

        painter->setBrush(b->get(CurveBar::_COLOR)->toColor());
        painter->drawPolygon(poly, Qt::OddEvenFill);
      }
    }
  }
}

int CurveHistogram::info (int index, Message *data)
{
  Data *b = _settings->getData(index);
  if (! b)
    return 1;

  Strip strip;
  QString s;
  strip.strip(b->get(CurveBar::_HIGH)->toDouble(), 4, s);

  data->insert(_settings->get(CurveData::_LABEL)->toString(), s);

  return 0;
}

int CurveHistogram::scalePoint (int i, QColor &color, double &v)
{
  Data *bar = _settings->getData(i);
  if (! bar)
    return 1;

  color = bar->get(CurveBar::_COLOR)->toColor();
  v = bar->get(CurveBar::_HIGH)->toDouble();

  return 0;
}
