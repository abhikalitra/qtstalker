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

#include "CandleDraw.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_scale_div.h>
#include <QDebug>

CandleDraw::CandleDraw (const QwtText &title) : QwtPlotCurve (title)
{
  init();
}

CandleDraw::CandleDraw (const QString &title) : QwtPlotCurve (QwtText(title))
{
  init();
}

CandleDraw::~CandleDraw ()
{
  qDeleteAll(_list);
}

void CandleDraw::init ()
{
  _high = 0;
  _low = 0;
  
  setItemAttribute(QwtPlotItem::AutoScale, TRUE);
  setItemAttribute(QwtPlotItem::Legend, TRUE);

  setZ(0.0);
}

void CandleDraw::setData (Curve *curve)
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

QwtDoubleRect CandleDraw::boundingRect () const
{
  QwtDoubleRect rect(0, 0, 0, 0);
  rect.setBottom(_low);
  rect.setTop(_high);

  return rect;
}

int CandleDraw::rtti () const
{
  return QwtPlotCurve::UserCurve;
}

void CandleDraw::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  QwtScaleDiv *sd = plot()->axisScaleDiv(QwtPlot::xBottom); // test

  int loop = sd->lowerBound();
  int size = sd->upperBound();
  if (size > _list.count())
    size = _list.count();
  
  bool ff = FALSE;
  for (; loop < size; loop++)
  {
    CurveBar *b = _list.at(loop);

    ff = FALSE;
    if (b->data(3) < b->data(0))
      ff = TRUE;

    painter->setPen(b->color());

    int x = xMap.transform(loop);

    int xo = yMap.transform(b->data(0));
    int xh = yMap.transform(b->data(1));
    int xl = yMap.transform(b->data(2));
    int xc = yMap.transform(b->data(3));

    if (! ff)
    {
      // empty candle
      painter->drawLine (x + 3, xh, x + 3, xc);
      painter->drawLine (x + 3, xo, x + 3, xl);
      painter->drawRect(x, xc, 6, xo - xc);
    }
    else
    {
      // filled candle
      painter->drawLine (x + 2, xh, x + 2, xl);
      painter->fillRect(x, xo, 5, xc - xo, b->color());
    }
  }
}

double CandleDraw::high ()
{
  return _high;
}

double CandleDraw::low ()
{
  return _low;
}
