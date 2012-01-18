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


#include "ChartObjectSell.h"
#include "GlobalParent.h"
#include "DateScaleDraw.h"
#include "ChartObjectKey.h"

#include <QDebug>
#include <QPolygon>
#include <QStatusBar>
#include <qwt_plot.h>

ChartObjectSell::ChartObjectSell ()
{
  ChartObjectKey keys;
  
  Data td(QString("Sell"));
  _settings.set(keys.indexToString(ChartObjectKey::_TYPE), td);
  
  td = Data(QDateTime::currentDateTime());
  td.setLabel(QObject::tr("Date"));
  _settings.set(keys.indexToString(ChartObjectKey::_DATE), td);
  
  td = Data(0.0);
  td.setLabel(QObject::tr("Price"));
  _settings.set(keys.indexToString(ChartObjectKey::_PRICE), td);
  
  td = Data(QColor(Qt::red));
  td.setLabel(QObject::tr("Color"));
  _settings.set(keys.indexToString(ChartObjectKey::_COLOR), td);
}

void ChartObjectSell::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  ChartObjectKey keys;
  Data date, price, color;
  _settings.toData(keys.indexToString(ChartObjectKey::_DATE), date);
  _settings.toData(keys.indexToString(ChartObjectKey::_PRICE), price);
  _settings.toData(keys.indexToString(ChartObjectKey::_COLOR), color);

  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(date.toDateTime()));

  int y = yMap.transform(price.toDouble());

  p->setBrush(color.toColor());

  QPolygon arrow;
  arrow.putPoints(0, 7, x, y,
                  x + 5, y - 5,
                  x + 2, y - 5,
                  x + 2, y - 11,
                  x - 2, y - 11,
                  x - 2, y - 5,
                  x - 5, y - 5);

  p->drawPolygon(arrow, Qt::OddEvenFill);

  _selectionArea.clear();

  _selectionArea.append(QRegion(arrow));

  if (_selected)
  {
    _grabHandles.clear();

    _grabHandles.append(QRegion(x - (_handleWidth / 2),
                                y - _handleWidth,
                                _handleWidth,
                                _handleWidth,
                                QRegion::Rectangle));

    p->fillRect(x - (_handleWidth / 2),
                y + (_handleWidth / 2),
                _handleWidth,
                _handleWidth,
                color.toColor());
  }
}

int ChartObjectSell::info (Entity &info)
{
  ChartObjectKey keys;
  Data type, date, price;
  _settings.toData(keys.indexToString(ChartObjectKey::_TYPE), type);
  _settings.toData(keys.indexToString(ChartObjectKey::_DATE), date);
  _settings.toData(keys.indexToString(ChartObjectKey::_PRICE), price);
  
  info.set(QObject::tr("Type"), type);

  info.set(QObject::tr("Price"), price);
  
  QDateTime dt = date.toDateTime();
  info.set(QString("D"), Data(dt.toString("yyyy-MM-dd")));
  info.set(QString("T"), Data(dt.toString("HH:mm:ss")));

  return 0;
}

void ChartObjectSell::move (QPoint p)
{
  switch (_status)
  {
    case _MOVE:
    {
      ChartObjectKey keys;
      Data date, price;
      _settings.toData(keys.indexToString(ChartObjectKey::_DATE), date);
      _settings.toData(keys.indexToString(ChartObjectKey::_PRICE), price);

      QwtScaleMap map = plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) p.x());

      DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
      QDateTime dt;
      dsd->date(x, dt);
      date.set(dt);
      _settings.set(keys.indexToString(ChartObjectKey::_DATE), date);

      map = plot()->canvasMap(QwtPlot::yRight);
      price.set(map.invTransform((double) p.y()));
      _settings.set(keys.indexToString(ChartObjectKey::_PRICE), price);

      plot()->replot();

      QString s = date.toString() + " " + price.toString();
      g_parent->statusBar()->showMessage(s);

      _modified++;
      break;
    }
    default:
      break;
  }
}

int ChartObjectSell::create ()
{
  _status = _MOVE;
  setSelected(TRUE);
  g_parent->statusBar()->showMessage(QObject::tr("Place Sell object..."));
  return 0;
}
