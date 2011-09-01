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
#include "Globals.h"
#include "DateScaleDraw.h"
#include "ChartObjectData.h"

#include <QDebug>
#include <QPolygon>
#include <QStatusBar>
#include <qwt_plot.h>

ChartObjectSell::ChartObjectSell ()
{
  _settings->set(ChartObjectData::_TYPE, QString("Sell"));
  _settings->set(ChartObjectData::_DATE, QDateTime::currentDateTime());
  _settings->set(ChartObjectData::_PRICE, 0);
  _settings->set(ChartObjectData::_COLOR, QColor(Qt::red));
  _settings->set(ChartObjectData::_Z, 1);
  _settings->set(ChartObjectData::_PEN, 1);
}

void ChartObjectSell::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings->getDateTime(ChartObjectData::_DATE)));

  int y = yMap.transform(_settings->getDouble(ChartObjectData::_PRICE));

  p->setBrush(_settings->getColor(ChartObjectData::_COLOR));

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
                _settings->getColor(ChartObjectData::_COLOR));
  }
}

int ChartObjectSell::info (Message &info)
{
  info.insert(QObject::tr("Type"), _settings->get(ChartObjectData::_TYPE));

  QDateTime dt = _settings->getDateTime(ChartObjectData::_DATE);
  info.insert("D", dt.toString("yyyy-MM-dd"));
  info.insert("T", dt.toString("HH:mm:ss"));

  info.insert(QObject::tr("Price"), _settings->get(ChartObjectData::_PRICE));

  return 0;
}

void ChartObjectSell::move (QPoint p)
{
  switch (_status)
  {
    case _MOVE:
    {
      QwtScaleMap map = plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) p.x());

      DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
      QDateTime dt;
      dsd->date(x, dt);
      _settings->set(ChartObjectData::_DATE, dt);

      map = plot()->canvasMap(QwtPlot::yRight);
      _settings->set(ChartObjectData::_PRICE, map.invTransform((double) p.y()));

      plot()->replot();

      QString s = _settings->get(ChartObjectData::_DATE) + " " + _settings->get(ChartObjectData::_PRICE);
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
