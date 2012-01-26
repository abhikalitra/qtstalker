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

#include "CurveHistogram.h"
#include "KeyCurveData.h"
#include "KeyCurveBar.h"
#include "Strip.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_scale_div.h>
#include <QDebug>

CurveHistogram::CurveHistogram ()
{
  _list << "Histogram" << "Histogram Bar";
}

void CurveHistogram::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  QwtScaleDiv *sd = plot()->axisScaleDiv(QwtPlot::xBottom);

  int loop = sd->lowerBound();
  int size = sd->upperBound();

//  int zero = yMap.transform(0);

  KeyCurveData cdkeys;
  KeyCurveBar cbkeys;
  Data style;
  _settings.toData(cdkeys.indexToString(KeyCurveData::_STYLE), style);
  if (style.toString() == "Histogram")
  {
    for (; loop < size; loop++)
    {
      Entity yb;
      if (_settings.toIndex(loop - 1, yb))
        continue;

      Entity b;
      if (_settings.toIndex(loop, b))
        continue;

      QPolygon poly;

      // bottom left
      Data td;
      if (yb.toData(cbkeys.indexToString(KeyCurveBar::_LOW), td))
	continue;
      int x = xMap.transform(loop - 1);
      int y = yMap.transform(td.toDouble());
      poly << QPoint(x, y);

      // top left
      if (yb.toData(cbkeys.indexToString(KeyCurveBar::_HIGH), td))
	continue;
      y = yMap.transform(td.toDouble());
      poly << QPoint(x, y);

      // top right
      if (b.toData(cbkeys.indexToString(KeyCurveBar::_HIGH), td))
	continue;
      x = xMap.transform(loop);
      y = yMap.transform(td.toDouble());
      poly << QPoint(x, y);

      // bottom right
      if (b.toData(cbkeys.indexToString(KeyCurveBar::_LOW), td))
	continue;
      y = yMap.transform(td.toDouble());
      poly << QPoint(x, y);

      if (b.toData(cbkeys.indexToString(KeyCurveBar::_COLOR), td))
	continue;
      painter->setBrush(td.toColor());
      painter->drawPolygon(poly, Qt::OddEvenFill);
    }
  }
  else
  {
    // histogram bar
    for (; loop < size; loop++)
    {
      Entity b;
      if (_settings.toIndex(loop, b))
        continue;

      Data high;
      if (b.toData(cbkeys.indexToString(KeyCurveBar::_HIGH), high))
        continue;
      
      Data low;
      if (b.toData(cbkeys.indexToString(KeyCurveBar::_LOW), low))
        continue;
      
      if (low.toDouble() > high.toDouble())
      {
        QPolygon poly;

        // bottom left
        int x = xMap.transform(loop);
        int y = yMap.transform(high.toDouble());
        poly << QPoint(x, y);

        // top left
        y = yMap.transform(low.toDouble());
        poly << QPoint(x, y);

        int width = xMap.transform(loop + 1) - x;

        // top right
        x += width;
        y = yMap.transform(low.toDouble());
        poly << QPoint(x, y);

        // bottom right
        y = yMap.transform(high.toDouble());
        poly << QPoint(x, y);

        Data color;
        if (b.toData(cbkeys.indexToString(KeyCurveBar::_COLOR), color))
          continue;
	
        painter->setBrush(color.toColor());
        painter->drawPolygon(poly, Qt::OddEvenFill);
      }
      else
      {
        QPolygon poly;

        // bottom left
        int x = xMap.transform(loop);
        int y = yMap.transform(low.toDouble());
        poly << QPoint(x, y);

        // top left
        y = yMap.transform(high.toDouble());
        poly << QPoint(x, y);

        int width = xMap.transform(loop + 1) - x;

        // top right
        x += width;
        y = yMap.transform(high.toDouble());
        poly << QPoint(x, y);

        // bottom right
        y = yMap.transform(low.toDouble());
        poly << QPoint(x, y);

        Data color;
        if (b.toData(cbkeys.indexToString(KeyCurveBar::_COLOR), color))
          continue;
	
        painter->setBrush(color.toColor());
        painter->drawPolygon(poly, Qt::OddEvenFill);
      }
    }
  }
}

int CurveHistogram::info (int index, Entity &data)
{
  Entity bar;
  if (_settings.toIndex(index, bar))
    return 1;

  KeyCurveBar cbkeys;
  Data high;
  if (bar.toData(cbkeys.indexToString(KeyCurveBar::_HIGH), high))
    return 1;

  KeyCurveData cdkeys;
  Data label;
  if (_settings.toData(cdkeys.indexToString(KeyCurveData::_LABEL), label))
    return 1;
  
  Strip strip;
  QString s;
  strip.strip(high.toDouble(), 4, s);

  data.set(label.toString(), Data(s));

  return 0;
}

int CurveHistogram::scalePoint (int i, QColor &color, double &v)
{
  Entity bar;
  if (_settings.toIndex(i, bar))
    return 1;

  KeyCurveBar cbkeys;
  Data td;
  if (bar.toData(cbkeys.indexToString(KeyCurveBar::_COLOR), td))
    return 1;
  color = td.toColor();
  
  if (bar.toData(cbkeys.indexToString(KeyCurveBar::_HIGH), td))
    return 1;
  v = td.toDouble();

  return 0;
}
