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
#include "ChartObjectData.h"
#include "SettingDouble.h"
#include "SettingDateTime.h"
#include "SettingString.h"
#include "SettingInteger.h"
#include "SettingColor.h"
#include "SettingBool.h"

#include <QDebug>
#include <QPolygon>
#include <QStatusBar>
#include <qwt_plot.h>

ChartObjectTLine::ChartObjectTLine ()
{
  _fieldList << QObject::tr("Open") << QObject::tr("High") << QObject::tr("Low") << QObject::tr("Close");

  _settings->set(ChartObjectData::_TYPE, new SettingString(QString("TLine")));
  _settings->set(ChartObjectData::_DATE, new SettingDateTime(QDateTime::currentDateTime()));
  _settings->set(ChartObjectData::_DATE_2, new SettingDateTime(QDateTime::currentDateTime()));
  _settings->set(ChartObjectData::_PRICE, new SettingDouble(0));
  _settings->set(ChartObjectData::_PRICE_2, new SettingDouble(0));
  _settings->set(ChartObjectData::_COLOR, new SettingColor(QColor(Qt::red)));
  _settings->set(ChartObjectData::_Z, new SettingInteger(1));
  _settings->set(ChartObjectData::_PEN, new SettingInteger(1));
  _settings->set(ChartObjectData::_EXTEND, new SettingBool(FALSE));
}

void ChartObjectTLine::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings->get(ChartObjectData::_DATE)->toDateTime()));

  int x2 = xMap.transform(dsd->x(_settings->get(ChartObjectData::_DATE_2)->toDateTime()));

  int y = yMap.transform(_settings->get(ChartObjectData::_PRICE)->toDouble());
  int y2 = yMap.transform(_settings->get(ChartObjectData::_PRICE_2)->toDouble());

  p->setPen(_settings->get(ChartObjectData::_COLOR)->toColor());

  p->drawLine (x, y, x2, y2);

  // save old values;
  int tx2 = x2;
  int ty2 = y2;
  int tx = x;
  int ty = y;

  if (_settings->get(ChartObjectData::_EXTEND)->toBool())
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
		_settings->get(ChartObjectData::_COLOR)->toColor());

    _grabHandles.append(QRegion(tx2,
		                ty2 - (_handleWidth / 2),
		                _handleWidth,
		                _handleWidth,
		                QRegion::Rectangle));

    p->fillRect(tx2,
		ty2 - (_handleWidth / 2),
		_handleWidth,
		_handleWidth,
		_settings->get(ChartObjectData::_COLOR)->toColor());
  }
}

int ChartObjectTLine::info (Message &info)
{
  info.insert(QObject::tr("Type"), _settings->get(ChartObjectData::_TYPE)->toString());

  QDateTime dt = _settings->get(ChartObjectData::_DATE)->toDateTime();
  info.insert("SD", dt.toString("yyyy-MM-dd"));
  info.insert("ST", dt.toString("HH:mm:ss"));

  dt = _settings->get(ChartObjectData::_DATE_2)->toDateTime();
  info.insert("ED", dt.toString("yyyy-MM-dd"));
  info.insert("ET", dt.toString("HH:mm:ss"));

  info.insert("SP", _settings->get(ChartObjectData::_PRICE)->toString());
  info.insert("EP", _settings->get(ChartObjectData::_PRICE_2)->toString());

  return 0;
}

int ChartObjectTLine::highLow (int start, int end, double &high, double &low)
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  if (! dsd)
    return 1;

  int x = dsd->x(_settings->get(ChartObjectData::_DATE)->toDateTime());
  if (x >= start && x <= end)
  {
    high = _settings->get(ChartObjectData::_PRICE)->toDouble();
    double t = _settings->get(ChartObjectData::_PRICE_2)->toDouble();
    if (t > high)
      high = t;
    low = _settings->get(ChartObjectData::_PRICE)->toDouble();
    if (t < low)
      low = t;
    return 0;
  }

  int x2 = dsd->x(_settings->get(ChartObjectData::_DATE_2)->toDateTime());
  if (x2 >= start && x2 <= end)
  {
    high = _settings->get(ChartObjectData::_PRICE)->toDouble();
    double t = _settings->get(ChartObjectData::_PRICE_2)->toDouble();
    if (t > high)
      high = t;
    low = _settings->get(ChartObjectData::_PRICE)->toDouble();
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
      QwtScaleMap map = plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) p.x());

      DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
      QDateTime dt;
      dsd->date(x, dt);
      _settings->set(ChartObjectData::_DATE, new SettingDateTime(dt));

      map = plot()->canvasMap(QwtPlot::yRight);
      _settings->set(ChartObjectData::_PRICE, new SettingDouble(map.invTransform((double) p.y())));

      if (_createFlag)
      {
        _settings->set(ChartObjectData::_DATE_2, new SettingDateTime(dt));
        _settings->set(ChartObjectData::_PRICE_2, new SettingDouble(_settings->get(ChartObjectData::_PRICE)->toDouble()));
      }

      plot()->replot();

      QStringList l;
      l << _settings->get(ChartObjectData::_DATE)->toString();
      Strip strip;
      QString ts;
      strip.strip(_settings->get(ChartObjectData::_PRICE)->toDouble(), 4, ts);
      l << ts;
      g_parent->statusBar()->showMessage(l.join(" "));

      _modified++;
      break;
    }
    case _MOVE2:
    {
      QwtScaleMap map = plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) p.x());

      DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
      QDateTime dt;
      dsd->date(x, dt);
      _settings->set(ChartObjectData::_DATE_2, new SettingDateTime(dt));

      map = plot()->canvasMap(QwtPlot::yRight);
      _settings->set(ChartObjectData::_PRICE_2, new SettingDouble(map.invTransform((double) p.y())));

      plot()->replot();

      QStringList l;
      l << _settings->get(ChartObjectData::_DATE_2)->toString();
      Strip strip;
      QString ts;
      strip.strip(_settings->get(ChartObjectData::_PRICE_2)->toDouble(), 4, ts);
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
	    tplot->select(_settings->get(ChartObjectData::_ID)->toString());
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
