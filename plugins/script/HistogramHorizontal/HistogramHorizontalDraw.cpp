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

#include "HistogramHorizontalDraw.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_scale_div.h>
#include <QDebug>

HistogramHorizontalDraw::HistogramHorizontalDraw (const QwtText &title) : QwtPlotCurve (title)
{
  _line = 0;
  init();
}

HistogramHorizontalDraw::HistogramHorizontalDraw (const QString &title) : QwtPlotCurve (QwtText(title))
{
  _line = 0;
  init();
}

HistogramHorizontalDraw::~HistogramHorizontalDraw ()
{
  if (_line)
    delete _line;
}

void HistogramHorizontalDraw::init ()
{
  if (_line)
    delete _line;
  _line = 0;

  _high = 0;
  _low = 0;
  _totalHigh = 0;
  
  setItemAttribute(QwtPlotItem::AutoScale, TRUE);
  setItemAttribute(QwtPlotItem::Legend, TRUE);

  setZ(-1);
}

void HistogramHorizontalDraw::setData (Curve *curve)
{
  init();

  _line = new Curve;
  curve->copy(_line);

  int flag = 0;
  int loop = 0;
  for (; loop < curve->count(); loop++)
  {
    CurveBar *b = curve->bar(loop);

    if (! flag)
    {
      _high = b->data(0);
      _low = b->data(1);
      flag++;
    }
    else
    {
      if (b->data(0) > _high)
        _high = b->data(0);
      if (b->data(1) < _low)
        _low = b->data(1);
    }

    double vol = b->data(2) + b->data(3);
    if (vol > _totalHigh)
      _totalHigh = vol;
  }

  itemChanged();
}

QwtDoubleRect HistogramHorizontalDraw::boundingRect () const
{
  QwtDoubleRect rect(0, 0, 0, 0);
  rect.setBottom(_low);
  rect.setTop(_high);

  return rect;
}

int HistogramHorizontalDraw::rtti () const
{
  return QwtPlotCurve::UserCurve;
}

void HistogramHorizontalDraw::draw(QPainter *painter, const QwtScaleMap &, const QwtScaleMap &yMap, const QRect &) const
{
//  QwtScaleDiv *sd = plot()->axisScaleDiv(QwtPlot::xBottom);

  int width = plot()->width() / 2;
  double scaler = _totalHigh / width;

  QRect prect;
  int loop = 0;
  for (; loop < _line->count(); loop += 2)
  {
    CurveBar *b = _line->bar(loop);
    
    QColor color(b->color());
    color.setAlpha(64);

    int y = yMap.transform(b->data(0)); // high
    int y2 = yMap.transform(b->data(1)); // low
    QRect rect;
    rect.setTop(y);
    rect.setBottom(y2);

    // draw a nice seperator between the blocks
    if (rect.bottom() <= prect.top())
      rect.setBottom(prect.top() - 2);
    
    // draw first block
    int x = 0;
    int x2 = b->data(2) / scaler;
    rect.setLeft(x);
    rect.setRight(x2);
    painter->fillRect(rect, color);

    // draw second block
    b = _line->bar(loop + 1);
    
    color = QColor(b->color());
    color.setAlpha(64);
    
    x = x2;
    x2 = (b->data(2) + b->data(3)) / scaler;
    rect.setLeft(x);
    rect.setRight(x2);
    painter->fillRect(rect, color);

    prect = rect;
  }
}

int HistogramHorizontalDraw::highLow (double &h, double &l)
{
  if (! _line)
    return 1;

  h = _high;
  l = _low;

  return 0;
}
