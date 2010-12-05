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

#include "PlotCandle.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_scale_div.h>
#include <QDebug>

PlotCandle::PlotCandle (const QwtText &title) : QwtPlotCurve (title)
{
  init();
}

PlotCandle::PlotCandle (const QString &title) : QwtPlotCurve (QwtText(title))
{
  init();
}

PlotCandle::~PlotCandle ()
{
}

void PlotCandle::init ()
{
  _high = 0;
  _low = 0;
  
  setItemAttribute(QwtPlotItem::AutoScale, TRUE);
  setItemAttribute(QwtPlotItem::Legend, TRUE);

  setZ(0.0);
}

void PlotCandle::setData (Curve *curve)
{
  QList<int> keys;
  curve->keys(keys);

  _list.clear();
  _high = -99999999;
  _low = 99999999;

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = curve->bar(keys.at(loop));
    OHLC ohlc;
    ohlc.open = bar->data(0);
    ohlc.high = bar->data(1);
    ohlc.low = bar->data(2);
    ohlc.close = bar->data(3);
    ohlc.color = bar->color();
    _list.append(ohlc);

    if (ohlc.high > _high)
      _high = ohlc.high;

    if (ohlc.low < _low)
      _low = ohlc.low;
  }

  itemChanged();
}

QwtDoubleRect PlotCandle::boundingRect () const
{
  QwtDoubleRect rect(0, 0, 0, 0);
  rect.setBottom(_low);
  rect.setTop(_high);

  return rect;
}

int PlotCandle::rtti () const
{
  return QwtPlotCurve::UserCurve;
}

void PlotCandle::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  QwtScaleDiv *sd = plot()->axisScaleDiv(QwtPlot::xBottom); // test

  int loop = sd->lowerBound();
  int size = sd->upperBound();
  if (size > _list.count())
    size = _list.count();
  
  bool ff = FALSE;
  for (; loop < size; loop++)
  {
    OHLC ohlc = _list.at(loop);

    ff = FALSE;
    if (ohlc.close < ohlc.open)
      ff = TRUE;

    painter->setPen(ohlc.color);

    int x = xMap.transform(loop);

    int xo = yMap.transform(ohlc.open);
    int xh = yMap.transform(ohlc.high);
    int xl = yMap.transform(ohlc.low);
    int xc = yMap.transform(ohlc.close);

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
      painter->fillRect(x, xo, 5, xc - xo, ohlc.color);
    }
  }
  
/*  
  int loop = 0;
  bool ff = FALSE;
  for (; loop < _list.count(); loop++)
  {
    OHLC ohlc = _list.at(loop);

    ff = FALSE;
    if (ohlc.close < ohlc.open)
      ff = TRUE;

    painter->setPen(ohlc.color);

    int x = xMap.transform(loop);

    int xo = yMap.transform(ohlc.open);
    int xh = yMap.transform(ohlc.high);
    int xl = yMap.transform(ohlc.low);
    int xc = yMap.transform(ohlc.close);

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
      painter->fillRect(x, xo, 5, xc - xo, ohlc.color);
    }
  }
*/
}

double PlotCandle::high ()
{
  return _high;
}

double PlotCandle::low ()
{
  return _low;
}

