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

#include "PlotHistogramBar.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_scale_div.h>

class PlotHistogramBar::PrivateData
{
  public:
    int attributes;
    QwtIntervalData data;
    QColor color;
    double reference;
};

PlotHistogramBar::PlotHistogramBar (const QwtText &title) : QwtPlotCurve (title)
{
  init();
}

PlotHistogramBar::PlotHistogramBar (const QString &title) : QwtPlotCurve (QwtText(title))
{
  init();
}

PlotHistogramBar::~PlotHistogramBar ()
{
  delete _data;
}

void PlotHistogramBar::init ()
{
  _data = new PrivateData();
  _data->reference = 0.0;
  _data->attributes = PlotHistogramBar::Auto;
  _data->color = QColor(Qt::red);

  setItemAttribute(QwtPlotItem::AutoScale, TRUE);
  setItemAttribute(QwtPlotItem::Legend, TRUE);

  setZ(20.0);
}

void PlotHistogramBar::setBaseline (double reference)
{
  if ( _data->reference != reference )
  {
    _data->reference = reference;
    itemChanged();
  }
}

double PlotHistogramBar::baseline () const
{
  return _data->reference;
}

void PlotHistogramBar::setData (Curve *curve)
{
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

const QwtIntervalData & PlotHistogramBar::data () const
{
  return _data->data;
}

void PlotHistogramBar::setColor (const QColor &color)
{
  if ( _data->color != color )
  {
    _data->color = color;
    itemChanged();
  }
}

QColor PlotHistogramBar::color () const
{
  return _data->color;
}

QwtDoubleRect PlotHistogramBar::boundingRect () const
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

int PlotHistogramBar::rtti () const
{
  return QwtPlotItem::Rtti_PlotHistogram;
}

void PlotHistogramBar::setHistogramAttribute (HistogramAttribute attribute, bool on)
{
  if (bool(_data->attributes & attribute) == on)
    return;

  if (on)
    _data->attributes |= attribute;
  else
    _data->attributes &= ~attribute;

  itemChanged();
}

bool PlotHistogramBar::testHistogramAttribute (HistogramAttribute attribute) const
{
  return _data->attributes & attribute;
}

void PlotHistogramBar::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  const QwtIntervalData &iData = _data->data;

  const int x0 = xMap.transform(baseline());
  const int y0 = yMap.transform(baseline());

  QwtScaleDiv *sd = plot()->axisScaleDiv(QwtPlot::xBottom);
  int i = sd->lowerBound();
  int size = sd->upperBound();
  if (size > (int) iData.size())
    size = (int) iData.size();

//  for (int i = 0; i < (int)iData.size(); i++)
  for (; i < size; i++)
  {
    if (_data->attributes & PlotHistogramBar::Xfy)
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

void PlotHistogramBar::drawBar (QPainter *painter, Qt::Orientation, const QRect& rect) const
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

