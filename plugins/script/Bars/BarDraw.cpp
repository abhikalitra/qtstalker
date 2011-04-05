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

#include "BarDraw.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_scale_div.h>
#include <QDebug>

BarDraw::BarDraw (const QwtText &title) : QwtPlotCurve (title)
{
  init();
}

BarDraw::BarDraw (const QString &title) : QwtPlotCurve (QwtText(title))
{
  init();
}

BarDraw::~BarDraw ()
{
  qDeleteAll(_list);
}

void BarDraw::init ()
{
  _high = 0;
  _low = 0;
  
  setItemAttribute(QwtPlotItem::AutoScale, TRUE);
  setItemAttribute(QwtPlotItem::Legend, TRUE);

  setZ(0.0);
}

void BarDraw::setData (Curve *curve)
{
  qDeleteAll(_list);
  _list.clear();

  QList<int> keys;
  curve->keys(keys);

  _high = -99999999;
  _low = 99999999;

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = curve->bar(keys.at(loop));
    CurveBar *b = new CurveBar;
    b->setData(0, bar->data(0));
    b->setData(1, bar->data(1));
    b->setData(2, bar->data(2));
    b->setData(3, bar->data(3));
    b->setColor(bar->color());
    _list.append(b);

    if (b->data(1) > _high)
      _high = b->data(1);

    if (b->data(2) < _low)
      _low = b->data(2);
  }

  itemChanged();
}

QwtDoubleRect BarDraw::boundingRect () const
{
  QwtDoubleRect rect(0, 0, 0, 0);
  rect.setBottom(_low);
  rect.setTop(_high);

  return rect;
}

int BarDraw::rtti () const
{
  return QwtPlotCurve::UserCurve;
}

void BarDraw::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  QwtScaleDiv *sd = plot()->axisScaleDiv(QwtPlot::xBottom);

  int loop = sd->lowerBound();
  int size = sd->upperBound();
  if (size > _list.count())
    size = _list.count();

  for (; loop < size; loop++)
  {
    CurveBar *b = _list.at(loop);

    painter->setPen(b->color());

    int x = xMap.transform(loop);

    // draw the open tick
    int y = yMap.transform(b->data(0));
    painter->drawLine (x, y, x + 2, y);

    // draw the close tick
    y = yMap.transform(b->data(3));
    painter->drawLine (x + 2, y, x + 4, y);

    // draw the high/low tick
    y = yMap.transform(b->data(1));
    int y2 = yMap.transform(b->data(2));
    painter->drawLine (x + 2, y, x + 2, y2);
  }
}

double BarDraw::high ()
{
  return _high;
}

double BarDraw::low ()
{
  return _low;
}
