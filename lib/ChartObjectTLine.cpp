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
#include "Globals.h"
#include "Strip.h"
#include "SettingDateTime.h"
#include "SettingInteger.h"
#include "SettingDouble.h"
#include "SettingColor.h"
#include "SettingBool.h"

#include <QDebug>
#include <QPolygon>
#include <QSettings>
#include <QStatusBar>
#include <qwt_plot.h>

ChartObjectTLine::ChartObjectTLine ()
{
  _type = "TLine";
  _fieldList << QObject::tr("Open") << QObject::tr("High") << QObject::tr("Low") << QObject::tr("Close");

  SettingDateTime *dt = new SettingDateTime(QDateTime::currentDateTime());
  dt->setKey("DATE");
  _settings->set(dt);

  dt = new SettingDateTime(QDateTime::currentDateTime());
  dt->setKey("DATE_2");
  _settings->set(dt);

  SettingDouble *sd = new SettingDouble(0.0);
  sd->setKey("PRICE");
  _settings->set(sd);

  sd = new SettingDouble(0.0);
  sd->setKey("PRICE_2");
  _settings->set(sd);

  SettingBool *sb = new SettingBool(FALSE);
  sb->setKey("EXTEND");
  _settings->set(sb);

  SettingColor *sc = new SettingColor(QColor(Qt::red));
  sc->setKey("COLOR");
  _settings->set(sc);

  SettingInteger *si = new SettingInteger(0, 0, 1, 99, 0);
  si->setKey("Z");
  _settings->set(si);
}

void ChartObjectTLine::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings->get("DATE")->getDateTime()));

  int x2 = xMap.transform(dsd->x(_settings->get("DATE_2")->getDateTime()));

  int y = yMap.transform(_settings->get("PRICE")->getDouble());
  int y2 = yMap.transform(_settings->get("PRICE_2")->getDouble());

  p->setPen(_settings->get("COLOR")->getColor());

  p->drawLine (x, y, x2, y2);

  // save old values;
  int tx2 = x2;
  int ty2 = y2;
  int tx = x;
  int ty = y;

  if (_settings->get("EXTEND")->getBool())
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
		_settings->get("COLOR")->getColor());

    _grabHandles.append(QRegion(tx2,
		                ty2 - (_handleWidth / 2),
		                _handleWidth,
		                _handleWidth,
		                QRegion::Rectangle));

    p->fillRect(tx2,
		ty2 - (_handleWidth / 2),
		_handleWidth,
		_handleWidth,
		_settings->get("COLOR")->getColor());
  }
}

int ChartObjectTLine::info (Message &info)
{
  info.insert(QObject::tr("Type"), _type);

  QDateTime dt = _settings->get("DATE")->getDateTime();
  info.insert("SD", dt.toString("yyyy-MM-dd"));
  info.insert("ST", dt.toString("HH:mm:ss"));

  dt = _settings->get("DATE_2")->getDateTime();
  info.insert("ED", dt.toString("yyyy-MM-dd"));
  info.insert("ET", dt.toString("HH:mm:ss"));

  info.insert("SP", _settings->get("PRICE")->toString());
  info.insert("EP", _settings->get("PRICE_2")->toString());

  return 0;
}

int ChartObjectTLine::highLow (int start, int end, double &high, double &low)
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  if (! dsd)
    return 1;

  int x = dsd->x(_settings->get("DATE")->getDateTime());
  if (x >= start && x <= end)
  {
    high = _settings->get("PRICE")->getDouble();
    double t = _settings->get("PRICE_2")->getDouble();
    if (t > high)
      high = t;
    low = _settings->get("PRICE")->getDouble();
    if (t < low)
      low = t;
    return 0;
  }

  int x2 = dsd->x(_settings->get("DATE_2")->getDateTime());
  if (x2 >= start && x2 <= end)
  {
    high = _settings->get("PRICE")->getDouble();
    double t = _settings->get("PRICE_2")->getDouble();
    if (t > high)
      high = t;
    low = _settings->get("PRICE")->getDouble();
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

      Setting *set = _settings->get("DATE");
      if (! set)
	break;
      set->setDateTime(dt);

      map = plot()->canvasMap(QwtPlot::yRight);
      set = _settings->get("PRICE");
      if (! set)
	break;
      set->setDouble(map.invTransform((double) p.y()));

      if (_createFlag)
      {
        set = _settings->get("DATE_2");
        if (! set)
	  break;
        set->setDateTime(dt);

        set = _settings->get("PRICE_2");
        if (! set)
	  break;
        set->setDouble(_settings->get("PRICE")->getDouble());
      }

      plot()->replot();

      QStringList l;
      l << _settings->get("DATE")->toString();
      Strip strip;
      QString ts;
      strip.strip(_settings->get("PRICE")->getDouble(), 4, ts);
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

      Setting *set = _settings->get("DATE_2");
      if (! set)
	break;
      set->setDateTime(dt);

      map = plot()->canvasMap(QwtPlot::yRight);
      set = _settings->get("PRICE_2");
      if (! set)
	break;
      set->setDouble(map.invTransform((double) p.y()));

      plot()->replot();

      QStringList l;
      l << _settings->get("DATE_2")->toString();
      Strip strip;
      QString ts;
      strip.strip(_settings->get("PRICE_2")->getDouble(), 4, ts);
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
	    tplot->select(_id);
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
