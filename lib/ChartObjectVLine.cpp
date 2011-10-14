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


#include "ChartObjectVLine.h"
#include "GlobalParent.h"
#include "DateScaleDraw.h"
#include "ChartObjectData.h"
#include "DataDateTime.h"
#include "DataString.h"
#include "DataInteger.h"
#include "DataColor.h"

#include <QDebug>
#include <QPolygon>
#include <QStatusBar>
#include <qwt_plot.h>

ChartObjectVLine::ChartObjectVLine ()
{
  _settings->set(ChartObjectData::_TYPE, new DataString(QString("VLine")));
  _settings->set(ChartObjectData::_DATE, new DataDateTime(QDateTime::currentDateTime()));
  _settings->set(ChartObjectData::_COLOR, new DataColor(QColor(Qt::red)));
}

void ChartObjectVLine::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings->toData(ChartObjectData::_DATE)->toDateTime()));

  p->setPen(_settings->toData(ChartObjectData::_COLOR)->toColor());

  p->drawLine (x, 0, x, p->window().height());

  _selectionArea.clear();

  QPolygon array;
  array.putPoints(0,
  	          4,
		  x - 2, 0,
		  x + 2, 0,
		  x + 2, p->window().height(),
		  x - 2, p->window().height());

  _selectionArea.append(QRegion(array));

  if (_selected)
  {
    _grabHandles.clear();

    int t = (int) p->window().height() / 4;
    int loop;
    for (loop = 0; loop < 5; loop++)
    {
      _grabHandles.append(QRegion(x - (_handleWidth / 2),
		                  t * loop,
		                  _handleWidth,
		                  _handleWidth,
		                  QRegion::Rectangle));

      p->fillRect(x - (_handleWidth / 2),
		  t * loop,
		  _handleWidth,
		  _handleWidth,
		  _settings->toData(ChartObjectData::_COLOR)->toColor());
    }
  }
}

int ChartObjectVLine::info (Message &info)
{
  info.insert(QObject::tr("Type"), _settings->toData(ChartObjectData::_TYPE)->toString());

  QDateTime dt = _settings->toData(ChartObjectData::_DATE)->toDateTime();
  info.insert("D", dt.toString("yyyy-MM-dd"));
  info.insert("T", dt.toString("HH:mm:ss"));

  return 0;
}

void ChartObjectVLine::move (QPoint p)
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
      _settings->set(ChartObjectData::_DATE, new DataDateTime(dt));

      plot()->replot();

      QString s = _settings->toData(ChartObjectData::_DATE)->toString();
      g_parent->statusBar()->showMessage(s);

      _modified++;
      break;
    }
    default:
      break;
  }
}

int ChartObjectVLine::create ()
{
  _status = _MOVE;
  setSelected(TRUE);
  g_parent->statusBar()->showMessage(QObject::tr("Place VLine object..."));
  return 0;
}
