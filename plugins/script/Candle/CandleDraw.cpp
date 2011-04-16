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
#include "Strip.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_scale_div.h>
#include <QDebug>

CandleDraw::CandleDraw (const QwtText &title) : QwtPlotCurve (title)
{
  _line = 0;
  init();
}

CandleDraw::CandleDraw (const QString &title) : QwtPlotCurve (QwtText(title))
{
  _line = 0;
  init();
}

CandleDraw::~CandleDraw ()
{
  if (_line)
    delete _line;
}

void CandleDraw::init ()
{
  if (_line)
    delete _line;
  _line = 0;

  _high = 0;
  _low = 0;
  
  setItemAttribute(QwtPlotItem::AutoScale, TRUE);
  setItemAttribute(QwtPlotItem::Legend, TRUE);

  setZ(-1);
}

void CandleDraw::setData (Curve *curve)
{
  init();

  _line = new Curve;
  curve->copy(_line);
  
  curve->highLow(_high, _low);

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
  if (size > _line->count())
    size = _line->count();
  
  bool ff = FALSE;
  for (; loop < size; loop++)
  {
    CurveBar *b = _line->bar(loop);
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

int CandleDraw::highLow (double &h, double &l)
{
  if (! _line)
    return 1;

  h = _high;
  l = _low;
  
  return 0;
}

int CandleDraw::highLowRange (int start, int end, double &h, double &l)
{
  if (! _line)
    return 1;
  
  int rc = 1;
  int loop = start;
  int flag = 0;
  for (; loop < end; loop++)
  {
    CurveBar *r = _line->bar(loop);
    if (! r)
      continue;

    double th, tl;
    if (r->highLow(th, tl))
      continue;

    rc = 0;

    if (! flag)
    {
      h = th;
      l = tl;
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

int CandleDraw::info (int index, Setting *data)
{
  if (! _line)
    return 1;
  
  CurveBar *b = _line->bar(index);
  if (! b)
    return 1;

  Strip strip;

  QString d;
  strip.strip(b->data(0), 4, d);
  data->setData("O", d);

  strip.strip(b->data(1), 4, d);
  data->setData("H", d);

  strip.strip(b->data(2), 4, d);
  data->setData("L", d);

  strip.strip(b->data(3), 4, d);
  data->setData("C", d);

  return 0;
}
