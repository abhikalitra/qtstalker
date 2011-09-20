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
#include "SettingDouble.h"
#include "SettingDateTime.h"
#include "SettingString.h"
#include "SettingInteger.h"
#include "SettingColor.h"
#include "SettingBool.h"

#include <QDebug>
#include <QPolygon>
#include <QSettings>
#include <QStatusBar>
#include <qwt_plot.h>

ChartObjectRetracement::ChartObjectRetracement ()
{
  _createFlag = 0;

  _settings->set(ChartObjectData::_TYPE, new SettingString(QString("Retracement")));
  _settings->set(ChartObjectData::_DATE, new SettingDateTime(QDateTime::currentDateTime()));
  _settings->set(ChartObjectData::_DATE2, new SettingDateTime(QDateTime::currentDateTime()));
  _settings->set(ChartObjectData::_HIGH, new SettingDouble(0));
  _settings->set(ChartObjectData::_LOW, new SettingDouble(0));
  _settings->set(ChartObjectData::_COLOR, new SettingColor(QColor(Qt::red)));
  _settings->set(ChartObjectData::_Z, new SettingInteger(1));
  _settings->set(ChartObjectData::_PEN, new SettingInteger(1));
  _settings->set(ChartObjectData::_EXTEND, new SettingBool(FALSE));
  _settings->set(ChartObjectData::_LINE1, new SettingDouble(0.382));
  _settings->set(ChartObjectData::_LINE2, new SettingDouble(0.5));
  _settings->set(ChartObjectData::_LINE3, new SettingDouble(0.618));
  _settings->set(ChartObjectData::_LINE4, new SettingDouble(0));
  _settings->set(ChartObjectData::_LINE5, new SettingDouble(0));
  _settings->set(ChartObjectData::_LINE6, new SettingDouble(0));
}

void ChartObjectRetracement::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings->get(ChartObjectData::_DATE)->toDateTime()));

  QDateTime dt = _settings->get(ChartObjectData::_DATE2)->toDateTime();
  if (_settings->get(ChartObjectData::_EXTEND)->toBool())
    dsd->date(dsd->count() - 1, dt);

  int x2 = xMap.transform(dsd->x(dt));

  p->setPen(_settings->get(ChartObjectData::_COLOR)->toColor());

  _selectionArea.clear();

  QList<double> lineList;
  lineList.append(_settings->get(ChartObjectData::_LINE1)->toDouble());
  lineList.append(_settings->get(ChartObjectData::_LINE2)->toDouble());
  lineList.append(_settings->get(ChartObjectData::_LINE3)->toDouble());
  lineList.append(_settings->get(ChartObjectData::_LINE4)->toDouble());
  lineList.append(_settings->get(ChartObjectData::_LINE5)->toDouble());
  lineList.append(_settings->get(ChartObjectData::_LINE6)->toDouble());

  int loop;
  for (loop = 0; loop < lineList.count(); loop++)
  {
    double td = lineList[loop];
    if (td != 0)
    {
      double range = _settings->get(ChartObjectData::_HIGH)->toDouble() - _settings->get(ChartObjectData::_LOW)->toDouble();
      double r = 0;
      if (td < 0)
        r = _settings->get(ChartObjectData::_LOW)->toDouble() + (range * td);
      else
      {
        if (td > 0)
          r = _settings->get(ChartObjectData::_LOW)->toDouble() + (range * td);
        else
        {
          if (td < 0)
            r = _settings->get(ChartObjectData::_HIGH)->toDouble();
          else
            r = _settings->get(ChartObjectData::_LOW)->toDouble();
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
  int y = yMap.transform(_settings->get(ChartObjectData::_LOW)->toDouble());
  p->drawLine (x, y, x2, y);
  Strip strip;
  QString ts;
  strip.strip(_settings->get(ChartObjectData::_LOW)->toDouble(), 4, ts);
  p->drawText(x, y - 1, "0% - " + ts);

  // store the selectable area the low line occupies
  QPolygon array;
  array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
  _selectionArea.append(QRegion(array));

  // draw the high line
  int y2 = yMap.transform(_settings->get(ChartObjectData::_HIGH)->toDouble());
  p->drawLine (x, y2, x2, y2);
  strip.strip(_settings->get(ChartObjectData::_HIGH)->toDouble(), 4, ts);
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
		_settings->get(ChartObjectData::_COLOR)->toColor());

    //bottom right corner
    _grabHandles.append(QRegion(x2, y - (_handleWidth / 2),
		                _handleWidth,
		                _handleWidth,
		                QRegion::Rectangle));

    p->fillRect(x2,
		y - (_handleWidth / 2),
		_handleWidth,
		_handleWidth,
		_settings->get(ChartObjectData::_COLOR)->toColor());
  }
}

int ChartObjectRetracement::info (Message &info)
{
  info.insert(QObject::tr("Type"), _settings->get(ChartObjectData::_TYPE)->toString());

  QDateTime dt = _settings->get(ChartObjectData::_DATE)->toDateTime();
  info.insert("SD", dt.toString("yyyy-MM-dd"));
  info.insert("ST", dt.toString("HH:mm:ss"));

  dt = _settings->get(ChartObjectData::_DATE2)->toDateTime();
  info.insert("ED", dt.toString("yyyy-MM-dd"));
  info.insert("ET", dt.toString("HH:mm:ss"));

  info.insert(QObject::tr("High"), _settings->get(ChartObjectData::_HIGH)->toString());
  info.insert(QObject::tr("Low"), _settings->get(ChartObjectData::_LOW)->toString());
  info.insert(QObject::tr("Level 1"), _settings->get(ChartObjectData::_LINE1)->toString());
  info.insert(QObject::tr("Level 2"), _settings->get(ChartObjectData::_LINE2)->toString());
  info.insert(QObject::tr("Level 3"), _settings->get(ChartObjectData::_LINE3)->toString());
  info.insert(QObject::tr("Level 4"), _settings->get(ChartObjectData::_LINE4)->toString());
  info.insert(QObject::tr("Level 5"), _settings->get(ChartObjectData::_LINE5)->toString());
  info.insert(QObject::tr("Level 6"), _settings->get(ChartObjectData::_LINE6)->toString());

  return 0;
}

int ChartObjectRetracement::highLow (int start, int end, double &high, double &low)
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  if (! dsd)
    return 1;

  int x = dsd->x(_settings->get(ChartObjectData::_DATE)->toDateTime());
  if (x >= start && x <= end)
  {
    high = _settings->get(ChartObjectData::_HIGH)->toDouble();
    low = _settings->get(ChartObjectData::_LOW)->toDouble();
    return 0;
  }

  int x2 = dsd->x(_settings->get(ChartObjectData::_DATE2)->toDateTime());
  if (x2 >= start && x2 <= end)
  {
    high = _settings->get(ChartObjectData::_HIGH)->toDouble();
    low = _settings->get(ChartObjectData::_LOW)->toDouble();
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
      _settings->set(ChartObjectData::_DATE, new SettingDateTime(dt));

      map = plot()->canvasMap(QwtPlot::yRight);
      _settings->set(ChartObjectData::_HIGH, new SettingDouble(map.invTransform((double) p.y())));

      if (_createFlag)
      {
        _settings->set(ChartObjectData::_DATE2, new SettingDateTime(dt));
        _settings->set(ChartObjectData::_LOW, new SettingDouble(_settings->get(ChartObjectData::_HIGH)->toDouble()));
      }

      plot()->replot();

      QStringList l;
      l << _settings->get(ChartObjectData::_DATE)->toString();
      Strip strip;
      QString ts;
      strip.strip(_settings->get(ChartObjectData::_HIGH)->toDouble(), 4, ts);
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
      _settings->set(ChartObjectData::_DATE2, new SettingDateTime(dt));

      map = plot()->canvasMap(QwtPlot::yRight);
      _settings->set(ChartObjectData::_LOW, new SettingDouble(map.invTransform((double) p.y())));

      plot()->replot();

      QStringList l;
      l << _settings->get(ChartObjectData::_DATE2)->toString();
      Strip strip;
      QString ts;
      strip.strip(_settings->get(ChartObjectData::_LOW)->toDouble(), 4, ts);
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

int ChartObjectRetracement::create ()
{
  _status = _MOVE;
  _createFlag = 1;
  setSelected(TRUE);
  g_parent->statusBar()->showMessage(QObject::tr("Select highest starting point..."));
  return 0;
}

DataDialog * ChartObjectRetracement::dialog (QWidget *p)
{
  DataDialog *dialog = new DataDialog(p);

  QStringList l;
  l << "QtStalker" + g_session + ":" << QObject::tr("Edit Retracement");
  dialog->setWindowTitle(l.join(" "));

  int tab = 0;
  dialog->addTab(tab, QObject::tr("Settings"));

  dialog->setDateTime(tab,
                      QObject::tr("Start Date"),
                      _settings->get(ChartObjectData::_DATE)->toDateTime(),
                      QString());

  dialog->setDateTime(tab,
                      QObject::tr("End Date"),
                      _settings->get(ChartObjectData::_DATE2)->toDateTime(),
                      QString());

  dialog->setColor(tab,
                   QObject::tr("Color"),
                   _settings->get(ChartObjectData::_COLOR)->toColor(),
                   QString());

  dialog->setDouble(tab,
                    QObject::tr("High"),
                    _settings->get(ChartObjectData::_HIGH)->toDouble(),
                    99999999.0,
                    -99999999.0,
                    QString());

  dialog->setDouble(tab,
                    QObject::tr("Low"),
                    _settings->get(ChartObjectData::_LOW)->toDouble(),
                    99999999.0,
                    -99999999.0,
                    QString());

  dialog->setBool(tab,
                  QObject::tr("Extend"),
                  _settings->get(ChartObjectData::_EXTEND)->toBool(),
                  QString());

  dialog->setInteger(tab,
                     QString("Z"),
                     _settings->get(ChartObjectData::_Z)->toInteger(),
                     99,
                     -1,
                     QString());

  // tab 2
  tab++;
  dialog->addTab(tab, QObject::tr("Levels"));

  dialog->setDouble(tab,
                    QObject::tr("Level 1"),
                    _settings->get(ChartObjectData::_LINE1)->toDouble(),
                    99999999.0,
                    0.0,
                    QString());

  dialog->setDouble(tab,
                    QObject::tr("Level 2"),
                    _settings->get(ChartObjectData::_LINE2)->toDouble(),
                    99999999.0,
                    0.0,
                    QString());

  dialog->setDouble(tab,
                    QObject::tr("Level 3"),
                    _settings->get(ChartObjectData::_LINE3)->toDouble(),
                    99999999.0,
                    0.0,
                    QString());

  dialog->setDouble(tab,
                    QObject::tr("Level 4"),
                    _settings->get(ChartObjectData::_LINE4)->toDouble(),
                    99999999.0,
                    0.0,
                    QString());

  dialog->setDouble(tab,
                    QObject::tr("Level 5"),
                    _settings->get(ChartObjectData::_LINE5)->toDouble(),
                    99999999.0,
                    0.0,
                    QString());

  dialog->setDouble(tab,
                    QObject::tr("Level 6"),
                    _settings->get(ChartObjectData::_LINE6)->toDouble(),
                    99999999.0,
                    0.0,
                    QString());

  return dialog;
}
