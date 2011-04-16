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

#include "HistogramBarDraw.h"
#include "Strip.h"

#include <QDebug>
#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_scale_div.h>

class HistogramBarDraw::PrivateData
{
  public:
    int attributes;
    QwtIntervalData data;
    QColor color;
    double reference;
};

HistogramBarDraw::HistogramBarDraw (const QwtText &title) : QwtPlotCurve (title)
{
  _line = 0;
  init();
}

HistogramBarDraw::HistogramBarDraw (const QString &title) : QwtPlotCurve (QwtText(title))
{
  _line = 0;
  init();
}

HistogramBarDraw::~HistogramBarDraw ()
{
  delete _data;
  if (_line)
    delete _line;
}

void HistogramBarDraw::init ()
{
  if (_line)
    delete _line;
  _line = 0;

  _high = 0;
  _low = 0;

  _data = new PrivateData();
  _data->reference = 0.0;
  _data->attributes = HistogramBarDraw::Auto;
  _data->color = QColor(Qt::red);

  setItemAttribute(QwtPlotItem::AutoScale, TRUE);
  setItemAttribute(QwtPlotItem::Legend, TRUE);

  setZ(-1);
}

void HistogramBarDraw::setBaseline (double reference)
{
  if ( _data->reference != reference )
  {
    _data->reference = reference;
    itemChanged();
  }
}

double HistogramBarDraw::baseline () const
{
  return _data->reference;
}

void HistogramBarDraw::setData (Curve *curve)
{
  init();

  _line = new Curve;
  curve->copy(_line);

  curve->highLow(_high, _low);
  
  QList<int> keys;
  curve->keys(keys);
  if (! keys.count())
    return;

  _colors.clear();

  int end = keys.at(keys.count() - 1);

  QwtArray<QwtDoubleInterval> intervals;
  QwtArray<double> values;
  double pos = 0;
  int width = 1;
  int loop = 0;
  for (; loop <= end; loop++)
  {
    intervals << QwtDoubleInterval(pos, pos + double(width));
    CurveBar *bar = curve->bar(loop);
    if (bar)
    {
      values << bar->data();
      _colors.insert(loop, bar->color());
    }
    else
    {
      values << 0;
      _colors.insert(loop, QColor());
    }
    pos += width;
  }

  _data->data = QwtIntervalData(intervals, values);

  itemChanged();
}

const QwtIntervalData & HistogramBarDraw::data () const
{
  return _data->data;
}

void HistogramBarDraw::setColor (const QColor &color)
{
  if ( _data->color != color )
  {
    _data->color = color;
    itemChanged();
  }
}

QColor HistogramBarDraw::color () const
{
  return _data->color;
}

QwtDoubleRect HistogramBarDraw::boundingRect () const
{
  QwtDoubleRect rect = _data->data.boundingRect();
  if (! rect.isValid())
    return rect;

  if (_data->attributes & Xfy)
  {
    rect = QwtDoubleRect(rect.y(), rect.x(), rect.height(), rect.width());

    if (rect.left() > _data->reference)
      rect.setLeft(_data->reference);
    else if (rect.right() < _data->reference)
      rect.setRight(_data->reference);
  } 
  else
  {
    if (rect.bottom() < _data->reference)
      rect.setBottom(_data->reference);
    else if (rect.top() > _data->reference)
      rect.setTop(_data->reference);
  }

  return rect;
}

int HistogramBarDraw::rtti () const
{
  return QwtPlotItem::Rtti_PlotHistogram;
}

void HistogramBarDraw::setHistogramAttribute (HistogramAttribute attribute, bool on)
{
  if (bool(_data->attributes & attribute) == on)
    return;

  if (on)
    _data->attributes |= attribute;
  else
    _data->attributes &= ~attribute;

  itemChanged();
}

bool HistogramBarDraw::testHistogramAttribute (HistogramAttribute attribute) const
{
  return _data->attributes & attribute;
}

void HistogramBarDraw::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  const QwtIntervalData &iData = _data->data;

  const int x0 = xMap.transform(baseline());
  const int y0 = yMap.transform(baseline());

  QwtScaleDiv *sd = plot()->axisScaleDiv(QwtPlot::xBottom);
  int i = sd->lowerBound();
  int size = sd->upperBound();
  if (size > (int) iData.size())
    size = (int) iData.size();

  for (; i < size; i++)
  {
    if (_data->attributes & HistogramBarDraw::Xfy)
    {
      const int x2 = xMap.transform(iData.value(i));
      if (x2 == x0)
        continue;

      int y1 = yMap.transform(iData.interval(i).minValue());
      int y2 = yMap.transform(iData.interval(i).maxValue());
      if (y1 > y2)
        qSwap(y1, y2);

      if (i < (int)iData.size() - 2)
      {
        const int yy1 = yMap.transform(iData.interval(i + 1).minValue());
        const int yy2 = yMap.transform(iData.interval(i + 1).maxValue());

        if (y2 == qwtMin(yy1, yy2))
        {
          const int xx2 = xMap.transform(iData.interval(i + 1).minValue());
          if (xx2 != x0 && ((xx2 < x0 && x2 < x0) || (xx2 > x0 && x2 > x0)))
          {
            // One pixel distance between neighboured bars
            y2++;
          }
        }
      }

      QColor color = _colors.value(i);
      painter->setPen(QPen(color));
      drawBar(painter, Qt::Horizontal, QRect(x0, y1, x2 - x0, y2 - y1));
    }
    else
    {
      const int y2 = yMap.transform(iData.value(i));
      if (y2 == y0)
        continue;

      int x1 = xMap.transform(iData.interval(i).minValue());
      int x2 = xMap.transform(iData.interval(i).maxValue());
      if (x1 > x2)
        qSwap(x1, x2);

      if (i < (int)iData.size() - 2)
      {
        const int xx1 = xMap.transform(iData.interval(i + 1).minValue());
        const int xx2 = xMap.transform(iData.interval(i + 1).maxValue());

        if (x2 == qwtMin(xx1, xx2))
        {
          const int yy2 = yMap.transform(iData.value(i + 1));
          if (yy2 != y0 && ((yy2 < y0 && y2 < y0) || (yy2 > y0 && y2 > y0)))
          {
            // One pixel distance between neighboured bars
            x2--;
          }
        }
      }

      QColor color = _colors.value(i);
      painter->setPen(QPen(color));
      drawBar(painter, Qt::Vertical, QRect(x1, y0, x2 - x1, y2 - y0));
    }
  }
}

void HistogramBarDraw::drawBar (QPainter *painter, Qt::Orientation, const QRect& rect) const
{
  painter->save();

  const QColor color(painter->pen().color());
  const QRect r = rect.normalized();

  const int factor = 125;
  const QColor light(color.light(factor));
  const QColor dark(color.dark(factor));

  painter->setBrush(color);
  painter->setPen(Qt::NoPen);
  QwtPainter::drawRect(painter, r.x() + 1, r.y() + 1, r.width() - 2, r.height() - 2);
  painter->setBrush(Qt::NoBrush);

  painter->setPen(QPen(light, 2));
  QwtPainter::drawLine(painter, r.left() + 1, r.top() + 2, r.right() + 1, r.top() + 2);

  painter->setPen(QPen(dark, 2));
  QwtPainter::drawLine(painter, r.left() + 1, r.bottom(), r.right() + 1, r.bottom());

  painter->setPen(QPen(light, 1));
  QwtPainter::drawLine(painter, r.left(), r.top() + 1, r.left(), r.bottom());
  QwtPainter::drawLine(painter, r.left() + 1, r.top() + 2, r.left() + 1, r.bottom() - 1);

  painter->setPen(QPen(dark, 1));
  QwtPainter::drawLine(painter, r.right() + 1, r.top() + 1, r.right() + 1, r.bottom());
  QwtPainter::drawLine(painter, r.right(), r.top() + 2, r.right(), r.bottom() - 1);

  painter->restore();
}

int HistogramBarDraw::highLowRange (int start, int end, double &h, double &l)
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

int HistogramBarDraw::info (int index, Setting *data)
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

int HistogramBarDraw::highLow (double &h, double &l)
{
  if (! _line)
    return 1;

  h = _high;
  l = _low;

  return 0;
}

