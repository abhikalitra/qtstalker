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
#include "CurveData.h"
#include "CurveBar.h"

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

  for (; loop < size; loop++)
  {
    Data *yb = _settings->getData(loop - 1);
    if (! yb)
      continue;

    Data *b = _settings->getData(loop);
    if (! b)
      continue;

    int x = xMap.transform(loop - 1);
    int x2 = xMap.transform(loop);

    int y = yMap.transform(yb->get(CurveBar::_VALUE).toDouble());
    int y2 = yMap.transform(b->get(CurveBar::_VALUE).toDouble());

    QPen tpen = painter->pen();
    if (_settings->get(CurveData::_STYLE).toString() == "Line")
      tpen.setStyle((Qt::PenStyle) Qt::SolidLine);
    else
    {
      if (_settings->get(CurveData::_STYLE).toString() == "Dash")
        tpen.setStyle((Qt::PenStyle) Qt::DashLine);
      else
        tpen.setStyle((Qt::PenStyle) Qt::DotLine);
    }
    tpen.setWidth(_settings->get(CurveData::_PEN).toInt());
    tpen.setColor(QColor(b->get(CurveBar::_COLOR).toString()));
    painter->setPen(tpen);

    painter->drawLine (x, y, x2, y2);
  }
}
