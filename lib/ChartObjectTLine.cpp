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


#include "ChartObjectTLine.h"
#include "GlobalParent.h"
#include "Strip.h"
#include "DateScaleDraw.h"
#include "Plot.h"
#include "KeyChartObject.h"

#include <QDebug>
#include <QPolygon>
#include <QStatusBar>
#include <qwt_plot.h>

ChartObjectTLine::ChartObjectTLine ()
{
  _fieldList << QObject::tr("Open") << QObject::tr("High") << QObject::tr("Low") << QObject::tr("Close");
  
  KeyChartObject keys;
  
  Data td(QString("TLine"));
  _settings.set(keys.indexToString(KeyChartObject::_TYPE), td);
  
  td = Data(QDateTime::currentDateTime());
  td.setLabel(QObject::tr("Date Start"));
  _settings.set(keys.indexToString(KeyChartObject::_DATE), td);
  
  td.setLabel(QObject::tr("Date End"));
  _settings.set(keys.indexToString(KeyChartObject::_DATE2), td);
  
  td = Data(0.0);
  td.setLabel(QObject::tr("Price Start"));
  _settings.set(keys.indexToString(KeyChartObject::_PRICE), td);
  
  td.setLabel(QObject::tr("Price End"));
  _settings.set(keys.indexToString(KeyChartObject::_PRICE2), td);
  
  td = Data(QColor(Qt::red));
  td.setLabel(QObject::tr("Color"));
  _settings.set(keys.indexToString(KeyChartObject::_COLOR), td);
  
  td = Data(FALSE);
  td.setLabel(QObject::tr("Extend"));
  _settings.set(keys.indexToString(KeyChartObject::_EXTEND), td);
}

void ChartObjectTLine::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  KeyChartObject keys;
  Data date, date2, price, price2, color, extend;
  _settings.toData(keys.indexToString(KeyChartObject::_DATE), date);
  _settings.toData(keys.indexToString(KeyChartObject::_DATE2), date2);
  _settings.toData(keys.indexToString(KeyChartObject::_PRICE), price);
  _settings.toData(keys.indexToString(KeyChartObject::_PRICE2), price2);
  _settings.toData(keys.indexToString(KeyChartObject::_COLOR), color);
  _settings.toData(keys.indexToString(KeyChartObject::_EXTEND), extend);

  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(date.toDateTime()));

  int x2 = xMap.transform(dsd->x(date2.toDateTime()));

  int y = yMap.transform(price.toDouble());
  int y2 = yMap.transform(price2.toDouble());

  p->setPen(color.toColor());

  p->drawLine (x, y, x2, y2);

  // save old values;
  int tx2 = x2;
  int ty2 = y2;
  int tx = x;
  int ty = y;

  if (extend.toBool())
  {
    int ydiff = y - y2;
    int xdiff = x2 - x;
    if (xdiff > 0)
    {
      while (x2 < p->window().width())
      {
        x = x2;
        y = y2;
        x2 = x2 + xdiff;
        y2 = y2 - ydiff;
        p->drawLine (x, y, x2, y2);
      }
    }
  }

  // store the selectable area the line occupies
  _selectionArea.clear();

  QPolygon array;
//  array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y2 + 4, x2, y2 - 4);
  array.putPoints(0, 4, tx, ty - 4, tx, ty + 4, x2, y2 + 4, x2, y2 - 4);

  _selectionArea.append(QRegion(array));

  if (_selected)
  {
    _grabHandles.clear();

    _grabHandles.append(QRegion(tx,
		                ty - (_handleWidth / 2),
		                _handleWidth,
		                _handleWidth,
		                QRegion::Rectangle));

    p->fillRect(tx,
		ty - (_handleWidth / 2),
		_handleWidth,
		_handleWidth,
		color.toColor());

    _grabHandles.append(QRegion(tx2,
		                ty2 - (_handleWidth / 2),
		                _handleWidth,
		                _handleWidth,
		                QRegion::Rectangle));

    p->fillRect(tx2,
		ty2 - (_handleWidth / 2),
		_handleWidth,
		_handleWidth,
		color.toColor());
  }
}

int ChartObjectTLine::info (Entity &info)
{
  KeyChartObject keys;
  Data type, date, date2, price, price2;
  _settings.toData(keys.indexToString(KeyChartObject::_TYPE), type);
  _settings.toData(keys.indexToString(KeyChartObject::_DATE), date);
  _settings.toData(keys.indexToString(KeyChartObject::_DATE2), date2);
  _settings.toData(keys.indexToString(KeyChartObject::_PRICE), price);
  _settings.toData(keys.indexToString(KeyChartObject::_PRICE2), price2);
  
  info.set(QObject::tr("Type"), type);

  info.set(QObject::tr("SP"), price);

  info.set(QObject::tr("EP"), price2);
  
  QDateTime dt = date.toDateTime();
  info.set(QString("SD"), Data(dt.toString("yyyy-MM-dd")));
  info.set(QString("ST"), Data(dt.toString("HH:mm:ss")));
  
  dt = date2.toDateTime();
  info.set(QString("ED"), Data(dt.toString("yyyy-MM-dd")));
  info.set(QString("ET"), Data(dt.toString("HH:mm:ss")));

  return 0;
}

int ChartObjectTLine::highLow (int start, int end, double &high, double &low)
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  if (! dsd)
    return 1;

  KeyChartObject keys;
  Data date, date2, price, price2;
  _settings.toData(keys.indexToString(KeyChartObject::_DATE), date);
  _settings.toData(keys.indexToString(KeyChartObject::_DATE2), date2);
  _settings.toData(keys.indexToString(KeyChartObject::_PRICE), price);
  _settings.toData(keys.indexToString(KeyChartObject::_PRICE2), price2);
  
  int x = dsd->x(date.toDateTime());
  if (x >= start && x <= end)
  {
    high = price.toDouble();
    double t = price2.toDouble();
    if (t > high)
      high = t;
    low = price.toDouble();
    if (t < low)
      low = t;
    return 0;
  }

  int x2 = dsd->x(date2.toDateTime());
  if (x2 >= start && x2 <= end)
  {
    high = price.toDouble();
    double t = price2.toDouble();
    if (t > high)
      high = t;
    low = price.toDouble();
    if (t < low)
      low = t;
    return 0;
  }

  return 1;
}

void ChartObjectTLine::move (QPoint p)
{
  switch (_status)
  {
    case _MOVE:
    {
      KeyChartObject keys;
      Data date, date2, price, price2;
      _settings.toData(keys.indexToString(KeyChartObject::_DATE), date);
      _settings.toData(keys.indexToString(KeyChartObject::_DATE2), date2);
      _settings.toData(keys.indexToString(KeyChartObject::_PRICE), price);
      _settings.toData(keys.indexToString(KeyChartObject::_PRICE2), price2);
      
      QwtScaleMap map = plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) p.x());

      DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
      QDateTime dt;
      dsd->date(x, dt);
      date.set(dt);
      _settings.set(keys.indexToString(KeyChartObject::_DATE), date);

      map = plot()->canvasMap(QwtPlot::yRight);
      price.set(map.invTransform((double) p.y()));
      _settings.set(keys.indexToString(KeyChartObject::_PRICE), price);

      if (_createFlag)
      {
	date2.set(dt);
        _settings.set(keys.indexToString(KeyChartObject::_DATE2), date2);
	
	price2.set(price.toDouble());
        _settings.set(keys.indexToString(KeyChartObject::_PRICE2), price2);
      }

      plot()->replot();

      QStringList l;
      l << date.toString();
      Strip strip;
      QString ts;
      strip.strip(price.toDouble(), 4, ts);
      l << ts;
      g_parent->statusBar()->showMessage(l.join(" "));

      _modified++;
      break;
    }
    case _MOVE2:
    {
      KeyChartObject keys;
      Data date2, price2;
      _settings.toData(keys.indexToString(KeyChartObject::_DATE2), date2);
      _settings.toData(keys.indexToString(KeyChartObject::_PRICE2), price2);

      QwtScaleMap map = plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) p.x());

      DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
      QDateTime dt;
      dsd->date(x, dt);
      date2.set(dt);
      _settings.set(keys.indexToString(KeyChartObject::_DATE2), date2);

      map = plot()->canvasMap(QwtPlot::yRight);
      price2.set(map.invTransform((double) p.y()));
      _settings.set(keys.indexToString(KeyChartObject::_PRICE2), price2);

      plot()->replot();

      QStringList l;
      l << date2.toString();
      Strip strip;
      QString ts;
      strip.strip(price2.toDouble(), 4, ts);
      l << ts;
      g_parent->statusBar()->showMessage(l.join(" "));

      _modified++;
      break;
    }
    default:
      break;
  }
}

void ChartObjectTLine::click (int button, QPoint p)
{
  switch (_status)
  {
    case _SELECTED:
    {
      switch (button)
      {
        case Qt::LeftButton:
        {
          int grab = isGrabSelected(p);
          if (grab)
          {
            _status = _MOVE;
            if (grab == 2)
              _status = _MOVE2;
            return;
          }

          if (! isSelected(p))
          {
            _status = _NONE;
            setSelected(FALSE);
	    Plot *tplot = (Plot *) plot();
	    tplot->unselect();
            plot()->replot();
            return;
          }
          break;
        }
        case Qt::RightButton:
	{
          Plot *p = (Plot *) plot();
	  p->showChartObjectMenu();
          break;
	}
        default:
          break;
      }

      break;
    }
    case _MOVE:
    {
      switch (button)
      {
        case Qt::LeftButton:
          if (_createFlag)
          {
            _status = _MOVE2;
            g_parent->statusBar()->showMessage(QObject::tr("Select TLine ending point..."));
            return;
          }

          _status = _SELECTED;
          return;
        default:
          break;
      }

      break;
    }
    case _MOVE2:
    {
      switch (button)
      {
        case Qt::LeftButton:
          _status = _SELECTED;
          _createFlag = 0;
          return;
        default:
          break;
      }

      break;
    }
    default: // _None
    {
      switch (button)
      {
        case Qt::LeftButton:
	{
          if (isSelected(p))
          {
            _status = _SELECTED;
            _selected = 1;
	    Plot *tplot = (Plot *) plot();
	    
	    KeyChartObject keys;
	    Data id;
	    _settings.toData(keys.indexToString(KeyChartObject::_ID), id);
	    tplot->select(id.toString());
            plot()->replot();
            return;
          }
          break;
	}
        default:
          break;
      }

      break;
    }
  }
}

int ChartObjectTLine::create ()
{
  _createFlag = 1;
  _status = _MOVE;
  setSelected(TRUE);
  g_parent->statusBar()->showMessage(QObject::tr("Select TLine starting point..."));
  return 0;
}
