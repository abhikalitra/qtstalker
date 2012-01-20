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

#include "CurveLine.h"
#include "KeyCurveData.h"
#include "KeyCurveBar.h"
#include "TypeLine.h"

#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_scale_div.h>
#include <QDebug>

CurveLine::CurveLine ()
{
}

void CurveLine::draw (QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  QwtScaleDiv *sd = plot()->axisScaleDiv(QwtPlot::xBottom);

  int loop = sd->lowerBound();
  int size = sd->upperBound();

//  painter->setRenderHint(QPainter::Antialiasing, TRUE);

  TypeLine ls;
  Data td;
  KeyCurveData cdkeys;
  KeyCurveBar cbkeys;
  
  _settings.toData(cdkeys.indexToString(KeyCurveData::_STYLE), td);
  int style = ls.stringToIndex(td.toString());

  _settings.toData(cdkeys.indexToString(KeyCurveData::_PEN), td);
  QPen tpen = painter->pen();
  tpen.setWidth(td.toInteger());
  switch ((TypeLine::Key) style)
  {
    case TypeLine::_DASH:
      tpen.setStyle((Qt::PenStyle) Qt::DashLine);
      break;
    case TypeLine::_DOT:
      tpen.setStyle((Qt::PenStyle) Qt::DotLine);
      break;
    default:
      tpen.setStyle((Qt::PenStyle) Qt::SolidLine);
      break;
  }

  if (style == (TypeLine::Key) TypeLine::_DASH || style == (TypeLine::Key) TypeLine::_LINE)
  {
    for (; loop < size; loop++)
    {
      Entity yb;
      if (_settings.toEntity(QString::number(loop - 1), yb))
        continue;

      Entity b;
      if (_settings.toEntity(QString::number(loop), b))
        continue;

      int x = xMap.transform(loop - 1);
      int x2 = xMap.transform(loop);

      Data td;
      if (yb.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), td))
	continue;
      int y = yMap.transform(td.toDouble());
      
      if (b.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), td))
	continue;
      int y2 = yMap.transform(td.toDouble());

      if (b.toData(cbkeys.indexToString(KeyCurveBar::_COLOR), td))
	continue;
      tpen.setColor(td.toColor());
      
      painter->setPen(tpen);

      painter->drawLine (x, y, x2, y2);
    }
  }
  else
  {
    for (; loop < size; loop++)
    {
      Entity b;
      if (_settings.toEntity(QString::number(loop), b))
        continue;

      int x = xMap.transform(loop);

      Data td;
      if (b.toData(cbkeys.indexToString(KeyCurveBar::_VALUE), td))
        continue;
      int y = yMap.transform(td.toDouble());

      if (b.toData(cbkeys.indexToString(KeyCurveBar::_COLOR), td))
        continue;
      tpen.setColor(td.toColor());
      
      painter->setPen(tpen);

      painter->drawPoint (x, y);
    }
  }
}
