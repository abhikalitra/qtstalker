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
#include "ChartObjectKey.h"

#include <QDebug>
#include <QPolygon>
#include <QStatusBar>
#include <qwt_plot.h>

ChartObjectVLine::ChartObjectVLine ()
{
  ChartObjectKey keys;
  
  Data td(QString("VLine"));
  _settings.set(keys.indexToString(ChartObjectKey::_TYPE), td);
  
  td = Data(QDateTime::currentDateTime());
  td.setLabel(QObject::tr("Date"));
  _settings.set(keys.indexToString(ChartObjectKey::_DATE), td);
  
  td = Data(QColor(Qt::red));
  td.setLabel(QObject::tr("Color"));
  _settings.set(keys.indexToString(ChartObjectKey::_COLOR), td);
}

void ChartObjectVLine::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &, const QRect &) const
{
  ChartObjectKey keys;
  Data date, color;
  _settings.toData(keys.indexToString(ChartObjectKey::_DATE), date);
  _settings.toData(keys.indexToString(ChartObjectKey::_COLOR), color);

  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(date.toDateTime()));

  p->setPen(color.toColor());

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
		  color.toColor());
    }
  }
}

int ChartObjectVLine::info (Entity &info)
{
  ChartObjectKey keys;
  Data type, date;
  _settings.toData(keys.indexToString(ChartObjectKey::_TYPE), type);
  _settings.toData(keys.indexToString(ChartObjectKey::_DATE), date);
  
  info.set(QObject::tr("Type"), type);

  QDateTime dt = date.toDateTime();
  info.set(QString("D"), Data(dt.toString("yyyy-MM-dd")));
  info.set(QString("T"), Data(dt.toString("HH:mm:ss")));

  return 0;
}

void ChartObjectVLine::move (QPoint p)
{
  switch (_status)
  {
    case _MOVE:
    {
      ChartObjectKey keys;
      Data date;
      _settings.toData(keys.indexToString(ChartObjectKey::_DATE), date);
      
      QwtScaleMap map = plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) p.x());

      DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
      QDateTime dt;
      dsd->date(x, dt);
      date.set(dt);
      _settings.set(keys.indexToString(ChartObjectKey::_DATE), date);

      plot()->replot();

      g_parent->statusBar()->showMessage(date.toString());

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
