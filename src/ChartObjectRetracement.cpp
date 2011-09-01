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


#include "ChartObjectRetracement.h"
#include "Globals.h"
#include "Strip.h"
#include "DateScaleDraw.h"
#include "Plot.h"
#include "ChartObjectData.h"

#include <QDebug>
#include <QPolygon>
#include <QSettings>
#include <QStatusBar>
#include <qwt_plot.h>

ChartObjectRetracement::ChartObjectRetracement ()
{
  _createFlag = 0;

  _settings->set(ChartObjectData::_TYPE, QString("Retracement"));
  _settings->set(ChartObjectData::_DATE, QDateTime::currentDateTime());
  _settings->set(ChartObjectData::_DATE2, QDateTime::currentDateTime());
  _settings->set(ChartObjectData::_HIGH, 0);
  _settings->set(ChartObjectData::_LOW, 0);
  _settings->set(ChartObjectData::_COLOR, QColor(Qt::red));
  _settings->set(ChartObjectData::_Z, 1);
  _settings->set(ChartObjectData::_PEN, 1);
  _settings->set(ChartObjectData::_EXTEND, FALSE);
  _settings->set(ChartObjectData::_LINE1, 0.382);
  _settings->set(ChartObjectData::_LINE2, 0.5);
  _settings->set(ChartObjectData::_LINE3, 0.618);
  _settings->set(ChartObjectData::_LINE4, 0);
  _settings->set(ChartObjectData::_LINE5, 0);
  _settings->set(ChartObjectData::_LINE6, 0);
}

void ChartObjectRetracement::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings->getDateTime(ChartObjectData::_DATE)));

  QDateTime dt = _settings->getDateTime(ChartObjectData::_DATE2);
  if (_settings->getBool(ChartObjectData::_EXTEND))
    dsd->date(dsd->count() - 1, dt);

  int x2 = xMap.transform(dsd->x(dt));

  p->setPen(_settings->getColor(ChartObjectData::_COLOR));

  _selectionArea.clear();

  QList<double> lineList;
  lineList.append(_settings->getDouble(ChartObjectData::_LINE1));
  lineList.append(_settings->getDouble(ChartObjectData::_LINE2));
  lineList.append(_settings->getDouble(ChartObjectData::_LINE3));
  lineList.append(_settings->getDouble(ChartObjectData::_LINE4));
  lineList.append(_settings->getDouble(ChartObjectData::_LINE5));
  lineList.append(_settings->getDouble(ChartObjectData::_LINE6));

  int loop;
  for (loop = 0; loop < lineList.count(); loop++)
  {
    double td = lineList[loop];
    if (td != 0)
    {
      double range = _settings->getDouble(ChartObjectData::_HIGH) - _settings->getDouble(ChartObjectData::_LOW);
      double r = 0;
      if (td < 0)
        r = _settings->getDouble(ChartObjectData::_LOW) + (range * td);
      else
      {
        if (td > 0)
          r = _settings->getDouble(ChartObjectData::_LOW) + (range * td);
        else
        {
          if (td < 0)
            r = _settings->getDouble(ChartObjectData::_HIGH);
          else
            r = _settings->getDouble(ChartObjectData::_LOW);
        }
      }

      int y = yMap.transform(r);
      p->drawLine (x, y, x2, y);
      p->drawText(x, y - 1, QString::number(td * 100) + "% - " + QString::number(r));

      QPolygon array;
      array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
      _selectionArea.append(QRegion(array));
    }
  }

  // draw the low line
  int y = yMap.transform(_settings->getDouble(ChartObjectData::_LOW));
  p->drawLine (x, y, x2, y);
  Strip strip;
  QString ts;
  strip.strip(_settings->getDouble(ChartObjectData::_LOW), 4, ts);
  p->drawText(x, y - 1, "0% - " + ts);

  // store the selectable area the low line occupies
  QPolygon array;
  array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
  _selectionArea.append(QRegion(array));

  // draw the high line
  int y2 = yMap.transform(_settings->getDouble(ChartObjectData::_HIGH));
  p->drawLine (x, y2, x2, y2);
  strip.strip(_settings->getDouble(ChartObjectData::_HIGH), 4, ts);
  p->drawText(x, y2 - 1, "100% - " + ts);

  // store the selectable area the high line occupies
  array.clear();
  array.putPoints(0, 4, x, y2 - 4, x, y2 + 4, x2, y2 + 4, x2, y2 - 4);
  _selectionArea.append(QRegion(array));

  if (_selected)
  {
    _grabHandles.clear();

    //top left corner
    _grabHandles.append(QRegion(x, y2 - (_handleWidth / 2),
		                _handleWidth,
		                _handleWidth,
		                QRegion::Rectangle));

    p->fillRect(x,
		y2 - (_handleWidth / 2),
		_handleWidth,
		_handleWidth,
		_settings->getColor(ChartObjectData::_COLOR));

    //bottom right corner
    _grabHandles.append(QRegion(x2, y - (_handleWidth / 2),
		                _handleWidth,
		                _handleWidth,
		                QRegion::Rectangle));

    p->fillRect(x2,
		y - (_handleWidth / 2),
		_handleWidth,
		_handleWidth,
		_settings->getColor(ChartObjectData::_COLOR));
  }
}

int ChartObjectRetracement::info (Message &info)
{
  info.insert(QObject::tr("Type"), _settings->get(ChartObjectData::_TYPE));

  QDateTime dt = _settings->getDateTime(ChartObjectData::_DATE);
  info.insert("SD", dt.toString("yyyy-MM-dd"));
  info.insert("ST", dt.toString("HH:mm:ss"));

  dt = _settings->getDateTime(ChartObjectData::_DATE2);
  info.insert("ED", dt.toString("yyyy-MM-dd"));
  info.insert("ET", dt.toString("HH:mm:ss"));

  info.insert(QObject::tr("High"), _settings->get(ChartObjectData::_HIGH));
  info.insert(QObject::tr("Low"), _settings->get(ChartObjectData::_LOW));
  info.insert(QObject::tr("Level 1"), _settings->get(ChartObjectData::_LINE1));
  info.insert(QObject::tr("Level 2"), _settings->get(ChartObjectData::_LINE2));
  info.insert(QObject::tr("Level 3"), _settings->get(ChartObjectData::_LINE3));
  info.insert(QObject::tr("Level 4"), _settings->get(ChartObjectData::_LINE4));
  info.insert(QObject::tr("Level 5"), _settings->get(ChartObjectData::_LINE5));
  info.insert(QObject::tr("Level 6"), _settings->get(ChartObjectData::_LINE6));

  return 0;
}

int ChartObjectRetracement::highLow (int start, int end, double &high, double &low)
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  if (! dsd)
    return 1;

  int x = dsd->x(_settings->getDateTime(ChartObjectData::_DATE));
  if (x >= start && x <= end)
  {
    high = _settings->getDouble(ChartObjectData::_HIGH);
    low = _settings->getDouble(ChartObjectData::_LOW);
    return 0;
  }

  int x2 = dsd->x(_settings->getDateTime(ChartObjectData::_DATE2));
  if (x2 >= start && x2 <= end)
  {
    high = _settings->getDouble(ChartObjectData::_HIGH);
    low = _settings->getDouble(ChartObjectData::_LOW);
    return 0;
  }

  return 1;
}

void ChartObjectRetracement::move (QPoint p)
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
      _settings->set(ChartObjectData::_HIGH, map.invTransform((double) p.y()));

      if (_createFlag)
      {
        _settings->set(ChartObjectData::_DATE2, dt);
        _settings->set(ChartObjectData::_LOW, _settings->getDouble(ChartObjectData::_HIGH));
      }

      plot()->replot();

      QStringList l;
      l << _settings->get(ChartObjectData::_DATE);
      Strip strip;
      QString ts;
      strip.strip(_settings->getDouble(ChartObjectData::_HIGH), 4, ts);
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
      _settings->set(ChartObjectData::_DATE2, dt);

      map = plot()->canvasMap(QwtPlot::yRight);
      _settings->set(ChartObjectData::_LOW, map.invTransform((double) p.y()));

      plot()->replot();

      QStringList l;
      l << _settings->get(ChartObjectData::_DATE2);
      Strip strip;
      QString ts;
      strip.strip(_settings->getDouble(ChartObjectData::_LOW), 4, ts);
      l << ts;
      g_parent->statusBar()->showMessage(l.join(" "));

      _modified++;
      break;
    }
    default:
      break;
  }
}

void ChartObjectRetracement::click (int button, QPoint p)
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
            g_parent->statusBar()->showMessage(QObject::tr("Select lowest ending point..."));
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
	    tplot->select(_settings->get(ChartObjectData::_ID));

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

int ChartObjectRetracement::create ()
{
  _status = _MOVE;
  _createFlag = 1;
  setSelected(TRUE);
  g_parent->statusBar()->showMessage(QObject::tr("Select highest starting point..."));
  return 0;
}
