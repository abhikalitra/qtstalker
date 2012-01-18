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


#include "ChartObjectHLine.h"
#include "GlobalParent.h"
#include "Strip.h"
#include "ChartObjectKey.h"

#include <QDebug>
#include <QPolygon>
#include <QStatusBar>
#include <qwt_plot.h>

ChartObjectHLine::ChartObjectHLine ()
{
  ChartObjectKey keys;

  Data td(QString("HLine"));
  _settings.set(keys.indexToString(ChartObjectKey::_TYPE), td);
  
  td = Data(0.0);
  td.setLabel(QObject::tr("Price"));
  _settings.set(keys.indexToString(ChartObjectKey::_PRICE), td);

  td = Data(QColor(Qt::red));
  td.setLabel(QObject::tr("Color"));
  _settings.set(keys.indexToString(ChartObjectKey::_COLOR), td);
}

void ChartObjectHLine::draw (QPainter *p, const QwtScaleMap &, const QwtScaleMap &yMap, const QRect &) const
{
  ChartObjectKey keys;
  Data color, price;
  _settings.toData(keys.indexToString(ChartObjectKey::_PRICE), price);
  _settings.toData(keys.indexToString(ChartObjectKey::_COLOR), color);
  
  p->setPen(color.toColor());

  int y = yMap.transform(price.toDouble());

  // test start
  Strip strip;
  QString ts;
  strip.strip(price.toDouble(), 4, ts);
  QString s = " " + ts; // prepend space so we can clearly read text
  QFontMetrics fm = p->fontMetrics();
  QRect rc = p->boundingRect(0, y - (fm.height() / 2), 1, 1, 0, s);
  p->fillRect(rc, plot()->canvasBackground()); // fill in behind text first
  p->drawText(rc, s); // draw text
  p->drawRect(rc); // draw a nice little box around text

  p->drawLine (rc.width(), y, p->window().width(), y);

  _selectionArea.clear();

  QPolygon array;
  array.putPoints(0,
                  4,
                  0,
                  y - 4,
                  0,
                  y + 4,
                  p->window().width(),
                  y + 4,
                  p->window().width(),
                  y - 4);

  _selectionArea.append(QRegion(array));

  if (_selected)
  {
    _grabHandles.clear();

    int t = (int) p->window().width() / 4;
    int loop;
    for (loop = 0; loop < 5; loop++)
    {
      _grabHandles.append(QRegion(t * loop,
                                  y - (_handleWidth / 2),
                                  _handleWidth,
                                  _handleWidth,
                                  QRegion::Rectangle));

      p->fillRect(t * loop,
                  y - (_handleWidth / 2),
                  _handleWidth,
                  _handleWidth,
                  color.toColor());
    }
  }
}

int ChartObjectHLine::info (Entity &info)
{
  ChartObjectKey keys;
  Data type, price;
  _settings.toData(keys.indexToString(ChartObjectKey::_PRICE), price);
  _settings.toData(keys.indexToString(ChartObjectKey::_TYPE), type);

  info.set(QObject::tr("Type"), type);
  info.set(QObject::tr("Price"), price);
  
  return 0;
}

void ChartObjectHLine::move (QPoint p)
{
  switch (_status)
  {
    case _MOVE:
    {
      ChartObjectKey keys;
      Data price;
      _settings.toData(keys.indexToString(ChartObjectKey::_PRICE), price);
      
      QwtScaleMap map = plot()->canvasMap(QwtPlot::yRight);
      price.set(map.invTransform((double) p.y()));
      _settings.set(keys.indexToString(ChartObjectKey::_PRICE), price);

      plot()->replot();

      Strip strip;
      QString s;
      strip.strip(price.toDouble(), 4, s);
      g_parent->statusBar()->showMessage(s);

      _modified++;
      break;
    }
    default:
      break;
  }
}

int ChartObjectHLine::create ()
{
  _status = _MOVE;
  setSelected(TRUE);
  g_parent->statusBar()->showMessage(QObject::tr("Place HLine object..."));
  return 0;
}

int ChartObjectHLine::highLow (int, int, double &high, double &low)
{
  ChartObjectKey keys;
  Data price;
  _settings.toData(keys.indexToString(ChartObjectKey::_PRICE), price);

  high = price.toDouble();
  low = price.toDouble();
  
  return 0;
}
