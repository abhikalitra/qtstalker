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


#include "ChartObjectText.h"
#include "GlobalParent.h"
#include "DateScaleDraw.h"
#include "ChartObjectData.h"
#include "DataDouble.h"
#include "DataDateTime.h"
#include "DataString.h"
#include "DataInteger.h"
#include "DataColor.h"
#include "DataFont.h"

#include <QDebug>
#include <QPolygon>
#include <QStatusBar>
#include <qwt_plot.h>

ChartObjectText::ChartObjectText ()
{
  _settings->set(ChartObjectData::_TYPE, new DataString(QString("Text")));
  _settings->set(ChartObjectData::_TEXT, new DataString(QString("Text")));
  _settings->set(ChartObjectData::_DATE, new DataDateTime(QDateTime::currentDateTime()));
  _settings->set(ChartObjectData::_PRICE, new DataDouble(0));
  _settings->set(ChartObjectData::_COLOR, new DataColor(QColor(Qt::red)));

  QFont f;
  _settings->set(ChartObjectData::_FONT, new DataFont(f.toString()));
}

void ChartObjectText::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings->toData(ChartObjectData::_DATE)->toDateTime()));

  int y = yMap.transform(_settings->toData(ChartObjectData::_PRICE)->toDouble());

  p->setPen(_settings->toData(ChartObjectData::_COLOR)->toColor());

  p->setFont(_settings->toData(ChartObjectData::_FONT)->toFont());

  p->drawText(x, y, _settings->toData(ChartObjectData::_TEXT)->toString());

  QFontMetrics fm = p->fontMetrics();

  _selectionArea.clear();

  _selectionArea.append(QRegion(x,
		                y - fm.height(),
		                fm.width(_settings->toData(ChartObjectData::_TEXT)->toString(), -1),
		                fm.height(),
		                QRegion::Rectangle));

  if (_selected)
  {
    _grabHandles.clear();

    _grabHandles.append(QRegion(x - _handleWidth - 1,
		                y - (fm.height() / 2),
		                _handleWidth,
		                _handleWidth,
		                QRegion::Rectangle));

    p->fillRect(x - _handleWidth - 1,
		y - (fm.height() / 2),
		_handleWidth,
		_handleWidth,
		_settings->toData(ChartObjectData::_COLOR)->toColor());
  }
}

void ChartObjectText::move (QPoint p)
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

      map = plot()->canvasMap(QwtPlot::yRight);
      _settings->set(ChartObjectData::_PRICE, new DataDouble(map.invTransform((double) p.y())));

      plot()->replot();

      QString s = _settings->toData(ChartObjectData::_DATE)->toString() + " " + _settings->toData(ChartObjectData::_PRICE)->toString();
      g_parent->statusBar()->showMessage(s);

      _modified++;
      break;
    }
    default:
      break;
  }
}

int ChartObjectText::create ()
{
  _status = _MOVE;
  setSelected(TRUE);
  g_parent->statusBar()->showMessage(QObject::tr("Place Text object..."));
  return 0;
}

int ChartObjectText::info (Message &info)
{
  info.insert(QObject::tr("Type"), _settings->toData(ChartObjectData::_TYPE)->toString());

  QDateTime dt = _settings->toData(ChartObjectData::_DATE)->toDateTime();
  info.insert("D", dt.toString("yyyy-MM-dd"));
  info.insert("T", dt.toString("HH:mm:ss"));

  info.insert(QObject::tr("Price"), _settings->toData(ChartObjectData::_PRICE)->toString());
  info.insert(QObject::tr("Text"), _settings->toData(ChartObjectData::_TEXT)->toString());

  return 0;
}
