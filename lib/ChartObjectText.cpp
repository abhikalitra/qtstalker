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
#include "ChartObjectKey.h"

#include <QDebug>
#include <QPolygon>
#include <QStatusBar>
#include <qwt_plot.h>

ChartObjectText::ChartObjectText ()
{
  ChartObjectKey keys;
  
  Data td(QString("Text"));
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
  
  td = Data(QString("Text"));
  td.setLabel(QObject::tr("Text"));
  _settings.set(keys.indexToString(ChartObjectKey::_TEXT), td);
  
  QFont f;
  td = Data(f);
  td.setLabel(QObject::tr("Font"));
  _settings.set(keys.indexToString(ChartObjectKey::_FONT), td);
}

void ChartObjectText::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  ChartObjectKey keys;
  Data date, price, color, font, text;
  _settings.toData(keys.indexToString(ChartObjectKey::_DATE), date);
  _settings.toData(keys.indexToString(ChartObjectKey::_PRICE), price);
  _settings.toData(keys.indexToString(ChartObjectKey::_COLOR), color);
  _settings.toData(keys.indexToString(ChartObjectKey::_FONT), font);
  _settings.toData(keys.indexToString(ChartObjectKey::_TEXT), text);

  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(date.toDateTime()));

  int y = yMap.transform(price.toDouble());

  p->setPen(color.toColor());

  p->setFont(font.toFont());

  p->drawText(x, y, text.toString());

  QFontMetrics fm = p->fontMetrics();

  _selectionArea.clear();

  _selectionArea.append(QRegion(x,
		                y - fm.height(),
		                fm.width(text.toString(), -1),
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
		color.toColor());
  }
}

int ChartObjectText::info (Entity &info)
{
  ChartObjectKey keys;
  Data type, date, price, text;
  _settings.toData(keys.indexToString(ChartObjectKey::_TYPE), type);
  _settings.toData(keys.indexToString(ChartObjectKey::_DATE), date);
  _settings.toData(keys.indexToString(ChartObjectKey::_PRICE), price);
  _settings.toData(keys.indexToString(ChartObjectKey::_TEXT), text);
  
  info.set(QObject::tr("Type"), type);

  info.set(QObject::tr("Price"), price);

  info.set(QObject::tr("Text"), text);
  
  QDateTime dt = date.toDateTime();
  info.set(QString("D"), Data(dt.toString("yyyy-MM-dd")));
  info.set(QString("T"), Data(dt.toString("HH:mm:ss")));

  return 0;
}

void ChartObjectText::move (QPoint p)
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

int ChartObjectText::create ()
{
  _status = _MOVE;
  setSelected(TRUE);
  g_parent->statusBar()->showMessage(QObject::tr("Place Text object..."));
  return 0;
}
