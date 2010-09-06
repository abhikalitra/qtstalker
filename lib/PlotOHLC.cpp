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

#include "PlotOHLC.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <QDebug>

PlotOHLC::PlotOHLC (const QwtText &title) : QwtPlotCurve (title)
{
  init();
}

PlotOHLC::PlotOHLC (const QString &title) : QwtPlotCurve (QwtText(title))
{
  init();
}

PlotOHLC::~PlotOHLC ()
{
}

void PlotOHLC::init ()
{
  _high = 0;
  _low = 0;
  
  setItemAttribute(QwtPlotItem::AutoScale, TRUE);
  setItemAttribute(QwtPlotItem::Legend, TRUE);

  setZ(20.0);
}

void PlotOHLC::setData (Curve *curve)
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

QwtDoubleRect PlotOHLC::boundingRect () const
{
  QwtDoubleRect rect(0, 0, 0, 0);
  rect.setBottom(_low);
  rect.setTop(_high);

  return rect;
}

int PlotOHLC::rtti () const
{
  return QwtPlotCurve::UserCurve;
}

void PlotOHLC::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  int loop = 0;
  for (; loop < _list.count(); loop++)
  {
    OHLC ohlc = _list.at(loop);

    painter->setPen(ohlc.color);

    int x = xMap.transform(loop);

    // draw the open tick
    int y = yMap.transform(ohlc.open);
    painter->drawLine (x, y, x + 2, y);

    // draw the close tick
    y = yMap.transform(ohlc.close);
    painter->drawLine (x + 2, y, x + 4, y);

    // draw the high/low tick
    y = yMap.transform(ohlc.high);
    int y2 = yMap.transform(ohlc.low);
    painter->drawLine (x + 2, y, x + 2, y2);
  }
}

double PlotOHLC::high ()
{
  return _high;
}

double PlotOHLC::low ()
{
  return _low;
}

