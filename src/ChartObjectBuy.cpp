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


#include "ChartObjectBuy.h"
#include "Globals.h"
#include "DateScaleDraw.h"
#include "ChartObjectData.h"
#include "SettingDouble.h"
#include "SettingDateTime.h"
#include "SettingString.h"
#include "SettingInteger.h"
#include "SettingColor.h"

#include <QDebug>
#include <QPolygon>
#include <QStatusBar>
#include <qwt_plot.h>

ChartObjectBuy::ChartObjectBuy ()
{
  _settings->set(ChartObjectData::_TYPE, new SettingString(QString("Buy")));
  _settings->set(ChartObjectData::_DATE, new SettingDateTime(QDateTime::currentDateTime()));
  _settings->set(ChartObjectData::_PRICE, new SettingDouble(0));
  _settings->set(ChartObjectData::_COLOR, new SettingColor(QColor(Qt::green)));
  _settings->set(ChartObjectData::_Z, new SettingInteger(1));
  _settings->set(ChartObjectData::_PEN, new SettingInteger(1));
}

void ChartObjectBuy::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings->get(ChartObjectData::_DATE)->toDateTime()));

  int y = yMap.transform(_settings->get(ChartObjectData::_PRICE)->toDouble());

  p->setBrush(_settings->get(ChartObjectData::_COLOR)->toColor());

  QPolygon arrow;
  arrow.putPoints(0, 7, x, y,
                  x + 5, y + 5,
                  x + 2, y + 5,
                  x + 2, y + 11,
                  x - 2, y + 11,
                  x - 2, y + 5,
                  x - 5, y + 5);

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
                y - _handleWidth,
                _handleWidth,
                _handleWidth,
                _settings->get(ChartObjectData::_COLOR)->toColor());
  }
}

int ChartObjectBuy::info (Message &info)
{
  info.insert(QObject::tr("Type"), _settings->get(ChartObjectData::_TYPE)->toString());

  QDateTime dt = _settings->get(ChartObjectData::_DATE)->toDateTime();
  info.insert("D", dt.toString("yyyy-MM-dd"));
  info.insert("T", dt.toString("HH:mm:ss"));

  info.insert(QObject::tr("Price"), _settings->get(ChartObjectData::_PRICE)->toString());

  return 0;
}

void ChartObjectBuy::move (QPoint p)
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
      _settings->set(ChartObjectData::_DATE, new SettingDateTime(dt));

      map = plot()->canvasMap(QwtPlot::yRight);
      _settings->set(ChartObjectData::_PRICE, new SettingDouble(map.invTransform((double) p.y())));

      plot()->replot();

      QString s = _settings->get(ChartObjectData::_DATE)->toString() + " " + _settings->get(ChartObjectData::_PRICE)->toString();
      g_parent->statusBar()->showMessage(s);

      _modified++;
      break;
    }
    default:
      break;
  }
}

int ChartObjectBuy::create ()
{
  _status = _MOVE;
  setSelected(TRUE);
  g_parent->statusBar()->showMessage(QObject::tr("Place Buy object..."));
  return 0;
}
