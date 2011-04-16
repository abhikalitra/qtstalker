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

#include "HistogramDraw.h"
#include "Strip.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_scale_div.h>
#include <QDebug>

HistogramDraw::HistogramDraw (const QwtText &title) : QwtPlotCurve (title)
{
  _line = 0;
  init();
}

HistogramDraw::HistogramDraw (const QString &title) : QwtPlotCurve (QwtText(title))
{
  _line = 0;
  init();
}

HistogramDraw::~HistogramDraw ()
{
  if (_line)
    delete _line;
}

void HistogramDraw::init ()
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

void HistogramDraw::setCurveData (Curve *curve)
{
  init();

  _line = new Curve;
  curve->copy(_line);

  curve->highLow(_high, _low);

  itemChanged();
}

QwtDoubleRect HistogramDraw::boundingRect () const
{
  QwtDoubleRect rect(0, 0, 0, 0);
  rect.setBottom(_low);
  rect.setTop(_high);

  return rect;
}

int HistogramDraw::rtti () const
{
  return QwtPlotCurve::UserCurve;
}

int HistogramDraw::highLow (double &h, double &l)
{
  if (! _line)
    return 1;

  h = _high;
  l = _low;

  return 0;
}

int HistogramDraw::highLowRange (int start, int end, double &h, double &l)
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

int HistogramDraw::info (int index, Setting *data)
{
  if (! _line)
    return 1;

  CurveBar *b = _line->bar(index);
  if (! b)
    return 1;

  Strip strip;
  QString d;
  strip.strip(b->data(), 4, d);
  data->setData(_line->label(), d);

  return 0;
}
