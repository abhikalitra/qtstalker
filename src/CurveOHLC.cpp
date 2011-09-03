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
#include "CurveBar.h"
#include "CurveData.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_scale_div.h>
#include <QDebug>

CurveOHLC::CurveOHLC ()
{
}

void CurveOHLC::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  QwtScaleDiv *sd = plot()->axisScaleDiv(QwtPlot::xBottom);

  int loop = sd->lowerBound();
  int size = sd->upperBound();

  if (_settings->get(CurveData::_STYLE) == "OHLC")
  {
    for (; loop < size; loop++)
    {
      Data *b = _settings->getData(loop);
      if (! b)
        continue;

      painter->setPen(QColor(b->get(CurveBar::_COLOR).toString()));

      int x = xMap.transform(loop);
      int yo = yMap.transform(b->get(CurveBar::_OPEN).toDouble());
      int yh = yMap.transform(b->get(CurveBar::_HIGH).toDouble());
      int yl = yMap.transform(b->get(CurveBar::_LOW).toDouble());
      int yc = yMap.transform(b->get(CurveBar::_CLOSE).toDouble());
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
  else
  {
    bool ff = FALSE;
    for (; loop < size; loop++)
    {
      Data *b = _settings->getData(loop);
      if (! b)
        continue;

      ff = FALSE;
      if (b->get(CurveBar::_CLOSE).toDouble() < b->get(CurveBar::_OPEN).toDouble())
        ff = TRUE;

      painter->setPen(QColor(b->get(CurveBar::_COLOR).toString()));

      int x = xMap.transform(loop);
      int xo = yMap.transform(b->get(CurveBar::_OPEN).toDouble());
      int xh = yMap.transform(b->get(CurveBar::_HIGH).toDouble());
      int xl = yMap.transform(b->get(CurveBar::_LOW).toDouble());
      int xc = yMap.transform(b->get(CurveBar::_CLOSE).toDouble());

      int width = xMap.transform(loop + 1) - x;

      if (! ff)
      {
        // empty candle c > o
        QRect rect(QPoint(x + 2, xc), QPoint(x + width - 2, xo));
        painter->drawLine (rect.center().x(), xh, rect.center().x(), xl);
        painter->setBrush(plot()->canvasBackground());
        painter->setPen(QColor(b->get(CurveBar::_COLOR).toString()));
        painter->drawRect(rect);
      }
      else
      {
        // filled candle c < o
        QRect rect(QPoint(x + 2, xo), QPoint(x + width - 2, xc));
        painter->drawLine (rect.center().x(), xh, rect.center().x(), xl);
        painter->setBrush(QColor(b->get(CurveBar::_COLOR).toString()));
        painter->drawRect(rect);
      }
    }
  }
}

int CurveOHLC::info (int index, Message *data)
{
  Data *b = _settings->getData(index);
  if (! b)
    return 1;

  Strip strip;

  QString d;
  strip.strip(b->get(CurveBar::_OPEN).toDouble(), 4, d);
  data->insert("O", d);

  strip.strip(b->get(CurveBar::_HIGH).toDouble(), 4, d);
  data->insert("H", d);

  strip.strip(b->get(CurveBar::_LOW).toDouble(), 4, d);
  data->insert("L", d);

  strip.strip(b->get(CurveBar::_CLOSE).toDouble(), 4, d);
  data->insert("C", d);

  return 0;
}

int CurveOHLC::scalePoint (int i, QColor &color, double &v)
{
  Data *bar = _settings->getData(i);
  if (! bar)
    return 1;

  color.setNamedColor(bar->get(CurveBar::_COLOR).toString());
  v = bar->get(CurveBar::_CLOSE).toDouble();

  return 0;
}
