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

#include "CurveOHLC.h"
#include "Strip.h"
#include "CurveDataKey.h"
#include "CurveBarKey.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_scale_div.h>
#include <QDebug>

CurveOHLC::CurveOHLC ()
{
}

void CurveOHLC::draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  QwtScaleDiv *sd = plot()->axisScaleDiv(QwtPlot::xBottom);

  int loop = sd->lowerBound();
  int size = sd->upperBound();

  Data style;
  CurveDataKey cdkeys;
  CurveBarKey cbkeys;
  
  _settings.toData(cdkeys.indexToString(CurveDataKey::_STYLE), style);
  if (style.toString() == "OHLC")
  {
    for (; loop < size; loop++)
    {
      Entity b;
      if (_settings.toEntity(QString::number(loop), b))
        continue;

      Data td;
      if (b.toData(cbkeys.indexToString(CurveBarKey::_COLOR), td))
	continue;
      painter->setPen(td.toColor());

      int x = xMap.transform(loop);
      
      if (b.toData(cbkeys.indexToString(CurveBarKey::_OPEN), td))
	continue;
      int yo = yMap.transform(td.toDouble());
      
      if (b.toData(cbkeys.indexToString(CurveBarKey::_HIGH), td))
	continue;
      int yh = yMap.transform(td.toDouble());
      
      if (b.toData(cbkeys.indexToString(CurveBarKey::_LOW), td))
	continue;
      int yl = yMap.transform(td.toDouble());
      
      if (b.toData(cbkeys.indexToString(CurveBarKey::_CLOSE), td))
	continue;
      int yc = yMap.transform(td.toDouble());
      
      int width = xMap.transform(loop + 1) - x;

      QRect rect(QPoint(x + 1, yh), QPoint(x + width - 1, yl));

      // draw the high/low line
      painter->drawLine (rect.center().x(), yh, rect.center().x(), yl);

      // draw the open tick
      painter->drawLine (rect.left(), yo, rect.center().x(), yo);

      // draw the close tick
      painter->drawLine (rect.right(), yc, rect.center().x(), yc);
    }
  }
  else
  {
    bool ff = FALSE;
    for (; loop < size; loop++)
    {
      Entity b;
      if (_settings.toEntity(QString::number(loop), b))
        continue;

      Data close;
      if (b.toData(cbkeys.indexToString(CurveBarKey::_CLOSE), close))
	continue;
      
      Data open;
      if (b.toData(cbkeys.indexToString(CurveBarKey::_OPEN), open))
	continue;
      
      ff = FALSE;
      if (close.toDouble() < open.toDouble())
        ff = TRUE;

      Data color;
      if (b.toData(cbkeys.indexToString(CurveBarKey::_COLOR), color))
	continue;
      painter->setPen(color.toColor());

      int x = xMap.transform(loop);
      
      int xo = yMap.transform(open.toDouble());

      Data td;
      if (b.toData(cbkeys.indexToString(CurveBarKey::_HIGH), td))
	continue;
      int xh = yMap.transform(td.toDouble());
      
      if (b.toData(cbkeys.indexToString(CurveBarKey::_LOW), td))
	continue;
      int xl = yMap.transform(td.toDouble());
      
      int xc = yMap.transform(close.toDouble());

      int width = xMap.transform(loop + 1) - x;

      if (! ff)
      {
        // empty candle c > o
        QRect rect(QPoint(x + 2, xc), QPoint(x + width - 2, xo));
        painter->drawLine (rect.center().x(), xh, rect.center().x(), xl);
        painter->setBrush(plot()->canvasBackground());
        painter->setPen(color.toColor());
        painter->drawRect(rect);
      }
      else
      {
        // filled candle c < o
        QRect rect(QPoint(x + 2, xo), QPoint(x + width - 2, xc));
        painter->drawLine (rect.center().x(), xh, rect.center().x(), xl);
        painter->setBrush(color.toColor());
        painter->drawRect(rect);
      }
    }
  }
}

int CurveOHLC::info (int index, Entity &data)
{
  Entity bar;
  if (_settings.toEntity(QString::number(index), bar))
    return 1;

  Strip strip;
  QString d;
  
  CurveBarKey cbkeys;
  Data td;
  if (bar.toData(cbkeys.indexToString(CurveBarKey::_OPEN), td))
    return 1;
  strip.strip(td.toDouble(), 4, d);
  data.set(QString("O"), d);

  if (bar.toData(cbkeys.indexToString(CurveBarKey::_HIGH), td))
    return 1;
  strip.strip(td.toDouble(), 4, d);
  data.set(QString("H"), d);

  if (bar.toData(cbkeys.indexToString(CurveBarKey::_LOW), td))
    return 1;
  strip.strip(td.toDouble(), 4, d);
  data.set(QString("L"), d);

  if (bar.toData(cbkeys.indexToString(CurveBarKey::_CLOSE), td))
    return 1;
  strip.strip(td.toDouble(), 4, d);
  data.set(QString("C"), d);

  return 0;
}

int CurveOHLC::scalePoint (int i, QColor &color, double &v)
{
  Entity bar;
  if (_settings.toEntity(QString::number(i), bar))
    return 1;

  CurveBarKey cbkeys;
  Data td;
  if (bar.toData(cbkeys.indexToString(CurveBarKey::_COLOR), td))
    return 1;
  color = td.toColor();
  
  if (bar.toData(cbkeys.indexToString(CurveBarKey::_CLOSE), td))
    return 1;
  v = td.toDouble();

  return 0;
}
