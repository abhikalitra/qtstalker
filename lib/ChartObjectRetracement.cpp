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

ChartObjectRetracement::ChartObjectRetracement ()
{
  _type = "Retracement";
  _createFlag = 0;

  SettingDateTime *dt = new SettingDateTime(QDateTime::currentDateTime());
  dt->setKey("DATE");
  _settings->set(dt);

  dt = new SettingDateTime(QDateTime::currentDateTime());
  dt->setKey("DATE_2");
  _settings->set(dt);

  SettingDouble *sd = new SettingDouble(0);
  sd->setKey("HIGH");
  _settings->set(sd);

  sd = new SettingDouble(0);
  sd->setKey("LOW");
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

  sd = new SettingDouble(0.382);
  sd->setKey("LINE_1");
  _settings->set(sd);

  sd = new SettingDouble(0.5);
  sd->setKey("LINE_2");
  _settings->set(sd);

  sd = new SettingDouble(0.618);
  sd->setKey("LINE_3");
  _settings->set(sd);

  sd = new SettingDouble(0.0);
  sd->setKey("LINE_4");
  _settings->set(sd);

  sd = new SettingDouble(0.0);
  sd->setKey("LINE_5");
  _settings->set(sd);

  sd = new SettingDouble(0.0);
  sd->setKey("LINE_6");
  _settings->set(sd);
}

void ChartObjectRetracement::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings->get("DATE")->getDateTime()));

  QDateTime dt = _settings->get("DATE_2")->getDateTime();
  if (_settings->get("EXTEND")->getBool())
    dsd->date(dsd->count() - 1, dt);

  int x2 = xMap.transform(dsd->x(dt));

  p->setPen(_settings->get("COLOR")->getColor());

  _selectionArea.clear();

  QList<double> lineList;
  lineList.append(_settings->get("LINE_1")->getDouble());
  lineList.append(_settings->get("LINE_2")->getDouble());
  lineList.append(_settings->get("LINE_3")->getDouble());
  lineList.append(_settings->get("LINE_4")->getDouble());
  lineList.append(_settings->get("LINE_5")->getDouble());
  lineList.append(_settings->get("LINE_6")->getDouble());

  int loop;
  for (loop = 0; loop < lineList.count(); loop++)
  {
    double td = lineList[loop];
    if (td != 0)
    {
      double range = _settings->get("HIGH")->getDouble() - _settings->get("LOW")->getDouble();
      double r = 0;
      if (td < 0)
        r = _settings->get("LOW")->getDouble() + (range * td);
      else
      {
        if (td > 0)
          r = _settings->get("LOW")->getDouble() + (range * td);
        else
        {
          if (td < 0)
            r = _settings->get("HIGH")->getDouble();
          else
            r = _settings->get("LOW")->getDouble();
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
  int y = yMap.transform(_settings->get("LOW")->getDouble());
  p->drawLine (x, y, x2, y);
  Strip strip;
  QString ts;
  strip.strip(_settings->get("LOW")->getDouble(), 4, ts);
  p->drawText(x, y - 1, "0% - " + ts);

  // store the selectable area the low line occupies
  QPolygon array;
  array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
  _selectionArea.append(QRegion(array));

  // draw the high line
  int y2 = yMap.transform(_settings->get("HIGH")->getDouble());
  p->drawLine (x, y2, x2, y2);
  strip.strip(_settings->get("HIGH")->getDouble(), 4, ts);
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
		_settings->get("COLOR")->getColor());

    //bottom right corner
    _grabHandles.append(QRegion(x2, y - (_handleWidth / 2),
		                _handleWidth,
		                _handleWidth,
		                QRegion::Rectangle));

    p->fillRect(x2,
		y - (_handleWidth / 2),
		_handleWidth,
		_handleWidth,
		_settings->get("COLOR")->getColor());
  }
}

int ChartObjectRetracement::info (Message &info)
{
  info.insert(QObject::tr("Type"), _type);

  QDateTime dt = _settings->get("DATE")->getDateTime();
  info.insert("SD", dt.toString("yyyy-MM-dd"));
  info.insert("ST", dt.toString("HH:mm:ss"));

  dt = _settings->get("DATE_2")->getDateTime();
  info.insert("ED", dt.toString("yyyy-MM-dd"));
  info.insert("ET", dt.toString("HH:mm:ss"));

  info.insert(QObject::tr("High"), _settings->get("HIGH")->toString());
  info.insert(QObject::tr("Low"), _settings->get("LOW")->toString());
  info.insert(QObject::tr("Level 1"), _settings->get("LINE_1")->toString());
  info.insert(QObject::tr("Level 2"), _settings->get("LINE_2")->toString());
  info.insert(QObject::tr("Level 3"), _settings->get("LINE_3")->toString());
  info.insert(QObject::tr("Level 4"), _settings->get("LINE_4")->toString());
  info.insert(QObject::tr("Level 5"), _settings->get("LINE_5")->toString());
  info.insert(QObject::tr("Level 6"), _settings->get("LINE_6")->toString());

  return 0;
}

int ChartObjectRetracement::highLow (int start, int end, double &high, double &low)
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  if (! dsd)
    return 1;

  int x = dsd->x(_settings->get("DATE")->getDateTime());
  if (x >= start && x <= end)
  {
    high = _settings->get("HIGH")->getDouble();
    low = _settings->get("LOW")->getDouble();
    return 0;
  }

  int x2 = dsd->x(_settings->get("DATE_2")->getDateTime());
  if (x2 >= start && x2 <= end)
  {
    high = _settings->get("HIGH")->getDouble();
    low = _settings->get("LOW")->getDouble();
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

      Setting *set = _settings->get("DATE");
      if (! set)
	break;
      set->setDateTime(dt);

      map = plot()->canvasMap(QwtPlot::yRight);
      set = _settings->get("HIGH");
      if (! set)
	break;
      set->setDouble(map.invTransform((double) p.y()));

      if (_createFlag)
      {
        set = _settings->get("DATE_2");
        if (! set)
	  break;
        set->setDateTime(dt);
        set = _settings->get("LOW");
        if (! set)
	  break;
        set->setDouble(_settings->get("HIGH")->getDouble());
      }

      plot()->replot();

      QStringList l;
      l << _settings->get("DATE")->toString();
      Strip strip;
      QString ts;
      strip.strip(_settings->get("HIGH")->getDouble(), 4, ts);
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
      set = _settings->get("LOW");
      if (! set)
	break;
      set->setDouble(map.invTransform((double) p.y()));

      plot()->replot();

      QStringList l;
      l << _settings->get("DATE_2")->toString();
      Strip strip;
      QString ts;
      strip.strip(_settings->get("LOW")->getDouble(), 4, ts);
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

int ChartObjectRetracement::create ()
{
  _status = _MOVE;
  _createFlag = 1;
  setSelected(TRUE);
  g_parent->statusBar()->showMessage(QObject::tr("Select highest starting point..."));
  return 0;
}
