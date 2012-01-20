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
#include "GlobalParent.h"
#include "Strip.h"
#include "DateScaleDraw.h"
#include "Plot.h"
#include "KeyChartObject.h"

#include <QDebug>
#include <QPolygon>
#include <QSettings>
#include <QStatusBar>
#include <qwt_plot.h>

ChartObjectRetracement::ChartObjectRetracement ()
{
  _createFlag = 0;

  KeyChartObject keys;

  Data td(QString("Retracement"));
  _settings.set(keys.indexToString(KeyChartObject::_TYPE), td);
  
  td = Data(QDateTime::currentDateTime());
  td.setLabel(QObject::tr("Date Start"));
  _settings.set(keys.indexToString(KeyChartObject::_DATE), td);
  
  td.setLabel(QObject::tr("Date End"));
  _settings.set(keys.indexToString(KeyChartObject::_DATE2), td);
  
  td = Data(0.0);
  td.setLabel(QObject::tr("High"));
  _settings.set(keys.indexToString(KeyChartObject::_HIGH), td);
  
  td = Data(0.0);
  td.setLabel(QObject::tr("Low"));
  _settings.set(keys.indexToString(KeyChartObject::_LOW), td);
  
  td = Data(QColor(Qt::red));
  td.setLabel(QObject::tr("Color"));
  _settings.set(keys.indexToString(KeyChartObject::_COLOR), td);
  
  td = Data(FALSE);
  td.setLabel(QObject::tr("Extend"));
  _settings.set(keys.indexToString(KeyChartObject::_EXTEND), td);
  
  td = Data(0.382);
  td.setTab(1);
  td.setLabel(QObject::tr("Level 1"));
  _settings.set(keys.indexToString(KeyChartObject::_LEVEL1), td);
  
  td = Data(0.5);
  td.setTab(1);
  td.setLabel(QObject::tr("Level 2"));
  _settings.set(keys.indexToString(KeyChartObject::_LEVEL2), td);
  
  td = Data(0.618);
  td.setTab(1);
  td.setLabel(QObject::tr("Level 3"));
  _settings.set(keys.indexToString(KeyChartObject::_LEVEL3), td);

  td = Data(0);
  td.setTab(1);
  td.setLabel(QObject::tr("Level 4"));
  _settings.set(keys.indexToString(KeyChartObject::_LEVEL4), td);
  
  td = Data();
  td.setTab(1);
  td.setLabel(QObject::tr("Level 5"));
  _settings.set(keys.indexToString(KeyChartObject::_LEVEL5), td);
  
  td = Data();
  td.setTab(1);
  td.setLabel(QObject::tr("Level 6"));
  _settings.set(keys.indexToString(KeyChartObject::_LEVEL6), td);
}

void ChartObjectRetracement::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  KeyChartObject keys;
  Data date, date2, high, low, color, extend, l1, l2, l3, l4, l5, l6;
  _settings.toData(keys.indexToString(KeyChartObject::_DATE), date);
  _settings.toData(keys.indexToString(KeyChartObject::_DATE2), date2);
  _settings.toData(keys.indexToString(KeyChartObject::_HIGH), high);
  _settings.toData(keys.indexToString(KeyChartObject::_LOW), low);
  _settings.toData(keys.indexToString(KeyChartObject::_COLOR), color);
  _settings.toData(keys.indexToString(KeyChartObject::_EXTEND), extend);
  _settings.toData(keys.indexToString(KeyChartObject::_LEVEL1), l1);
  _settings.toData(keys.indexToString(KeyChartObject::_LEVEL2), l2);
  _settings.toData(keys.indexToString(KeyChartObject::_LEVEL3), l3);
  _settings.toData(keys.indexToString(KeyChartObject::_LEVEL4), l4);
  _settings.toData(keys.indexToString(KeyChartObject::_LEVEL5), l5);
  _settings.toData(keys.indexToString(KeyChartObject::_LEVEL6), l6);
  
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(date.toDateTime()));

  QDateTime dt = date2.toDateTime();
  if (extend.toBool())
    dsd->date(dsd->count() - 1, dt);

  int x2 = xMap.transform(dsd->x(dt));

  p->setPen(color.toColor());

  _selectionArea.clear();

  QList<double> lineList;
  lineList.append(l1.toDouble());
  lineList.append(l2.toDouble());
  lineList.append(l3.toDouble());
  lineList.append(l4.toDouble());
  lineList.append(l5.toDouble());
  lineList.append(l6.toDouble());

  int loop;
  for (loop = 0; loop < lineList.count(); loop++)
  {
    double td = lineList[loop];
    if (td != 0)
    {
      double range = high.toDouble() - low.toDouble();
      double r = 0;
      if (td < 0)
        r = low.toDouble() + (range * td);
      else
      {
        if (td > 0)
          r = low.toDouble() + (range * td);
        else
        {
          if (td < 0)
            r = high.toDouble();
          else
            r = low.toDouble();
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
  int y = yMap.transform(low.toDouble());
  p->drawLine (x, y, x2, y);
  Strip strip;
  QString ts;
  strip.strip(low.toDouble(), 4, ts);
  p->drawText(x, y - 1, "0% - " + ts);

  // store the selectable area the low line occupies
  QPolygon array;
  array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
  _selectionArea.append(QRegion(array));

  // draw the high line
  int y2 = yMap.transform(high.toDouble());
  p->drawLine (x, y2, x2, y2);
  strip.strip(high.toDouble(), 4, ts);
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
		color.toColor());

    //bottom right corner
    _grabHandles.append(QRegion(x2, y - (_handleWidth / 2),
		                _handleWidth,
		                _handleWidth,
		                QRegion::Rectangle));

    p->fillRect(x2,
		y - (_handleWidth / 2),
		_handleWidth,
		_handleWidth,
		color.toColor());
  }
}

int ChartObjectRetracement::info (Entity &info)
{
  KeyChartObject keys;
  Data type, date, date2, high, low, l1, l2, l3, l4, l5, l6;
  _settings.toData(keys.indexToString(KeyChartObject::_TYPE), type);
  _settings.toData(keys.indexToString(KeyChartObject::_DATE), date);
  _settings.toData(keys.indexToString(KeyChartObject::_DATE2), date2);
  _settings.toData(keys.indexToString(KeyChartObject::_HIGH), high);
  _settings.toData(keys.indexToString(KeyChartObject::_LOW), low);
  _settings.toData(keys.indexToString(KeyChartObject::_LEVEL1), l1);
  _settings.toData(keys.indexToString(KeyChartObject::_LEVEL2), l2);
  _settings.toData(keys.indexToString(KeyChartObject::_LEVEL3), l3);
  _settings.toData(keys.indexToString(KeyChartObject::_LEVEL4), l4);
  _settings.toData(keys.indexToString(KeyChartObject::_LEVEL5), l5);
  _settings.toData(keys.indexToString(KeyChartObject::_LEVEL6), l6);
  
  info.set(QObject::tr("Type"), type);

  QDateTime dt = date.toDateTime();
  info.set(QString("SD"), dt.toString("yyyy-MM-dd"));
  info.set(QString("ST"), dt.toString("HH:mm:ss"));

  dt = date2.toDateTime();
  info.set(QString("ED"), dt.toString("yyyy-MM-dd"));
  info.set(QString("ET"), dt.toString("HH:mm:ss"));

  info.set(QObject::tr("High"), high);
  info.set(QObject::tr("Low"), low);
  info.set(QObject::tr("Level 1"), l1);
  info.set(QObject::tr("Level 2"), l2);
  info.set(QObject::tr("Level 3"), l3);
  info.set(QObject::tr("Level 4"), l4);
  info.set(QObject::tr("Level 5"), l5);
  info.set(QObject::tr("Level 6"), l6);

  return 0;
}

int ChartObjectRetracement::highLow (int start, int end, double &rhigh, double &rlow)
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  if (! dsd)
    return 1;

  KeyChartObject keys;
  Data date, date2, high, low;
  _settings.toData(keys.indexToString(KeyChartObject::_DATE), date);
  _settings.toData(keys.indexToString(KeyChartObject::_DATE2), date2);
  _settings.toData(keys.indexToString(KeyChartObject::_HIGH), high);
  _settings.toData(keys.indexToString(KeyChartObject::_LOW), low);
  
  int x = dsd->x(date.toDateTime());
  if (x >= start && x <= end)
  {
    rhigh = high.toDouble();
    rlow = low.toDouble();
    return 0;
  }

  int x2 = dsd->x(date2.toDateTime());
  if (x2 >= start && x2 <= end)
  {
    rhigh = high.toDouble();
    rlow = low.toDouble();
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
      KeyChartObject keys;
      Data date, date2, high, low;
      _settings.toData(keys.indexToString(KeyChartObject::_DATE), date);
      _settings.toData(keys.indexToString(KeyChartObject::_DATE2), date2);
      _settings.toData(keys.indexToString(KeyChartObject::_HIGH), high);
      _settings.toData(keys.indexToString(KeyChartObject::_LOW), low);
      
      QwtScaleMap map = plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) p.x());

      DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
      QDateTime dt;
      dsd->date(x, dt);
      date.set(dt);
      _settings.set(keys.indexToString(KeyChartObject::_DATE), date);

      map = plot()->canvasMap(QwtPlot::yRight);
      high.set(map.invTransform((double) p.y()));
      _settings.set(keys.indexToString(KeyChartObject::_HIGH), high);

      if (_createFlag)
      {
	date2.set(dt);
        _settings.set(keys.indexToString(KeyChartObject::_DATE2), date2);
	
	low.set(high.toDouble());
        _settings.set(keys.indexToString(KeyChartObject::_LOW), low);
      }

      plot()->replot();

      QStringList l;
      l << date.toString();
      Strip strip;
      QString ts;
      strip.strip(high.toDouble(), 4, ts);
      l << ts;
      g_parent->statusBar()->showMessage(l.join(" "));

      _modified++;
      break;
    }
    case _MOVE2:
    {
      KeyChartObject keys;
      Data date2, low;
      _settings.toData(keys.indexToString(KeyChartObject::_DATE2), date2);
      _settings.toData(keys.indexToString(KeyChartObject::_LOW), low);

      QwtScaleMap map = plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) p.x());

      DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
      QDateTime dt;
      dsd->date(x, dt);
      date2.set(dt);
      _settings.set(keys.indexToString(KeyChartObject::_DATE2), date2);

      map = plot()->canvasMap(QwtPlot::yRight);
      low.set(map.invTransform((double) p.y()));
      _settings.set(keys.indexToString(KeyChartObject::_LOW), low);

      plot()->replot();

      QStringList l;
      l << date2.toString();
      Strip strip;
      QString ts;
      strip.strip(low.toDouble(), 4, ts);
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

int ChartObjectRetracement::create ()
{
  _status = _MOVE;
  _createFlag = 1;
  setSelected(TRUE);
  g_parent->statusBar()->showMessage(QObject::tr("Select highest starting point..."));
  return 0;
}
